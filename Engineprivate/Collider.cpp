#include "..\public\Collider.h"
#include "PipeLine.h"

Engine::Collider::Collider(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:Component(pDevice, pDeviceContext)
{
}

Engine::Collider::Collider(const Collider & rhs)
	: Component(rhs)
	, m_pEffect(rhs.m_pEffect)
	, m_pBatch(rhs.m_pBatch)
	, m_pInputLayout(rhs.m_pInputLayout)
	, m_vColor(rhs.m_vColor)
{
	Safe_AddRef(m_pInputLayout);
}

HRESULT Engine::Collider::Ready_Prototype()
{
	m_pEffect = new BasicEffect(m_pDevice);
	m_pEffect->SetVertexColorEnabled(true);

	const void*		pShaderByteCode = nullptr;
	size_t			ShaderByteCodeLength = 0;
	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &ShaderByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, ShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;

	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pDeviceContext);


	return S_OK;
}

HRESULT Engine::Collider::Ready(void * pArg)
{
	COLLIDERDESC*	pColliderDesc = (COLLIDERDESC*)pArg;

	_matrix WorldMat;
	WorldMat= XMMatrixIdentity();
	WorldMat.r[0] *= pColliderDesc->vScale.x;
	WorldMat.r[1] *= pColliderDesc->vScale.y;
	WorldMat.r[2] *= pColliderDesc->vScale.z;

	_matrix	RotX = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), pColliderDesc->vRotation.x);
	_matrix	RotY = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), pColliderDesc->vRotation.y);
	_matrix	RotZ = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), pColliderDesc->vRotation.z);
	_matrix RotMat = RotX * RotY * RotZ;
	WorldMat *= RotMat;

	WorldMat.r[3] = XMLoadFloat3(&pColliderDesc->vPosition);

	XMStoreFloat4x4(&m_TransformMatrix, WorldMat);

	m_pOrign = new BoundingBox(XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.5f, 0.5f, 0.5f));
	m_pOrign->Transform(*m_pOrign, WorldMat);

	m_pCollider = new BoundingBox;

	return S_OK;
}

void Engine::Collider::Update(_fmatrix TransformMatrix)
{
	m_pOrign->Transform(*m_pCollider, RemoveScale(TransformMatrix));

	m_vMin= _float4(m_pCollider->Center.x - m_pCollider->Extents.x,
		m_pCollider->Center.y - m_pCollider->Extents.y,
		m_pCollider->Center.z - m_pCollider->Extents.z,
		1.f);

	m_vMax = _float4(m_pCollider->Center.x + m_pCollider->Extents.x,
		m_pCollider->Center.y + m_pCollider->Extents.y,
		m_pCollider->Center.z + m_pCollider->Extents.z,
		1.f);
}

HRESULT Engine::Collider::Render()
{
	m_vColor = m_isCollision == true ? _float4(1.f, 0.f, 0.f, 1.f) : _float4(0.f, 1.f, 0.f, 1.f);

	PipeLine* pPipeLine = GET_INSTANCE(PipeLine);
	m_pEffect->SetView(pPipeLine->GetTransform(PipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(pPipeLine->GetTransform(PipeLine::D3DTS_PROJECTION));

	m_pDeviceContext->IASetInputLayout(m_pInputLayout);

	m_pEffect->Apply(m_pDeviceContext);

	m_pBatch->Begin();

	DX::Draw(m_pBatch, *m_pCollider, XMLoadFloat4(&m_vColor));

	m_pBatch->End();
	RELEASE_INSTANCE(PipeLine);
	return S_OK;
}

_bool Engine::Collider::Collision(Collider * pTarget)
{
	_vector		vTargetMin, vTargetMax;
	vTargetMin = pTarget->GetMin();
	vTargetMax = pTarget->GetMax();

	m_isCollision = false;

	if (max(m_vMin.x, XMVectorGetX(vTargetMin)) >
		min(m_vMax.x, XMVectorGetX(vTargetMax)))
		return false;

	if (max(m_vMin.y, XMVectorGetY(vTargetMin)) >
		min(m_vMax.y, XMVectorGetY(vTargetMax)))
		return false;

	if (max(m_vMin.z, XMVectorGetZ(vTargetMin)) >
		min(m_vMax.z, XMVectorGetZ(vTargetMax)))
		return false;

	m_isCollision = true;

	return true;
}

_fmatrix Engine::Collider::RemoveScale(_fmatrix TransformMatrix)
{
	_matrix mat = TransformMatrix;
	mat.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
	mat.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
	mat.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

	return mat;
}

Collider * Engine::Collider::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	Collider* pInstance = new Collider(pDevice, pDeviceContext);
	if (FAILED(pInstance->Ready_Prototype()))
	{
		MSGBOX("Failed to Creating Collider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

Component * Engine::Collider::Clone(void * pArg)
{
	Collider* pInstance = new Collider(*this);
	if (FAILED(pInstance->Ready(pArg)))
	{
		MSGBOX("Failed to Creating Collider Clone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void Engine::Collider::Free()
{
	__super::Free();

	Safe_Release(m_pInputLayout);

	Safe_Delete(m_pBatch);
	Safe_Delete(m_pEffect);
}
