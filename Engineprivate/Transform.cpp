#include "..\public\Transform.h"
#include "GameObject.h"

Engine::Transform::Transform(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:Component(pDevice,pDeviceContext)
{
}

Engine::Transform::Transform(const Transform & rhs)
	: Component(rhs),
	m_WorldMat(rhs.m_WorldMat)
{
}

HRESULT Engine::Transform::Ready_Prototype()
{
	XMStoreFloat4x4(&m_WorldMat, XMMatrixIdentity());
	return S_OK;
}

HRESULT Engine::Transform::Ready(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_TransformDesc, pArg, sizeof(TRANSFORMDESC));

	return S_OK;
}

void Engine::Transform::GoStright(_double TimeDelta)
{
	_vector		vLook = GetState(Transform::STATE_LOOK);
	_vector		vPosition = GetState(Transform::STATE_POSITION);

	vPosition += XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	SetState(Transform::STATE_POSITION, vPosition);
}

void Engine::Transform::GoLeft(_double TimeDelta)
{
	_vector	vRight = GetState(STATE_RIGHT);
	_vector vPos = GetState(STATE_POSITION);

	vPos += XMVector3Normalize(vRight) *-1.f *m_TransformDesc.fSpeedPerSec * _float(TimeDelta);

	SetState(STATE_POSITION, vPos);
}

void Engine::Transform::GoRight(_double TimeDelta)
{
	_vector	vRight = GetState(STATE_RIGHT);
	_vector vPos = GetState(STATE_POSITION);

	vPos += XMVector3Normalize(vRight) *m_TransformDesc.fSpeedPerSec * _float(TimeDelta);

	SetState(STATE_POSITION, vPos);
}

void Engine::Transform::GoBackWard(_double TimeDelta)
{
	_vector	vLook = GetState(STATE_LOOK);
	_vector vPos = GetState(STATE_POSITION);

	vPos += XMVector3Normalize(vLook) * -1 * m_TransformDesc.fSpeedPerSec * _float(TimeDelta);

	SetState(STATE_POSITION, vPos);
}

void Engine::Transform::ChaseTarget(const Transform * pTargetTransform, _double TimeDelta)
{
	_vector vPos = GetState(STATE_POSITION);
	_vector vLook = pTargetTransform->GetState(STATE_POSITION) - vPos;

	vPos += XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * _float(TimeDelta);
	
	LookAt(pTargetTransform->GetState(STATE_POSITION));

	SetState(STATE_POSITION, vPos);
}

void Engine::Transform::LookAt(_fvector vTarget)
{
	_vector vPos = GetState(STATE_POSITION);
	_vector vLook = vTarget - vPos;

	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vRight = XMVector3Cross(vUp, vLook);
	vLook = XMVector3Cross(vRight, vUp);

	//스케일 적용
	vLook = XMVector3Normalize(vLook) * GetScale(STATE_LOOK);
	vRight = XMVector3Normalize(vRight) * GetScale(STATE_RIGHT);

	SetState(STATE_LOOK, vLook);
	SetState(STATE_RIGHT, vRight);
}

void Engine::Transform::RotationAxis(_fvector vAxis, _double TimeDelta)
{
	_matrix RotationMat = XMMatrixRotationAxis(vAxis, m_TransformDesc.fRotationPerSec * TimeDelta);

	_vector vRight =  GetState(STATE_RIGHT);
	_vector vUp = GetState(STATE_UP);
	_vector	vLook = GetState(STATE_LOOK);

	vRight = XMVector4Transform(vRight, RotationMat);
	vUp = XMVector4Transform(vUp, RotationMat);
	vLook = XMVector4Transform(vLook, RotationMat);

	SetState(STATE_RIGHT, vRight);
	SetState(STATE_UP, vUp);
	SetState(STATE_LOOK, vLook);
}

void Engine::Transform::RotationAxis(_fvector vAxis, _float fRadian)
{
	_matrix RotationMat = XMMatrixRotationAxis(vAxis, fRadian);

	_vector vRight = GetState(STATE_RIGHT);
	_vector vUp = GetState(STATE_UP);
	_vector	vLook = GetState(STATE_LOOK);

	vRight = XMVector4Transform(vRight, RotationMat);
	vUp = XMVector4Transform(vUp, RotationMat);
	vLook = XMVector4Transform(vLook, RotationMat);

	SetState(STATE_RIGHT, vRight);
	SetState(STATE_UP, vUp);
	SetState(STATE_LOOK, vLook);
}

void Engine::Transform::SetUpRotation(_fvector vAxis, _float fRadian)
{
	_matrix RotationMat = XMMatrixRotationAxis(vAxis, fRadian);

	_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * GetScale(STATE_RIGHT);
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * GetScale(STATE_UP);
	_vector	vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * GetScale(STATE_LOOK);

	vRight = XMVector4Transform(vRight, RotationMat);
	vUp = XMVector4Transform(vUp, RotationMat);
	vLook = XMVector4Transform(vLook, RotationMat);

	SetState(STATE_RIGHT, vRight);
	SetState(STATE_UP, vUp);
	SetState(STATE_LOOK, vLook);
}

void Engine::Transform::Scaling(_fvector vScale)
{
	_vector vRight = XMVector3Normalize(GetState(STATE_RIGHT)) * XMVectorGetX(vScale);
	_vector vUp = XMVector3Normalize(GetState(STATE_UP)) * XMVectorGetY(vScale);
	_vector	vLook = XMVector3Normalize(GetState(STATE_LOOK)) * XMVectorGetZ(vScale);

	SetState(STATE_RIGHT, vRight);
	SetState(STATE_UP, vUp);
	SetState(STATE_LOOK, vLook);
}

Transform * Engine::Transform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	Transform* pInstance = new Transform(pDevice, pDeviceContext);
	if (FAILED(pInstance->Ready_Prototype()))
	{
		MSGBOX("Failed to Creating Transform");
		Safe_Release(pInstance);
	}
	return pInstance;
}

Component * Engine::Transform::Clone(void * pArg)
{
	Transform* pInstance = new Transform(*this);
	if (FAILED(pInstance->Ready(pArg)))
	{
		MSGBOX("Failed to Creating Transform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Engine::Transform::Free()
{
	__super::Free();
}
