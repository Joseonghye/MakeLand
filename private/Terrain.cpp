#include "stdafx.h"
#include "..\public\Terrain.h"

#include "GameInstance.h"

Client::Terrain::Terrain(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: GameObject(pDevice, pDeviceContext),
	m_pRenderer(nullptr), m_pTexture(nullptr), m_pTransform(nullptr),
	m_pTerrain(nullptr)
{
}

Client::Terrain::Terrain(const GameObject & rhs)
	: GameObject(rhs)
{
}

HRESULT Client::Terrain::Ready_Prototype()
{
	if (FAILED(__super::Ready_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT Client::Terrain::Ready(void * pArg)
{
	if (FAILED(__super::Ready(pArg)))
		return E_FAIL;

	if (FAILED(SetUpComponents()))
		return E_FAIL;


	return S_OK;
}

_int Client::Terrain::Update(_double TimeDelta)
{

	return _int();
}

_int Client::Terrain::LateUpdate(_double TimeDelta)
{
	if (nullptr != m_pRenderer)
		m_pRenderer->AddRenderGroup(Renderer::RENDER_NONALPHA, this);

	return _int();
}

HRESULT Client::Terrain::Render()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	m_pTerrain->SetUpValueOnShader("g_WorldMatrix",&XMMatrixTranspose(m_pTransform->GetWorldMatrix()),sizeof(_matrix));
	m_pTerrain->SetUpValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pGameInstance->GetTransform(PipeLine::D3DTS_VIEW)), sizeof(_matrix));
	m_pTerrain->SetUpValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pGameInstance->GetTransform(PipeLine::D3DTS_PROJECTION)), sizeof(_matrix));

	m_pTerrain->SetUpTextureOnShader("g_DiffuseSourceTexture", m_pTexture, 0);
	m_pTerrain->SetUpTextureOnShader("g_DiffuseDestTexture", m_pTexture, 1);
	m_pTerrain->SetUpTextureOnShader("g_FilterTexture", m_pFilterTex[0], 0);
	m_pTerrain->SetUpTextureOnShader("g_BrushTexture", m_pFilterTex[1], 0);

	m_pTerrain->SetUpValueOnShader("g_vCamPosition", (void*)&pGameInstance->GetCamPos(), sizeof(_vector));
	m_pTerrain->Render(0);
	RELEASE_INSTANCE(GameInstance);
	return S_OK;
}

HRESULT Client::Terrain::SetUpComponents()
{
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (Component**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(__super::SetUpComponents(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (Component**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::SetUpComponents(SCENE_GAMEPLAY, TEXT("Prototype_Componenet_TerrainBuffer"), TEXT("Com_Terrain"), (Component**)&m_pTerrain)))
		return E_FAIL;

	if (FAILED(__super::SetUpComponents(SCENE_GAMEPLAY, TEXT("Prototype_Componenet_Texture_Terrain"), TEXT("Com_Texture"), (Component**)&m_pTexture)))
		return E_FAIL;
	if (FAILED(__super::SetUpComponents(SCENE_GAMEPLAY, TEXT("Prototype_Componenet_Texture_Filter"), TEXT("Com_Filter"), (Component**)&m_pFilterTex[0])))
		return E_FAIL;
	if (FAILED(__super::SetUpComponents(SCENE_GAMEPLAY, TEXT("Prototype_Componenet_Texture_Brush"), TEXT("Com_Brush"), (Component**)&m_pFilterTex[1])))
		return E_FAIL;

	return S_OK;
}

Terrain * Client::Terrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	Terrain*	pInstance = new Terrain(pDevice, pDeviceContext);

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MSGBOX("Failed to Creating Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

GameObject * Client::Terrain::Clone(void * pArg)
{
	Terrain*	pInstance = new Terrain(*this);

	if (FAILED(pInstance->Ready(pArg)))
	{
		MSGBOX("Failed to Creating Terrain Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Terrain::Free()
{
	__super::Free();
	
	for(auto& pTex:m_pFilterTex)
		Safe_Release(pTex);
	
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);
	Safe_Release(m_pTerrain);
	Safe_Release(m_pRenderer);
}
