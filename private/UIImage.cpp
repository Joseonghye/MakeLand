#include "stdafx.h"
#include "..\public\UIImage.h"

Client::UIImage::UIImage(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:BaseUI(pDevice, pDeviceContext)
{
}

Client::UIImage::UIImage(const UIImage & rhs)
	: BaseUI(rhs), m_isSlider(false), m_fPersent(1.0f), m_iTextureIndex(0), m_fTime(0)
{
	m_WorldMatrix = XMMatrixIdentity();
}

void Client::UIImage::Rotation(_float angle)
{
	_matrix RotationMat = XMMatrixRotationAxis(XMVectorSet(0, 0, 1, 0), XMConvertToRadians(angle));

	for(int i=0; i<3; i++)
		m_WorldMatrix.r[0] = XMVector4Transform(m_WorldMatrix.r[0], RotationMat);

}

HRESULT Client::UIImage::Ready_Prototype()
{
	if (FAILED(__super::Ready_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT Client::UIImage::Ready(void * pArg)
{
	if (FAILED(__super::Ready(pArg)))
		return E_FAIL;

	if (pArg == nullptr)
		return E_FAIL;

	m_ImgDesc = *((UIDESC*)pArg);
	if (FAILED(SetUpComponents()))
		return E_FAIL;

	m_ProjMatrix = XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f);

	m_WorldMatrix.r[0] = XMVectorSet(m_ImgDesc.fSizeX, 0.f, 0.f, 0.f);
	m_WorldMatrix.r[1] = XMVectorSet(0.0f, m_ImgDesc.fSizeY, 0.f, 0.f);
	m_WorldMatrix.r[2] = XMVectorSet(0.0f, 0.f, 1.f, 0.f);
	m_WorldMatrix.r[3] = XMVectorSet(m_ImgDesc.fX - (g_iWinCX >> 1), -(m_ImgDesc.fY) + (g_iWinCY >> 1), m_ImgDesc.fZ, 1.f);

	return S_OK;
}

_int Client::UIImage::Update(_double TimeDelta)
{
	m_fTime += TimeDelta;
	return _int();
}

_int Client::UIImage::LateUpdate(_double TimeDelta)
{
	return __super::LateUpdate(TimeDelta);
}

HRESULT Client::UIImage::Render()
{
	m_pRectBuf->SetUpValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_WorldMatrix), sizeof(_float) * 16);
	m_pRectBuf->SetUpValueOnShader("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float) * 16);
	m_pRectBuf->SetUpValueOnShader("g_ProjMatrix", &XMMatrixTranspose(m_ProjMatrix), sizeof(XMMATRIX));

	m_pRectBuf->SetUpValueOnShader("g_fPersent", &m_fPersent, sizeof(float));

	m_pRectBuf->SetUpTextureOnShader("g_DiffuseTexture", m_pTexture, m_iTextureIndex);

	if (m_ImgDesc.bAnimation)
	{
		if (m_fTime >= 1.f)
		{
			m_fTime = 0;
			m_iTextureIndex++;
			if (m_ImgDesc.iAnimationCnt <= m_iTextureIndex)
				m_iTextureIndex = 0;
		}
	}

	if (m_isSlider)
		m_pRectBuf->Render(1);
	else
		m_pRectBuf->Render(0);

	return S_OK;
}

HRESULT Client::UIImage::SetUpComponents()
{
	//tchar* to string
	int len = wcslen(m_ImgDesc.TextureName);
	char* psz = new char[2 * len + 1];
	size_t cn = 0;
	wcstombs_s(&cn,psz, 2 * len + 1, m_ImgDesc.TextureName,_TRUNCATE);
	string str = psz;

	m_strName = psz;

	if (FAILED(__super::SetUpComponents(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (Component**)&m_pRenderer)))
		return E_FAIL;
	if (FAILED(__super::SetUpComponents(m_ImgDesc.TextureSceneID, m_ImgDesc.TextureName, TEXT("Com_Texture"), (Component**)&m_pTexture)))
		return E_FAIL;
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, TEXT("Prototype_Component_RectBuffer"), TEXT("Com_Rect"), (Component**)&m_pRectBuf)))
		return E_FAIL;

	return S_OK;
}

UIImage * Client::UIImage::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	UIImage* pInstance = new UIImage(pDevice, pDeviceContext);
	if (FAILED(pInstance->Ready_Prototype()))
	{
		MSGBOX("failed to Creating UIImage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

GameObject * Client::UIImage::Clone(void * pArg)
{
	UIImage* pInstance = new UIImage(*this);
	if (FAILED(pInstance->Ready(pArg)))
	{
		MSGBOX("failed to Creating UIImage Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::UIImage::Free()
{
	__super::Free();
}
