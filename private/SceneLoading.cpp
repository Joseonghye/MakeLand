#include "stdafx.h"
#include "..\public\SceneLoading.h"
#include "Loader.h"
#include "SceneLogo.h"
#include "SceneGamePlay.h"
#include "SceneBattle.h"

#include "UIImage.h"

#include "GameInstance.h"

Client::SceneLoading::SceneLoading(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:Scene(pDevice, pDeviceContext),
	m_eNextScene(SCENE_END), m_pLoader(nullptr)
{
}

HRESULT Client::SceneLoading::Ready(SCENE eNextScene)
{
	if (FAILED(__super::Ready()))
		return E_FAIL;

	//m_eNextScene = m_iNextSceneID;
	m_eNextScene = eNextScene;

	//if (m_eNextScene == SCENE_LOGO)
	//{
	//	m_hVideo = MCIWndCreate(g_hWnd
	//		, nullptr, WS_CHILD | WS_VISIBLE | MCIWNDF_NOPLAYBAR
	//		, L"../Bin/Resources/Video/Opening.wma");

	//	MoveWindow(m_hVideo, 0, 0, g_iWinCX, g_iWinCY, FALSE);

	//	MCIWndPlay(m_hVideo);
	//}
	//else
	//{
		// 로딩에서 배경을 담당할 복제본 객체 생성 
//		if (FAILED(Ready_LayerBackGround(TEXT("Layer_BackGround"))))
//			return E_FAIL;
	if (m_eNextScene != SCENE_LOGO) 
	{
		if (FAILED(Ready_LayerUI(TEXT("Layer_UI"))))
			return E_FAIL;
	}
//	}
	

	// 다음 씬에 필요한 원형 객체 생성
	m_pLoader = Loader::Create(m_pDevice, m_pDeviceContext, m_eNextScene);
	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

_int Client::SceneLoading::Update(_double TimeDelta)
{
	if (nullptr == m_pLoader)
		return -1;

	if (0 > __super::Update(TimeDelta))
		return -1;

	//로더가 다했는지
	if (true == m_pLoader->GetFinished())
	{
		/*if (m_eNextScene == SCENE_LOGO &&
			MCIWndGetLength(m_hVideo) <= MCIWndGetPosition(m_hVideo))
		{
			if (FAILED(OpenScene()))
				return -1;

			return 0;
		}
		elseif (GetKeyState(VK_UP) < 0)
		{ */
			if (FAILED(OpenScene()))
				return -1;

	//		return 0;
	//	}
	}

	return _int(0);
}

HRESULT Client::SceneLoading::Render()
{
	if (nullptr == m_pLoader)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pLoader->PrintLoaderText();

	return S_OK;
}

HRESULT Client::SceneLoading::Ready_LayerBackGround(const _tchar * pLayerTag)
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);
	
	GameObject* pOut;
	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_LOADING, pLayerTag, TEXT("Prototype_GameObject_BackGround"),nullptr,&pOut)))
		return E_FAIL;

	RELEASE_INSTANCE(GameInstance);
	return S_OK;
}

HRESULT Client::SceneLoading::Ready_LayerUI(const _tchar * pLayerTag)
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	GameObject* pOut;

	UIImage::UIDESC imgDesc;
	imgDesc.TextureSceneID = SCENE_STATIC;
	imgDesc.TextureName = TEXT("Prototype_Component_Texture_LoadingUI");
	imgDesc.fX = 1150;
	imgDesc.fY = 590;
	imgDesc.fSizeX = 256;
	imgDesc.fSizeY = 256;
	imgDesc.bAnimation = true;
	imgDesc.iAnimationCnt = 3;

	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_LOADING, pLayerTag, TEXT("Prototype_GameObject_Image"), &imgDesc, &pOut)))
		return E_FAIL;

	RELEASE_INSTANCE(GameInstance);
	return S_OK;
}

HRESULT Client::SceneLoading::Ready_LightDesc()
{
	LIGHTDESC lightDesc;
	ZeroMemory(&lightDesc, sizeof(LIGHTDESC));

	lightDesc.vDirection = _float3();
	lightDesc.vDiffuse = _float4();
	lightDesc.vAmbient = _float4();
	lightDesc.vSpecular = _float4();

	return S_OK;
}

HRESULT Client::SceneLoading::OpenScene()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	Scene* pScene = nullptr;

	switch (m_eNextScene)
	{
	case SCENE_LOGO:
		pScene = SceneLogo::Create(m_pDevice, m_pDeviceContext);
		break;
	case SCENE_GAMEPLAY:
		pScene = SceneGamePlay::Create(m_pDevice, m_pDeviceContext);
		break;
	case SCENE_BATTLE:
		pScene = SceneBattle::Create(m_pDevice, m_pDeviceContext);
		break;
	}

  	if (FAILED(pGameInstance->OpenScene(m_eNextScene,0,pScene)))
		return E_FAIL;

	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

SceneLoading * Client::SceneLoading::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, SCENE eNextScene)
{
	SceneLoading* pInstance = new SceneLoading(pDevice, pDeviceContext);
	if (FAILED(pInstance->Ready(eNextScene)))
	{
		MSGBOX("Faile to Creating Scene Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::SceneLoading::Free()
{
	__super::Free();

	MCIWndClose(m_hVideo);
	Safe_Release(m_pLoader);
}
