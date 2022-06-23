#include "..\public\LightMgr.h"
#include "Light.h"

IMPLEMENT_SINGLETON(LightMgr)

LightMgr::LightMgr()
{

}

const LIGHTDESC * Engine::LightMgr::GetLightDesc(_uint iIndex)
{
	LIGHTS::iterator iter = m_Lights.begin();
	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->GetLightDesc();
}

HRESULT Engine::LightMgr::AddLight(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext,const LIGHTDESC& LightDesc)
{
	Light* pLight = Light::Create(pDevice, pDeviceContext,LightDesc);
	if (nullptr == pLight)
		return E_FAIL;

	m_Lights.push_back(pLight);

	return S_OK;
}

HRESULT Engine::LightMgr::Render_Lights()
{
	for (auto& pLight : m_Lights)
		pLight->Render();

	return S_OK;
}

void Engine::LightMgr::Free()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight);
	m_Lights.clear();
}
