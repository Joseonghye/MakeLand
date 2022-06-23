#include "..\public\Renderer.h"
#include "GameObject.h"
#include "TargetMgr.h"
#include "ViewPortBuffer.h"
#include "LightMgr.h"

Engine::Renderer::Renderer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:Component(pDevice, pDeviceContext),
	m_pTargetMgr(TargetMgr::GetInstance())
{
	Safe_AddRef(m_pTargetMgr);
}

HRESULT Engine::Renderer::Ready_Prototype()
{
	if (nullptr == m_pTargetMgr)
		return E_FAIL;

	_uint iViewportIndex = 1;
	D3D11_VIEWPORT ViewportDesc;

	m_pDeviceContext->RSGetViewports(&iViewportIndex, &ViewportDesc);

	if (FAILED(m_pTargetMgr->AddRenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Diffuse"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->AddRenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Normal"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->AddRenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Depth"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->AddRenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Shade"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->AddRenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Specular"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pTargetMgr->AddMRT(TEXT("MRT_Deferred"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->AddMRT(TEXT("MRT_Deferred"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->AddMRT(TEXT("MRT_Deferred"), TEXT("Target_Depth"))))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->AddMRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->AddMRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
		return E_FAIL;

	m_pVIBuffer = ViewPortBuffer::Create(m_pDevice, m_pDeviceContext, 0.f, 0.f, ViewportDesc.Width, ViewportDesc.Height, TEXT("../Bin/ShaderFiles/Shader_RectViewPort.hlsl"));
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(m_pTargetMgr->Ready_DebugBuffer(TEXT("Target_Diffuse"), 0.f, 0.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->Ready_DebugBuffer(TEXT("Target_Normal"), 0.f, 200.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->Ready_DebugBuffer(TEXT("Target_Depth"), 0.f, 400.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->Ready_DebugBuffer(TEXT("Target_Shade"), 200.f, 0.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->Ready_DebugBuffer(TEXT("Target_Specular"), 200.f, 200.f, 200.f, 200.f)))
		return E_FAIL;
#endif // _DEBUG


	return S_OK;
}

HRESULT Engine::Renderer::Ready(void * pArg)
{
	return S_OK;
}


HRESULT Engine::Renderer::AddRenderGroup(RENDER eRenderID, GameObject* pGameObject)
{
	if (nullptr == pGameObject ||
		eRenderID >= RENDER_END)
		return E_FAIL;

	m_RenderGroup[eRenderID].push_back(pGameObject);

	Safe_AddRef(pGameObject);

	return S_OK;
}

HRESULT Engine::Renderer::DrawRenderGroup()
{
	//for (_uint i = 0; i < RENDER_END; ++i)
	//{
	//	for (auto& pGameObject : m_RenderGroup[i])
	//	{
	//		if (nullptr != pGameObject && pGameObject->GetActive())
	//			pGameObject->Render();

	//		Safe_Release(pGameObject);
	//	}
	//	m_RenderGroup[i].clear();
	//}

	if (FAILED(Render_Priority()))
		return E_FAIL;
	if (FAILED(Render_NonAlpha()))
		return E_FAIL;

	if (FAILED(Render_LightAcc()))
		return E_FAIL;

	if (FAILED(Render_Blend()))
		return E_FAIL;

	if (FAILED(Render_Alpha()))
		return E_FAIL;
	if (FAILED(Render_UI()))
		return E_FAIL;

#ifdef _DEBUG
	//if (FAILED(m_pTargetMgr->Render_DebugBuffer(TEXT("MRT_Deferred"))))
	//	return E_FAIL;
	//if (FAILED(m_pTargetMgr->Render_DebugBuffer(TEXT("MRT_LightAcc"))))
	//	return E_FAIL;
#endif // _DEBUG

	return S_OK;
}

HRESULT Engine::Renderer::Render_Priority()
{
	for (auto& pGameObject : m_RenderGroup[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject && pGameObject->GetActive())
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderGroup[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT Engine::Renderer::Render_NonAlpha()
{
	if (nullptr == m_pTargetMgr)
		return E_FAIL;

	/*  Target_Diffuse + Target_Normal 를 장치에 바인드하였다. */
	if (FAILED(m_pTargetMgr->BeginMRT(m_pDeviceContext, TEXT("MRT_Deferred"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderGroup[RENDER_NONALPHA])
	{
		if (nullptr != pGameObject && pGameObject->GetActive())
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderGroup[RENDER_NONALPHA].clear();

	if (FAILED(m_pTargetMgr->EndMRT(m_pDeviceContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT Engine::Renderer::Render_Alpha()
{
	for (auto& pGameObject : m_RenderGroup[RENDER_ALPHA])
	{
		if (nullptr != pGameObject && pGameObject->GetActive())
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderGroup[RENDER_ALPHA].clear();

	return S_OK;
}

HRESULT Engine::Renderer::Render_UI()
{
	for (auto& pGameObject : m_RenderGroup[RENDER_UI])
	{
		if (nullptr != pGameObject && pGameObject->GetActive())
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderGroup[RENDER_UI].clear();

	return S_OK;
}

HRESULT Engine::Renderer::Render_LightAcc()
{
	LightMgr*		pLightMgr = GET_INSTANCE(LightMgr);

	/*  Target_Shader를 장치에 바인드하였다. */
	m_pTargetMgr->BeginMRT(m_pDeviceContext, TEXT("MRT_LightAcc"));

	pLightMgr->Render_Lights();

	m_pTargetMgr->EndMRT(m_pDeviceContext);

	RELEASE_INSTANCE(LightMgr);

	return S_OK;
}

HRESULT Engine::Renderer::Render_Blend()
{
	if (FAILED(m_pVIBuffer->SetUpTextureOnShader("g_DiffuseTexture", m_pTargetMgr->GetSRV(TEXT("Target_Diffuse")))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUpTextureOnShader("g_ShadeTexture", m_pTargetMgr->GetSRV(TEXT("Target_Shade")))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUpTextureOnShader("g_SpecularTexture", m_pTargetMgr->GetSRV(TEXT("Target_Specular")))))
		return E_FAIL;

	m_pVIBuffer->Render(3);

	return S_OK;
}

Renderer * Engine::Renderer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	Renderer* pInstance = new Renderer(pDevice, pDeviceContext);
	if (FAILED(pInstance->Ready_Prototype()))
	{
		MSGBOX("Failed to Creating Renderer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

Component * Engine::Renderer::Clone(void * pArg)
{
	AddRef();
	return this;
}

void Engine::Renderer::Free()
{
	__super::Free();

	for (_uint i = 0; i < RENDER_END; ++i)
	{
		for (auto& pGameObject : m_RenderGroup[i])
			Safe_Release(pGameObject);

		m_RenderGroup[i].clear();
	}
	Safe_Release(m_pTargetMgr);
}
