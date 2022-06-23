#include "stdafx.h"
#include "..\public\AgidyneEffect.h"
#include "GameInstance.h"

Client::AgidyneEffect::AgidyneEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:UIInGame(pDevice, pDeviceContext)
{
}

Client::AgidyneEffect::AgidyneEffect(const UIInGame & rhs)
	: UIInGame(rhs)
{
}

HRESULT Client::AgidyneEffect::Ready_Prototype()
{
	if (FAILED(__super::Ready_Prototype()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT Client::AgidyneEffect::Ready(void * pArg)
{
	if (FAILED(SetUpComponents()))
		return E_FAIL;

	if (FAILED(__super::Ready(pArg)))
		return E_FAIL;

	m_iAllFrameCount = 9;
	m_iCountX = 3;
	m_iCountY = 3;
	m_fEffectPlaySpeed = 10.f;

	return S_OK;
}

_int Client::AgidyneEffect::Update(_double TimeDelta)
{
	//빌보드 , 프레임계산
	return __super::Update(TimeDelta);
}

_int Client::AgidyneEffect::LateUpdate(_double TimeDelta)
{
	return __super::LateUpdate(TimeDelta);
}

HRESULT Client::AgidyneEffect::Render()
{
	return __super::Render();
}

HRESULT Client::AgidyneEffect::SetUpComponents()
{
	Transform::TRANSFORMDESC transDesc;
	transDesc.fSpeedPerSec = 7.f;
	transDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUpComponents(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (Component**)&m_pTransform, &transDesc)))
		return E_FAIL;
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (Component**)&m_pRenderer)))
		return E_FAIL;
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, TEXT("Prototype_Component_RectBuffer"), TEXT("Com_Rect"), (Component**)&m_pRect)))
		return E_FAIL;
	if (FAILED(__super::SetUpComponents(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_Agidyne"), TEXT("Com_Texture"), (Component**)&m_pTexture)))
		return E_FAIL;

	return S_OK;
}

void Client::AgidyneEffect::Free()
{
	__super::Free();
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);
	Safe_Release(m_pRect);
	Safe_Release(m_pRenderer);
}

AgidyneEffect * Client::AgidyneEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	AgidyneEffect* pInstance = new AgidyneEffect(pDevice, pDeviceContext);

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MSGBOX("Failed to Creating AgidyneEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

AgidyneEffect * Client::AgidyneEffect::Clone(void * pArg)
{
	AgidyneEffect* pInstance = new AgidyneEffect(*this);

	if (FAILED(pInstance->Ready(pArg)))
	{
		MSGBOX("Failed to Creating AgidyneEffect Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}
