#include "stdafx.h"
#include "..\public\BackGround.h"

#include "GameInstance.h"

Client::BackGround::BackGround(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: GameObject(pDevice, pDeviceContext),
	m_pRenderer(nullptr), m_pRect(nullptr), m_pTexture(nullptr)
{
}

Client::BackGround::BackGround(const GameObject & rhs)
	: GameObject(rhs)
{
}

HRESULT Client::BackGround::Ready_Prototype()
{
	if (FAILED(__super::Ready_Prototype()))
		return E_FAIL;

	/* AWS, PlayFab, GameBase, 뒤끝(BackEnd) */

	//XMMATRIX matProj= XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f);
	//XMStoreFloat4x4(&m_ProjMat, matProj);

	//XMMATRIX matWorld = XMMatrixIdentity();
	//XMStoreFloat4x4(&m_WorldMat, matWorld);

	m_ProjMatrix = XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f);
	m_WorldMatrix = XMMatrixIdentity();

	return S_OK;
}

HRESULT Client::BackGround::Ready(void * pArg)
{
	if (FAILED(__super::Ready(pArg)))
		return E_FAIL;

	if (FAILED(SetUpComponents()))
		return E_FAIL;

	//XMMATRIX matWorld;
	//matWorld.r[0] = XMVectorSet((_float)g_iWinCX, 0.f, 0.f, 0.f);
	//matWorld.r[1] = XMVectorSet(0.0f, (_float)g_iWinCY, 0.f, 0.f);
	//matWorld.r[2] = XMVectorSet(0.0f, 0.f, 1.f, 0.f);

	//XMStoreFloat4x4(&m_WorldMat, matWorld);

	m_WorldMatrix.r[0] = XMVectorSet((_float)g_iWinCX, 0.f, 0.f, 0.f);
	m_WorldMatrix.r[1] = XMVectorSet(0.0f, (_float)g_iWinCY, 0.f, 0.f);
	m_WorldMatrix.r[2] = XMVectorSet(0.0f, 0.f, 1.f, 0.f);

	return S_OK;
}

_int Client::BackGround::Update(_double TimeDelta)
{
	return _int();
}

_int Client::BackGround::LateUpdate(_double TimeDelta)
{
	if (nullptr != m_pRenderer)
		m_pRenderer->AddRenderGroup(Renderer::RENDER_PRIORITY, this);

	return _int();
}

HRESULT Client::BackGround::Render()
{
	/*XMMATRIX matWorld = XMLoadFloat4x4(&m_WorldMat);
	XMMATRIX matProj= XMLoadFloat4x4(&m_ProjMat);

	m_pRect->SetUpValueOnShader("g_WorldMatrix", &XMMatrixTranspose(matWorld), sizeof(XMMATRIX));
	m_pRect->SetUpValueOnShader("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float) * 16);
	m_pRect->SetUpValueOnShader("g_ProjMatrix", &XMMatrixTranspose(matProj), sizeof(XMMATRIX));
	*///쉐이더로 넘어가서 전치 되기 때문에 넘기기전에 전치해야함

	m_pRect->SetUpValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_WorldMatrix), sizeof(_float) * 16);
	m_pRect->SetUpValueOnShader("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float) * 16);
	m_pRect->SetUpValueOnShader("g_ProjMatrix", &XMMatrixTranspose(m_ProjMatrix), sizeof(XMMATRIX));


	m_pRect->SetUpTextureOnShader("g_DiffuseTexture", m_pTexture, 0);

	m_pRect->Render(0);

	return S_OK;
}

HRESULT Client::BackGround::SetUpComponents()
{
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (Component**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(__super::SetUpComponents(SCENE_STATIC, TEXT("Prototype_Component_RectBuffer"), TEXT("Com_RectBuffer"), (Component**)&m_pRect)))
		return E_FAIL;

	if (FAILED(__super::SetUpComponents(SCENE_STATIC, TEXT("Prototype_Component_Texture_Loading"), TEXT("Com_Texture"), (Component**)&m_pTexture)))
		return E_FAIL;

	return S_OK;
}

BackGround * Client::BackGround::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	BackGround*	pInstance = new BackGround(pDevice, pDeviceContext);

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MSGBOX("Failed to Creating BackGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

GameObject * Client::BackGround::Clone(void * pArg)
{
	BackGround*	pInstance = new BackGround(*this);

	if (FAILED(pInstance->Ready(pArg)))
	{
		MSGBOX("Failed to Creating BackGround Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::BackGround::Free()
{
	__super::Free();

	Safe_Release(m_pTexture);
	Safe_Release(m_pRect);
	Safe_Release(m_pRenderer);
}
