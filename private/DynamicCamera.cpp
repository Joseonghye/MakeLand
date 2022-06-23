#include "stdafx.h"
#include "..\public\DynamicCamera.h"
#include "GameInstance.h"

Client::DynamicCamera::DynamicCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:Camera(pDevice, pDeviceContext)
{
}

Client::DynamicCamera::DynamicCamera(const DynamicCamera & rhs)
	: Camera(rhs)
{
}

HRESULT Client::DynamicCamera::Ready_Prototype()
{
	if (FAILED(__super::Ready_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT Client::DynamicCamera::Ready(void * pArg)
{
	if (FAILED(__super::Ready(pArg)))
		return E_FAIL;
	return S_OK;
}

_int Client::DynamicCamera::Update(_double TimeDelta)
{
	GameInstance*	pGameInstance = GET_INSTANCE(GameInstance);

	if (pGameInstance->Get_DIKeyState(DIK_UP) & 0x80)
	{
		m_pTransform->GoStright(TimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_DOWN) & 0x80)
	{
		m_pTransform->GoBackWard(TimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_LEFT) & 0x80)
	{
		m_pTransform->GoLeft(TimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_RIGHT) & 0x80)
	{
		m_pTransform->GoRight(TimeDelta);
	}

	_long	MouseMove = 0; //마우스 감도

	if (MouseMove = pGameInstance->Get_MouseMoveState(Input_Device::MMS_X))
	{
		m_pTransform->RotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * MouseMove * 0.1f);
	}

	if (MouseMove = pGameInstance->Get_MouseMoveState(Input_Device::MMS_Y))
	{
		m_pTransform->RotationAxis(m_pTransform->GetState(Transform::STATE_RIGHT), TimeDelta * MouseMove * 0.1f);
	}

	RELEASE_INSTANCE(GameInstance);

	return Camera::Update(TimeDelta);
}

_int Client::DynamicCamera::LateUpdate(_double TimeDelta)
{
	return _int();
}

HRESULT Client::DynamicCamera::Render()
{
	return S_OK;
}

DynamicCamera * Client::DynamicCamera::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	DynamicCamera* pInstance = new DynamicCamera(pDevice, pDeviceContext);
	if (FAILED(pInstance->Ready_Prototype()))
	{
		MSGBOX("Failed to Creating DynamicCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

GameObject * Client::DynamicCamera::Clone(void * pArg)
{
	DynamicCamera* pInstance = new DynamicCamera(*this);
	if (FAILED(pInstance->Ready(pArg)))
	{
		MSGBOX("Failed to Creating DynamicCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::DynamicCamera::Free()
{
	__super::Free();
}
