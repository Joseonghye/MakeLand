#include "..\public\Component.h"

Engine::Component::Component(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)//, COMPONENT_ID ID)
	:m_pDevice(pDevice), m_pDeviceContext(pDeviceContext),
	m_isCloned(false)//, m_eID(ID)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

Engine::Component::Component(const Component & rhs)
	:m_pDevice(rhs.m_pDevice), m_pDeviceContext(rhs.m_pDeviceContext),
	m_isCloned(true)//, m_eID(rhs.m_eID)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT Engine::Component::Ready_Prototype()
{
	return S_OK;
}

HRESULT Engine::Component::Ready(void * pArg)
{
	return S_OK;
}

void Engine::Component::Free()
{
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
