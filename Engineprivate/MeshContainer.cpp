#include "..\public\MeshContainer.h"
#include "HierarchyNode.h"

Engine::MeshContainer::MeshContainer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:m_pDevice(pDevice), m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

Engine::MeshContainer::MeshContainer(const MeshContainer & rhs)
	:m_pDevice(rhs.m_pDevice)
	, m_pDeviceContext(rhs.m_pDeviceContext)
	, m_MeshDesc(rhs.m_MeshDesc),
	m_pParent(nullptr)
{
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pDevice);
}

HRESULT Engine::MeshContainer::Ready(const MESHDESC & MeshDesc)
{
	memcpy(&m_MeshDesc, &MeshDesc, sizeof(MESHDESC));

	return S_OK;
}

HRESULT Engine::MeshContainer::Render()
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	m_pDeviceContext->DrawIndexed(m_MeshDesc.iNumFaces * 3, m_MeshDesc.iStartFaceIndex * 3, m_MeshDesc.iStartVertexIndex);

	return S_OK;
}

HRESULT Engine::MeshContainer::AddBoneDesc(BONEDESC * pBoneDesc)
{ 
	m_Bones.push_back(pBoneDesc);
	return S_OK;
}

void Engine::MeshContainer::SetUpBoneMatrice(_matrix * pBoneMatrices, _fmatrix PivotMat)
{
	_uint index = 0;

	for (auto& pBone : m_Bones)
	{
		pBoneMatrices[index++] = PivotMat* XMMatrixTranspose(XMLoadFloat4x4(&pBone->OffsetMatrix) * pBone->pNode->GetCombinedMatrix());
	}
}

MeshContainer * Engine::MeshContainer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const MESHDESC& MeshDesc)
{
	MeshContainer* pInstance = new MeshContainer(pDevice, pDeviceContext);
	if (FAILED(pInstance->Ready(MeshDesc)))
	{
		MSGBOX("Failed to Creating MeshContainer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

MeshContainer * MeshContainer::Clone()
{
	return new MeshContainer(*this);
}

void Engine::MeshContainer::Free()
{
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);

	for (auto& pBone : m_Bones)
		Safe_Delete(pBone);
	m_Bones.clear();
}
