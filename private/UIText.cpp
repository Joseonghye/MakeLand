#include "stdafx.h"
#include "..\public\UIText.h"
#include "Renderer.h"
#include "GameInstance.h"

Client::UIText::UIText(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:GameObject(pDevice,pDeviceContext)
{
}

Client::UIText::UIText(const UIText & rhs)
	: GameObject(rhs)
{
}

HRESULT Client::UIText::Ready_Prototype()
{
	return S_OK;
}

HRESULT Client::UIText::Ready(void * pArg)
{
	if (pArg != nullptr)
	{
		m_vPosition = *(_float2*)pArg;
	}else
		m_vPosition = _float2();

	XMStoreFloat4(&m_vColor,Colors::Black);
	

	if (FAILED(__super::SetUpComponents(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (Component**)&m_pRenderer)))
		return E_FAIL;

	return S_OK;
}

_int Client::UIText::Update(_double TimeDelta)
{
	return _int();
}

_int Client::UIText::LateUpdate(_double TimeDelta)
{
	if (nullptr != m_pRenderer)
		m_pRenderer->AddRenderGroup(Renderer::RENDER_UI, this);

	return _int();
}

HRESULT Client::UIText::Render()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	if (FAILED(pGameInstance->Render_Font(TEXT("Font_Persona"), m_vPosition, m_pText, XMLoadFloat4(&m_vColor))))
		return E_FAIL;

	RELEASE_INSTANCE(GameInstance);
	return S_OK;
}

void Client::UIText::SetText(string str)
{
	
	//int len = 1 + strlen(szTxt);
	//size_t cn;
	//_tchar* Text = new _tchar[len];
	//mbstowcs_s(&cn, Text, len, szTxt, len);

	const char* szTxt = str.c_str();
	_tchar Text[256] = {};
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szTxt, strlen(szTxt), Text, 256);
	const int nArraySize = _countof(Text);
	m_pText = new _tchar[nArraySize];

	_tcscpy_s(m_pText, nArraySize, Text);
}

void Client::UIText::SetText(_tchar * str)
{
	m_pText = new _tchar[260];
	_tcscpy_s(m_pText, 260, str);
}

UIText * Client::UIText::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	UIText*	pInstance = new UIText(pDevice, pDeviceContext);

	if (FAILED(pInstance->Ready_Prototype()))
	{
		MSGBOX("Failed to Creating UIText");
		Safe_Release(pInstance);
	}

	return pInstance;
}

UIText * Client::UIText::Clone(void * pArg)
{
	UIText*	pInstance = new UIText(*this);

	if (FAILED(pInstance->Ready(pArg)))
	{
		MSGBOX("Failed to Creating UIText Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::UIText::Free()
{
	__super::Free();
//	Safe_Release(m_pRenderer);
}
