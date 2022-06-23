#include "..\public\HierarchyNode.h"
#include "AnimChannel.h"

Engine::HierarchyNode::HierarchyNode(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:m_pDevice(pDevice), m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT Engine::HierarchyNode::Ready(char* pBoneName, _fmatrix TransformationMatrix, _uint iDepth, HierarchyNode* pParent)
{
	strcpy_s(m_szBoneName, pBoneName);
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(TransformationMatrix));
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());
	m_iDepth = iDepth;

	m_pParent = pParent;
	Safe_AddRef(m_pParent);
	return S_OK;
}

//모든 뼈의 TransformationMatrix가 애니메이션에 맞게 셋팅 후 호출
//현재 노드의 CombinedTransformationMatrix를 셋팅하기 위함.
void Engine::HierarchyNode::UpdateCombinedTransformationMatrix(_uint iAniIndex)
{
	if (nullptr != m_Channels[iAniIndex])
		XMStoreFloat4x4(&m_TransformationMatrix, m_Channels[iAniIndex]->GetTransformMatrix());
	
	if (nullptr != m_pParent)
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&m_pParent->m_CombinedTransformationMatrix));
	else
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * XMMatrixIdentity());
}

HierarchyNode * Engine::HierarchyNode::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, char * pBoneName, _fmatrix TransformationMatrix, _uint iDepth, HierarchyNode * pParent)
{
	HierarchyNode* pInstance = new HierarchyNode(pDevice, pDeviceContext);
	if (FAILED(pInstance->Ready(pBoneName, TransformationMatrix, iDepth, pParent)))
	{
		MSGBOX("Failed to Creating HierarchyNode");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void Engine::HierarchyNode::Free()
{
	Safe_Release(m_pParent);

	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);
	m_Channels.clear();

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
