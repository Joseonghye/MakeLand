#include "stdafx.h"
#include "..\public\Galdyne.h"
#include "GameInstance.h"
#include "PointInstanceBuffer.h"
#include "Transform.h"
#include "Texture.h"
#include "Renderer.h"
#include "UIInGame.h"

Client::Galdyne::Galdyne(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:GameObject(pDevice,pDeviceContext)
{
}

Client::Galdyne::Galdyne(const Galdyne & rhs)
	: GameObject(rhs)
{
}

HRESULT Client::Galdyne::Ready_Prototype()
{
	if (FAILED(__super::Ready_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT Client::Galdyne::Ready(void * pArg)
{
	if (FAILED(__super::Ready(pArg)))
		return E_FAIL;

	if (FAILED(SetUpComponents()))
		return E_FAIL;

	if (pArg == nullptr)
		return E_FAIL;

	UIInGame::PosData* posData = (UIInGame::PosData*)pArg;
	m_pTransform->SetState(Transform::STATE_POSITION, XMLoadFloat4(&(posData->vPos)));
	m_pTransform->Scaling(XMVectorSet(0.5f,0.5f, 0, 0));

	m_iAllFrameCount = 8;
	m_iCountX = 4;
	m_iCountY = 2;
	m_fEffectPlaySpeed = 10.f;

	return S_OK;
}

_int Client::Galdyne::Update(_double TimeDelta)
{
	m_fFrame += (_float)(TimeDelta * m_fEffectPlaySpeed); //플레이속도 
	if (m_fFrame >= m_iAllFrameCount)
	{
		m_fFrame = 0;
	}

	m_Time += TimeDelta;
	if (m_Time >= 1.f)
	{
		m_Time = 0;
		m_isDestroy = true;
	}


	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);
	_matrix ViewMatrix;

	//빌보드
	ViewMatrix = pGameInstance->GetTransform(PipeLine::D3DTS_VIEW);

	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);

	m_pTransform->SetState(Transform::STATE::STATE_RIGHT, ViewMatrix.r[0]);
	m_pTransform->SetState(Transform::STATE::STATE_LOOK, ViewMatrix.r[2]);

	RELEASE_INSTANCE(GameInstance);

	m_pBuffer->Update(TimeDelta);
	return _int();
}

_int Client::Galdyne::LateUpdate(_double TimeDelta)
{
	if (nullptr != m_pRenderer)
		m_pRenderer->AddRenderGroup(Renderer::RENDER_ALPHA, this);

	return _int();
}

HRESULT Client::Galdyne::Render()
{
	GameInstance*		pGameInstance = GET_INSTANCE(GameInstance);

	m_pBuffer->SetUpValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransform->GetWorldMatrix()), sizeof(_float) * 16);
	m_pBuffer->SetUpValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pGameInstance->GetTransform(PipeLine::D3DTS_VIEW)), sizeof(_matrix));
	m_pBuffer->SetUpValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pGameInstance->GetTransform(PipeLine::D3DTS_PROJECTION)), sizeof(_matrix));
	
	m_pBuffer->SetUpValueOnShader("g_iImageCountX", &m_iCountX, sizeof(_uint));
	m_pBuffer->SetUpValueOnShader("g_iImageCountY", &m_iCountY, sizeof(_uint));

	_uint iFrame = (_uint)m_fFrame;
	m_pBuffer->SetUpValueOnShader("g_iFrame", &iFrame, sizeof(_uint));

	
	m_pBuffer->SetUpTextureOnShader("g_DiffuseTexture", m_pTexture, 0);

	m_pBuffer->Render(1);

	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

HRESULT Client::Galdyne::SetUpComponents()
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
	if (FAILED(__super::SetUpComponents(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_Galdyne"), TEXT("Com_Texture"), (Component**)&m_pTexture)))
		return E_FAIL;

	return S_OK;
}

void Client::Galdyne::Free()
{
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);
	Safe_Release(m_pBuffer);
	Safe_Release(m_pRenderer);
}

Galdyne * Client::Galdyne::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{

	Galdyne*	pInstance = new Galdyne(pDevice, pDeviceContext);

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MSGBOX("Failed to Creating Galdyne");
		Safe_Release(pInstance);
	}

	return pInstance;
}

Galdyne * Client::Galdyne::Clone(void * pArg)
{
	Galdyne*	pInstance = new Galdyne(*this);

	if (FAILED(pInstance->Ready(pArg)))
	{
		MSGBOX("Failed to Creating Galdyne Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}
