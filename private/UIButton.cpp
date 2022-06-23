#include "stdafx.h"
#include "..\public\UIButton.h"

Client::UIButton::UIButton(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:BaseUI(pDevice, pDeviceContext)
{
}

Client::UIButton::UIButton(const UIButton & rhs)
	: BaseUI(rhs)
{
	m_strName = "UIButton";
}

HRESULT Client::UIButton::Ready_Prototype()
{
	if (FAILED(__super::Ready_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT Client::UIButton::Ready(void * pArg)
{
	if (FAILED(__super::Ready(pArg)))
		return E_FAIL;

	if (pArg == nullptr)
		return E_FAIL;

	m_ImgDesc = (UIDESC*)pArg;
	if (FAILED(SetUpComponents()))
		return E_FAIL;

	m_eBtnState = BTN_NORMAL;

	m_ProjMatrix = XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f);
//	m_WorldMatrix = XMMatrixIdentity();

	m_WorldMatrix.r[0] = XMVectorSet(m_ImgDesc->fSizeX, 0.f, 0.f, 0.f);
	m_WorldMatrix.r[1] = XMVectorSet(0.0f, m_ImgDesc->fSizeY, 0.f, 0.f);
	m_WorldMatrix.r[2] = XMVectorSet(0.0f, 0.f, 1.f, 0.f);
	m_WorldMatrix.r[3] = XMVectorSet(m_ImgDesc->fX - (g_iWinCX >> 1), -(m_ImgDesc->fY) + (g_iWinCY >> 1), m_ImgDesc->fZ, 1.f);

	return S_OK;
}

_int Client::UIButton::Update(_double TimeDelta)
{
	return _int();
}

_int Client::UIButton::LateUpdate(_double TimeDelta)
{
	return __super::LateUpdate(TimeDelta);
}

HRESULT Client::UIButton::Render()
{
	m_pRectBuf->SetUpValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_WorldMatrix), sizeof(_float) * 16);
	m_pRectBuf->SetUpValueOnShader("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float) * 16);
	m_pRectBuf->SetUpValueOnShader("g_ProjMatrix", &XMMatrixTranspose(m_ProjMatrix), sizeof(XMMATRIX));

	m_pRectBuf->SetUpTextureOnShader("g_DiffuseTexture", m_pTexture, (int)m_eBtnState);

	m_pRectBuf->Render(0);
	return S_OK;
}

HRESULT Client::UIButton::SetUpComponents()
{
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (Component**)&m_pRenderer)))
		return E_FAIL;
	if (FAILED(__super::SetUpComponents(m_ImgDesc->TextureSceneID, m_ImgDesc->TextureName, TEXT("Com_Texture"), (Component**)&m_pTexture)))
		return E_FAIL;
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, TEXT("Prototype_Component_RectBuffer"), TEXT("Com_Rect"), (Component**)&m_pRectBuf)))
		return E_FAIL;

	return S_OK;
}

UIButton * Client::UIButton::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	UIButton* pInstance = new UIButton(pDevice, pDeviceContext);
	if (FAILED(pInstance->Ready_Prototype()))
	{
		MSGBOX("failed to Creating UIButton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

GameObject * Client::UIButton::Clone(void * pArg)
{
	UIButton* pInstance = new UIButton(*this);
	if (FAILED(pInstance->Ready(pArg)))
	{
		MSGBOX("failed to Creating UIButton Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::UIButton::Free()
{
	__super::Free();
}
