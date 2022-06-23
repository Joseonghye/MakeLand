#include "stdafx.h"
#include "..\public\Cube.h"

#include "GameInstance.h"

Client::Cube::Cube(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: GameObject(pDevice, pDeviceContext),
	m_pRenderer(nullptr), m_pCube(nullptr), m_pTexture(nullptr), m_pTransform(nullptr)
{
}

Client::Cube::Cube(const GameObject & rhs)
	: GameObject(rhs)
{
}

HRESULT Client::Cube::Ready_Prototype()
{
	if (FAILED(__super::Ready_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT Client::Cube::Ready(void * pArg)
{
	if (FAILED(__super::Ready(pArg)))
		return E_FAIL;

	if (FAILED(SetUpComponents()))
		return E_FAIL;

	return S_OK;
}

_int Client::Cube::Update(_double TimeDelta)
{
	return _int();
}

_int Client::Cube::LateUpdate(_double TimeDelta)
{
	if (nullptr != m_pRenderer)
		m_pRenderer->AddRenderGroup(Renderer::RENDER_PRIORITY, this);

	GameInstance*	pGameInstance = GET_INSTANCE(GameInstance);

	m_pTransform->SetState(Transform::STATE_POSITION, pGameInstance->GetCamPos());

	RELEASE_INSTANCE(GameInstance);

	return _int();
}

HRESULT Client::Cube::Render()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	m_pCube->SetUpValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransform->GetWorldMatrix()), sizeof(_matrix));
	m_pCube->SetUpValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pGameInstance->GetTransform(PipeLine::D3DTS_VIEW)), sizeof(_matrix));
	m_pCube->SetUpValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pGameInstance->GetTransform(PipeLine::D3DTS_PROJECTION)), sizeof(_matrix));

	m_pCube->SetUpTextureOnShader("g_DiffuseTexture", m_pTexture, 0);

	m_pCube->Render(0);

	RELEASE_INSTANCE(GameInstance);
	return S_OK;
}

HRESULT Client::Cube::SetUpComponents()
{
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (Component**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(__super::SetUpComponents(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (Component**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::SetUpComponents(SCENE_GAMEPLAY, TEXT("Prototype_Component_CubeBuffer"), TEXT("Com_CubeBuffer"), (Component**)&m_pCube)))
		return E_FAIL;

	if (FAILED(__super::SetUpComponents(SCENE_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"), TEXT("Com_Sky"), (Component**)&m_pTexture)))
		return E_FAIL;

	return S_OK;
}

Cube * Client::Cube::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	Cube*	pInstance = new Cube(pDevice, pDeviceContext);

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MSGBOX("Failed to Creating Cube");
		Safe_Release(pInstance);
	}

	return pInstance;
}

GameObject * Client::Cube::Clone(void * pArg)
{
	Cube*	pInstance = new Cube(*this);

	if (FAILED(pInstance->Ready(pArg)))
	{
		MSGBOX("Failed to Creating Cube Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Cube::Free()
{
	__super::Free();

	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);
	Safe_Release(m_pCube);
	Safe_Release(m_pRenderer);
}
