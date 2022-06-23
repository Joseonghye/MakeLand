#include "..\public\Camera.h"

Engine::Camera::Camera(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:GameObject(pDevice, pDeviceContext), m_pTransform(nullptr)
{
}

Engine::Camera::Camera(const Camera & rhs)
	: GameObject(rhs)
{
}

HRESULT Engine::Camera::Ready_Prototype()
{
	if (FAILED(__super::Ready_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT Engine::Camera::Ready(void* pArg)
{
	if (FAILED(__super::Ready(pArg)))
		return E_FAIL;

	m_pTransform = Transform::Create(m_pDevice, m_pDeviceContext);
	if (nullptr == m_pTransform)
		return E_FAIL;

	if (nullptr != pArg)
		memcpy(&m_CameraDesc, pArg, sizeof(CAMERADESC));

	// 카메라 초기상태
	_vector	vPosition = XMLoadFloat3(&m_CameraDesc.vEye);
	vPosition = XMVectorSetW(vPosition, 1.f);

	_vector	vLook = XMLoadFloat3(&m_CameraDesc.vAt) - XMLoadFloat3(&m_CameraDesc.vEye);
	vLook = XMVector3Normalize(vLook);

	_vector	vRight = XMVector3Cross(XMLoadFloat3(&m_CameraDesc.vAxisY), vLook);
	vRight = XMVector3Normalize(vRight);

	_vector	vUp = XMVector3Cross(vLook, vRight);
	vUp = XMVector3Normalize(vUp);

	m_pTransform->SetState(Transform::STATE_RIGHT, vRight);
	m_pTransform->SetState(Transform::STATE_UP, vUp);
	m_pTransform->SetState(Transform::STATE_LOOK, vLook);
	m_pTransform->SetState(Transform::STATE_POSITION, vPosition);

	m_pTransform->SetTransformDesc(m_CameraDesc.TransformDesc);

	return S_OK;
}

_int Engine::Camera::Update(_double TimeDelta)
{
	PipeLine* pPipeLine = GET_INSTANCE(PipeLine);
	//파이프라인에게 값 전달
	//카메라 월드 행렬 역행렬 == 뷰페이스 변환 행렬
	pPipeLine->SetTransform(PipeLine::D3DTS_VIEW, m_pTransform->GetWorldMatrixInverse());

	pPipeLine->SetTransform(PipeLine::D3DTS_PROJECTION, XMMatrixPerspectiveFovLH(m_CameraDesc.fFovy, m_CameraDesc.fAspect, m_CameraDesc.fNear, m_CameraDesc.fFar));

	RELEASE_INSTANCE(PipeLine);
	return _int();
}

_int Engine::Camera::LateUpdate(_double TimeDelta)
{
	return _int();
}

HRESULT Engine::Camera::Render()
{
	return S_OK;
}

void Engine::Camera::Free()
{
	__super::Free();

	Safe_Release(m_pTransform);
}
