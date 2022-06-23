#include "stdafx.h"
#include "..\public\TargetCamera.h"
#include "GameInstance.h"
#include "Transform.h"

Client::TargetCamera::TargetCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:Camera(pDevice, pDeviceContext)
{
}

Client::TargetCamera::TargetCamera(const TargetCamera & rhs)
	: Camera(rhs)
{
}

HRESULT Client::TargetCamera::Ready_Prototype()
{
	if (FAILED(__super::Ready_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT Client::TargetCamera::Ready(void * pArg)
{
	if (FAILED(__super::Ready(pArg)))
		return E_FAIL;

	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	m_pTargetTransform = (Transform*)pGameInstance->GetComponent(SCENE_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform"));
	if (nullptr == m_pTargetTransform)
		return E_FAIL;

	_vector vPosition = m_pTargetTransform->GetState(Transform::STATE_POSITION);
	_float y = XMVectorGetY(vPosition) + m_fHeight;
	_float z = XMVectorGetZ(vPosition) + m_fDist;

	m_pTransform->SetState(Transform::STATE_POSITION, XMVectorSet(XMVectorGetX(vPosition), y, z, 1));

	m_vForward = XMVectorSet(0, 0, 1, 0);

	RELEASE_INSTANCE(GameInstance);
	return S_OK;
}

_int Client::TargetCamera::Update(_double TimeDelta)
{
	GameInstance*	pGameInstance = GET_INSTANCE(GameInstance);

	//_long	MouseMove = 0; //마우스 감도


	if (GetKeyState('Q') & 0x8000)
	{
		m_pTransform->RotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f),-TimeDelta *m_fRotSpeed * 0.1f);
		m_isRot = true;
	}
	else if (GetKeyState('E') & 0x8000) 
	{
		m_pTransform->RotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta *m_fRotSpeed * 0.1f);
		m_isRot = true;
	}

	if (m_isRot) 
	{
		m_vForward = XMVector3Normalize(XMVector3Cross(XMVector3Normalize(m_pTransform->GetState(Transform::STATE_RIGHT)), XMVectorSet(0, 1, 0, 0)));
		m_isRot = false;
	}
	

	/*if (MouseMove = pGameInstance->Get_MouseMoveState(Input_Device::MMS_X))
	{
		m_pTransform->RotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * MouseMove * 0.1f);
	}

	if (MouseMove = pGameInstance->Get_MouseMoveState(Input_Device::MMS_Y))
	{
		m_pTransform->RotationAxis(m_pTransform->GetState(Transform::STATE_RIGHT), TimeDelta * MouseMove * 0.1f);
	}*/

	RELEASE_INSTANCE(GameInstance);

	return Camera::Update(TimeDelta);
}

_int Client::TargetCamera::LateUpdate(_double TimeDelta)
{
	_vector vPosition = m_pTargetTransform->GetState(Transform::STATE_POSITION);
	_float y = (XMVectorGetY(vPosition) + m_fHeight);
//	_float z = (XMVectorGetZ(vPosition) + m_fDist);
	vPosition = XMVectorSet(XMVectorGetX(vPosition), y, XMVectorGetZ(vPosition), 1);
	vPosition += (m_vForward* m_fDist);
	

	m_pTransform->SetState(Transform::STATE_POSITION, vPosition);
	
	//_vector	vLook = m_pTargetTransform->GetState(Transform::STATE_POSITION) - vPosition;
	//vLook = XMVector3Normalize(vLook);

	//m_pTransform->SetState(Transform::STATE_LOOK, vLook);


	return _int();
}

HRESULT Client::TargetCamera::Render()
{
	return S_OK;
}

TargetCamera * Client::TargetCamera::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	TargetCamera* pInstance = new TargetCamera(pDevice, pDeviceContext);
	if (FAILED(pInstance->Ready_Prototype()))
	{
		MSGBOX("Failed to Creating TargetCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

GameObject * Client::TargetCamera::Clone(void * pArg)
{
	TargetCamera* pInstance = new TargetCamera(*this);
	if (FAILED(pInstance->Ready(pArg)))
	{
		MSGBOX("Failed to Creating TargetCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::TargetCamera::Free()
{
	__super::Free();
}
