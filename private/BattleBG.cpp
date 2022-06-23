#include "stdafx.h"
#include "..\public\BattleBG.h"
#include "GameInstance.h"

Client::BattleBG::BattleBG(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:GameObject(pDevice, pDeviceContext),
	m_pRenderer(nullptr), m_pModel(nullptr)
{
}

Client::BattleBG::BattleBG(const GameObject & rhs)
	: GameObject(rhs)
{
}

HRESULT Client::BattleBG::Ready_Prototype()
{
	if (FAILED(__super::Ready_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT Client::BattleBG::Ready(void * pArg)
{
	if (FAILED(__super::Ready(pArg)))
		return E_FAIL;

	if (FAILED(SetUpComponents()))
		return E_FAIL;

	return S_OK;
}

_int Client::BattleBG::Update(_double TimeDelta)
{
	return _int();
}

_int Client::BattleBG::LateUpdate(_double TimeDelta)
{
	if (nullptr != m_pRenderer)
		m_pRenderer->AddRenderGroup(Renderer::RENDER_NONALPHA, this);
	return _int();
}

HRESULT Client::BattleBG::Render()
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

HRESULT Client::BattleBG::SetUpComponents()
{
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (Component**)&m_pRenderer)))
		return E_FAIL;
	if (FAILED(__super::SetUpComponents(SCENE_BATTLE, TEXT("Prototype_Component_Model_TurnBack"), TEXT("Com_Model"), (Component**)&m_pModel)))
		return E_FAIL;
	return S_OK;
}

BattleBG * Client::BattleBG::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	BattleBG*	pInstance = new BattleBG(pDevice, pDeviceContext);

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MSGBOX("Failed to Creating BattleBG");
		Safe_Release(pInstance);
	}

	return pInstance;
}

GameObject * Client::BattleBG::Clone(void * pArg)
{
	BattleBG*	pInstance = new BattleBG(*this);

	if (FAILED(pInstance->Ready(pArg)))
	{
		MSGBOX("Failed to Creating BattleBG Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::BattleBG::Free()
{
	__super::Free();

	Safe_Release(m_pModel);
	Safe_Release(m_pRenderer);
}
