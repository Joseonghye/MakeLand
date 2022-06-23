#include "stdafx.h"
#include "..\public\ZiodyneEffect.h"
#include "GameInstance.h"

Client::ZiodyneEffect::ZiodyneEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:UIInGame(pDevice,pDeviceContext)
{
}

Client::ZiodyneEffect::ZiodyneEffect(const UIInGame & rhs)
	: UIInGame(rhs)
{
}

HRESULT Client::ZiodyneEffect::Ready_Prototype()
{
	if (FAILED(__super::Ready_Prototype()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT Client::ZiodyneEffect::Ready(void * pArg)
{
	if (FAILED(SetUpComponents()))
		return E_FAIL;
	 
	if (FAILED(__super::Ready(pArg)))
		return E_FAIL;

	m_iAllFrameCount = 6;
	m_iCountX = 3;
	m_iCountY = 2;
	m_fEffectPlaySpeed = 10.f;

	return S_OK;
}

_int Client::ZiodyneEffect::Update(_double TimeDelta)
{
	//빌보드 , 프레임계산
	return __super::Update(TimeDelta);
}

_int Client::ZiodyneEffect::LateUpdate(_double TimeDelta)
{
	return __super::LateUpdate(TimeDelta);
}

HRESULT Client::ZiodyneEffect::Render()
{
	return __super::Render();
}

HRESULT Client::ZiodyneEffect::SetUpComponents()
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
	if (FAILED(__super::SetUpComponents(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_Ziodyne"), TEXT("Com_Texture"), (Component**)&m_pTexture)))
		return E_FAIL;

	return S_OK;
}

void Client::ZiodyneEffect::Free()
{
	__super::Free();
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);
	Safe_Release(m_pRect);
	Safe_Release(m_pRenderer);
}

ZiodyneEffect * Client::ZiodyneEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	ZiodyneEffect* pInstance = new ZiodyneEffect(pDevice, pDeviceContext);

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MSGBOX("Failed to Creating ZiodyneEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

ZiodyneEffect * Client::ZiodyneEffect::Clone(void * pArg)
{
	ZiodyneEffect* pInstance = new ZiodyneEffect(*this);

	if (FAILED(pInstance->Ready(pArg)))
	{
		MSGBOX("Failed to Creating ZiodyneEffect Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}
