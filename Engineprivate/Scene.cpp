#include "..\public\Scene.h"

Engine::Scene::Scene(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:m_pDevice(pDevice),m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT Engine::Scene::Ready()
{
	return S_OK;
}

_int Engine::Scene::Update(_double TimeDelta)
{
	return _int();
}

HRESULT Engine::Scene::Render()
{
	return S_OK;
}

void Engine::Scene::Free()
{
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
