#include "..\public\TargetMgr.h"
#include "RenderTarget.h"

IMPLEMENT_SINGLETON(TargetMgr)

Engine::TargetMgr::TargetMgr()
{
}

ID3D11ShaderResourceView * Engine::TargetMgr::GetSRV(const _tchar * pTargetTag)
{
	RenderTarget*	pRenderTarget = FindTarget(pTargetTag);
	if (nullptr == pRenderTarget)
		return nullptr;

	return pRenderTarget->GetSTV();
}

HRESULT Engine::TargetMgr::AddRenderTarget(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar * pTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor)
{
	if (nullptr != FindTarget(pTargetTag))
		return E_FAIL;

	RenderTarget* pRenderTarget = RenderTarget::Create(pDevice, pDeviceContext, iWidth, iHeight, eFormat, vClearColor);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_Targets.emplace(pTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT Engine::TargetMgr::AddMRT(const _tchar * pMRTTag, const _tchar * pTargetTag)
{
	RenderTarget* pRenderTarget = FindTarget(pTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	vector<RenderTarget*>*	pMRTList = FindMRT(pMRTTag);

	if (nullptr == pMRTList)
	{
		vector<RenderTarget*>		MRTList;
		MRTList.push_back(pRenderTarget);

		m_MRTs.emplace(pMRTTag, MRTList);
	}
	else
		pMRTList->push_back(pRenderTarget);

	Safe_AddRef(pRenderTarget);

	return S_OK;
}

HRESULT Engine::TargetMgr::BeginMRT(ID3D11DeviceContext * pDeviceContext, const _tchar * pMRTTag)
{
	vector<RenderTarget*>* pMRTVec = FindMRT(pMRTTag);

	ID3D11RenderTargetView*	RTVs[8] = { nullptr };

	pDeviceContext->OMGetRenderTargets(1, &m_pOldView, &m_pDepthStencilView);

	_uint iNumViews = pMRTVec->size();
		  
	for(int i=0; i<iNumViews; i++)
	{
		(*pMRTVec)[i]->Clear();
		RTVs[i] = (*pMRTVec)[i]->GetRTV();
	}

	pDeviceContext->OMSetRenderTargets(iNumViews, RTVs, m_pDepthStencilView);

	return S_OK;
}

HRESULT Engine::TargetMgr::EndMRT(ID3D11DeviceContext * pDeviceContext)
{
	pDeviceContext->OMSetRenderTargets(1, &m_pOldView, m_pDepthStencilView);
	return S_OK;
}

#ifdef _DEBUG
HRESULT Engine::TargetMgr::Ready_DebugBuffer(const _tchar * pTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	RenderTarget* pRenderTarget = FindTarget(pTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Ready_DebugBuffer(fX, fY, fSizeX, fSizeY);
}

HRESULT Engine::TargetMgr::Render_DebugBuffer(const _tchar * pMRTTag)
{
	vector<RenderTarget*>*		pMRTList = FindMRT(pMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	for (auto& pRenderTarget : *pMRTList)
		pRenderTarget->Render_DebugBuffer();

	return S_OK;
}
#endif // _DEBUG

RenderTarget * Engine::TargetMgr::FindTarget(const _tchar * pTargetTag)
{
	auto	iter = find_if(m_Targets.begin(), m_Targets.end(), Tag_Finder(pTargetTag));
	if (iter == m_Targets.end())
		return nullptr;

	return iter->second;
}

vector<class RenderTarget*>* Engine::TargetMgr::FindMRT(const _tchar * pMRTTag)
{
	auto	iter = find_if(m_MRTs.begin(), m_MRTs.end(), Tag_Finder(pMRTTag));
	if (iter == m_MRTs.end())
		return nullptr;

	return &iter->second;
}

void Engine::TargetMgr::Free()
{
	for (auto& Pair : m_MRTs)
	{
		for (auto& pRenderTarget : Pair.second)
			Safe_Release(pRenderTarget);
		Pair.second.clear();
	}
	m_MRTs.clear();

	for (auto& Pair : m_Targets)
		Safe_Release(Pair.second);

	m_Targets.clear();
}
