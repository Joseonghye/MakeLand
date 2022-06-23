#include "stdafx.h"
#include "..\public\UIInGame.h"
#include "Transform.h"
#include "Texture.h"
#include "Renderer.h"
#include "RectBuffer.h"
#include "GameInstance.h"

Client::UIInGame::UIInGame(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:GameObject(pDevice, pDeviceContext)
{
}

Client::UIInGame::UIInGame(const UIInGame & rhs)
	: GameObject(rhs)
{
}

HRESULT Client::UIInGame::Ready_Prototype()
{
	if (FAILED(__super::Ready_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT Client::UIInGame::Ready(void * pArg)
{
	if(pArg == nullptr)
		return E_FAIL;

	PosData* posData = (PosData*)pArg;
	m_pTransform->SetState(Transform::STATE_POSITION, XMLoadFloat4(&(posData->vPos)));
	m_pTransform->Scaling(XMVectorSet(4, 4, 0, 0));
	return S_OK;
}

_int Client::UIInGame::Update(_double TimeDelta)
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);
	_matrix ViewMatrix;

	//빌보드
	ViewMatrix = pGameInstance->GetTransform(PipeLine::D3DTS_VIEW);

	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);

	m_pTransform->SetState(Transform::STATE::STATE_RIGHT, ViewMatrix.r[0]);
	m_pTransform->SetState(Transform::STATE::STATE_LOOK, ViewMatrix.r[2]);

//	m_pTransformCom->Set_Scale(XMVectorSet(2.f, 2.f, 2.f, 1.f));

	m_fFrame += (_float)(TimeDelta * m_fEffectPlaySpeed); //플레이속도 
	if (m_fFrame >= m_iAllFrameCount)
	{
		m_fFrame = 0;
		m_isDestroy = true;
	}

	RELEASE_INSTANCE(GameInstance);
	return _int();
}

_int Client::UIInGame::LateUpdate(_double TimeDelta)
{
	if (nullptr != m_pRenderer)
	{
		m_pRenderer->AddRenderGroup(Renderer::RENDER::RENDER_ALPHA, this);
	}
	return _int();
}

HRESULT Client::UIInGame::Render()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);
	XMMATRIX XMWorldMatrix = XMMatrixTranspose(m_pTransform->GetWorldMatrix());
	XMMATRIX XMViewMatrix = XMMatrixTranspose(pGameInstance->GetTransform(PipeLine::D3DTS_VIEW));
	XMMATRIX XMProjMatrix = XMMatrixTranspose(pGameInstance->GetTransform(PipeLine::D3DTS_PROJECTION));

	m_pRect->SetUpValueOnShader("g_WorldMatrix", &XMWorldMatrix, sizeof(_float) * 16);
	m_pRect->SetUpValueOnShader("g_ViewMatrix", &XMViewMatrix, sizeof(_float) * 16);
	m_pRect->SetUpValueOnShader("g_ProjMatrix", &XMProjMatrix, sizeof(XMMATRIX));

	/*_uint countX = 3;
	_uint countY = 3;*/
	m_pRect->SetUpValueOnShader("g_iImageCountX", &m_iCountX, sizeof(_uint));
	m_pRect->SetUpValueOnShader("g_iImageCountY", &m_iCountY, sizeof(_uint));

	_uint iFrame = (_uint)m_fFrame;
	m_pRect->SetUpValueOnShader("g_iFrame", &iFrame, sizeof(_uint));

	m_pRect->SetUpTextureOnShader("g_DiffuseTexture", m_pTexture, 0);

	m_pRect->Render(2);
	RELEASE_INSTANCE(GameInstance);
	return S_OK;
}

UIInGame * Client::UIInGame::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	UIInGame*	pInstance = new UIInGame(pDevice, pDeviceContext);

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MSGBOX("Failed to Creating UIInGame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

UIInGame * Client::UIInGame::Clone(void * pArg)
{
	UIInGame*	pInstance = new UIInGame(*this);

	if (FAILED(pInstance->Ready(pArg)))
	{
		MSGBOX("Failed to Creating UIInGame Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::UIInGame::Free()
{
	__super::Free();
}
