#include "stdafx.h"
#include "..\public\RePlayPanel.h"
#include "UIImage.h"
#include "UIButton.h"
#include "GameInstance.h"

RePlayPanel::RePlayPanel()
{
}

HRESULT Client::RePlayPanel::Ready()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	GameObject* pObj;

	UIImage::UIDESC imgDesc;
	imgDesc.TextureSceneID = SCENE_BATTLE;
	imgDesc.TextureName = TEXT("Prototype_Componenet_Texture_Black");
	imgDesc.fX = 640;
	imgDesc.fY = 360;
	imgDesc.fZ = 0.5f;
	imgDesc.fSizeX = 1280;
	imgDesc.fSizeY = 720;

	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_BATTLE, TEXT("Layer_StateUI"), TEXT("Prototype_GameObject_Image"), &imgDesc, &pObj)))
		return E_FAIL;
	m_pPanel = static_cast<UIImage*>(pObj);


	imgDesc.TextureName = TEXT("Prototype_Componenet_Texture_End");
	imgDesc.fX = 640;
	imgDesc.fY = 360;
	imgDesc.fZ = 0.3f;
	imgDesc.fSizeX = 1280;
	imgDesc.fSizeY = 720;
	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_BATTLE, TEXT("Layer_StateUI"), TEXT("Prototype_GameObject_Image"), &imgDesc, &pObj)))
		return E_FAIL;
	m_pDieImg = static_cast<UIImage*>(pObj);

	imgDesc.TextureName = TEXT("Prototype_Componenet_Texture_Yes");
	imgDesc.fX = 540;
	imgDesc.fY = 360;
	imgDesc.fZ = 0.3f;
	imgDesc.fSizeX = 150;
	imgDesc.fSizeY = 55;
	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_BATTLE, TEXT("Layer_StateUI"), TEXT("Prototype_GameObject_Button"), &imgDesc, &pObj)))
		return E_FAIL;
	m_pSelectImg[0] = static_cast<UIButton*>(pObj);

	imgDesc.TextureName = TEXT("Prototype_Componenet_Texture_No");
	imgDesc.fX = 740;
	imgDesc.fY = 360;
	imgDesc.fZ = 0.3f;
	imgDesc.fSizeX = 150;
	imgDesc.fSizeY = 55;
	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_BATTLE, TEXT("Layer_StateUI"), TEXT("Prototype_GameObject_Button"), &imgDesc, &pObj)))
		return E_FAIL;
	m_pSelectImg[1] = static_cast<UIButton*>(pObj);


	m_pPanel->SetActive(false);
	m_pSelectImg[0]->SetActive(false);
	m_pSelectImg[1]->SetActive(false);
	m_pDieImg->SetActive(false);
	RELEASE_INSTANCE(GameInstance);
	return S_OK;
}

void Client::RePlayPanel::RePlayActive()
{
	m_pPanel->SetActive(true);
//	m_pSelectImg[0]->SetActive(true);
//	m_pSelectImg[1]->SetActive(true);
	m_pDieImg->SetActive(true);
}

void Client::RePlayPanel::Replay(_int isRe)
{
	if (isRe == 0)
	{
		m_pSelectImg[0]->SetBtnState(UIButton::BTN_OVER);
		m_pSelectImg[1]->SetActive(UIButton::BTN_NORMAL);
	}
	else
	{
		m_pSelectImg[0]->SetBtnState(UIButton::BTN_NORMAL);
		m_pSelectImg[1]->SetActive(UIButton::BTN_OVER);
	}
}

RePlayPanel * Client::RePlayPanel::Create()
{
	RePlayPanel* pInstance = new RePlayPanel();
	if(FAILED(pInstance->Ready()))
	{
		MSGBOX("Failed to Creating RePlayPanel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void Client::RePlayPanel::Free()
{
}
