#include "stdafx.h"
#include "..\public\DungeonEntry.h"
#include "GameInstance.h"

Client::DungeonEntry::DungeonEntry(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:GameObject(pDevice, pDeviceContext),
	m_pRenderer(nullptr), m_pModel(nullptr)
{
}

Client::DungeonEntry::DungeonEntry(const GameObject & rhs)
	: GameObject(rhs)
{
}

HRESULT Client::DungeonEntry::Ready_Prototype()
{
	if (FAILED(__super::Ready_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT Client::DungeonEntry::Ready(void * pArg)
{
	if (FAILED(__super::Ready(pArg)))
		return E_FAIL;

	if (FAILED(SetUpComponents()))
		return E_FAIL;

	return S_OK;
}

_int Client::DungeonEntry::Update(_double TimeDelta)
{
	return _int();
}

_int Client::DungeonEntry::LateUpdate(_double TimeDelta)
{
	if (nullptr != m_pRenderer)
		m_pRenderer->AddRenderGroup(Renderer::RENDER_NONALPHA, this);
	return _int();
}

HRESULT Client::DungeonEntry::Render()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	m_pModel->SetUpValueOnShader("g_WorldMatrix",&XMMatrixIdentity(), sizeof(_matrix));
	m_pModel->SetUpValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pGameInstance->GetTransform(PipeLine::D3DTS_VIEW)), sizeof(_matrix));
	m_pModel->SetUpValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pGameInstance->GetTransform(PipeLine::D3DTS_PROJECTION)), sizeof(_matrix));

	if (FAILED(m_pModel->BindBuffers()))
		return E_FAIL;

	for (_uint i = 0; i < m_pModel->GetNumMeshContainer(); ++i)
	{
		m_pModel->SetUpTextureOnShader("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		m_pModel->Render(i, 0);
	}

	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

HRESULT Client::DungeonEntry::SetUpComponents()
{
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (Component**)&m_pRenderer)))
		return E_FAIL;
	if (FAILED(__super::SetUpComponents(SCENE_GAMEPLAY, TEXT("Prototype_Component_Model_Entry"), TEXT("Com_Model"), (Component**)&m_pModel)))
		return E_FAIL;
	return S_OK;
}

DungeonEntry * Client::DungeonEntry::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	DungeonEntry*	pInstance = new DungeonEntry(pDevice, pDeviceContext);

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MSGBOX("Failed to Creating DungeonEntry");
		Safe_Release(pInstance);
	}

	return pInstance;
}

GameObject * Client::DungeonEntry::Clone(void * pArg)
{
	DungeonEntry*	pInstance = new DungeonEntry(*this);

	if (FAILED(pInstance->Ready(pArg)))
	{
		MSGBOX("Failed to Creating DungeonEntry Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::DungeonEntry::Free()
{
	__super::Free();

	Safe_Release(m_pModel);
	Safe_Release(m_pRenderer);
}
