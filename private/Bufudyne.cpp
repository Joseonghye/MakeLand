#include "stdafx.h"
#include "..\public\Bufudyne.h"
#include "GameInstance.h"
#include "PointInstanceBuffer.h"
#include "Transform.h"
#include "Texture.h"
#include "Renderer.h"
#include "UIInGame.h"

Client::Bufudyne::Bufudyne(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:GameObject(pDevice,pDeviceContext)
{
}

Client::Bufudyne::Bufudyne(const Bufudyne & rhs)
	: GameObject(rhs)
{
}

HRESULT Client::Bufudyne::Ready_Prototype()
{
	if (FAILED(__super::Ready_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT Client::Bufudyne::Ready(void * pArg)
{
	if (FAILED(__super::Ready(pArg)))
		return E_FAIL;

	if (FAILED(SetUpComponents()))
		return E_FAIL;

	if (pArg == nullptr)
		return E_FAIL;

	UIInGame::PosData* posData = (UIInGame::PosData*)pArg;
	m_pTransform->SetState(Transform::STATE_POSITION, XMLoadFloat4(&(posData->vPos)));

	return S_OK;
}

_int Client::Bufudyne::Update(_double TimeDelta)
{
	m_Time += TimeDelta;
	if (m_Time >= 1.f)
	{
		m_Time = 0;
		m_isDestroy = true;
	}

	GameInstance* pGameInstance = GameInstance::GetInstance();  // GET_INSTANCE(GameInstance);
	_matrix ViewMatrix;

	//ºôº¸µå
	ViewMatrix = pGameInstance->GetTransform(PipeLine::D3DTS_VIEW);

	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);

	m_pTransform->SetState(Transform::STATE::STATE_RIGHT, ViewMatrix.r[0]);
	m_pTransform->SetState(Transform::STATE::STATE_LOOK, ViewMatrix.r[2]);

	//RELEASE_INSTANCE(GameInstance);

	m_pBuffer->Update(TimeDelta);
	return _int();
}

_int Client::Bufudyne::LateUpdate(_double TimeDelta)
{
	if (nullptr != m_pRenderer)
		m_pRenderer->AddRenderGroup(Renderer::RENDER_ALPHA, this);

	return _int();
}

HRESULT Client::Bufudyne::Render()
{
	GameInstance*		pGameInstance = GameInstance::GetInstance(); // GET_INSTANCE(GameInstance);

	m_pBuffer->SetUpValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransform->GetWorldMatrix()), sizeof(_float) * 16);
	m_pBuffer->SetUpValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pGameInstance->GetTransform(PipeLine::D3DTS_VIEW)), sizeof(_matrix));
	m_pBuffer->SetUpValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pGameInstance->GetTransform(PipeLine::D3DTS_PROJECTION)), sizeof(_matrix));
	m_pBuffer->SetUpTextureOnShader("g_DiffuseTexture", m_pTexture, 0);

	m_pBuffer->Render(0);

	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

HRESULT Client::Bufudyne::SetUpComponents()
{
	Transform::TRANSFORMDESC transDesc;
	transDesc.fSpeedPerSec = 7.f;
	transDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUpComponents(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (Component**)&m_pTransform, &transDesc)))
		return E_FAIL;
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (Component**)&m_pRenderer)))
		return E_FAIL;
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, TEXT("Prototype_Component_PointInstanceBuffer"), TEXT("Com_PonitInstance"), (Component**)&m_pBuffer)))
		return E_FAIL;
	if (FAILED(__super::SetUpComponents(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_Bufudyne"), TEXT("Com_Texture"), (Component**)&m_pTexture)))
		return E_FAIL;

	return S_OK;
}

void Client::Bufudyne::Free()
{
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);
	Safe_Release(m_pBuffer);
	Safe_Release(m_pRenderer);
}

Bufudyne * Client::Bufudyne::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{

	Bufudyne*	pInstance = new Bufudyne(pDevice, pDeviceContext);

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MSGBOX("Failed to Creating Bufudyne");
		Safe_Release(pInstance);
	}

	return pInstance;
}

Bufudyne * Client::Bufudyne::Clone(void * pArg)
{
	Bufudyne*	pInstance = new Bufudyne(*this);

	if (FAILED(pInstance->Ready(pArg)))
	{
		MSGBOX("Failed to Creating Bufudyne Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}
