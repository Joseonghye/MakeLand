#include "stdafx.h"
#include "..\public\StatusUI.h"
#include "UIImage.h"
#include "GameInstance.h"

Client::StatusUI::StatusUI(CHARACTER chName)
	:m_eChName(chName)
{
}

HRESULT Client::StatusUI::Ready(string strChName, _float fX, _float fY)
{
	//»ý¼º
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	GameObject* pObj;

	UIImage::UIDESC imgDesc;
	imgDesc.TextureSceneID = SCENE_GAMEPLAY;

	imgDesc.TextureName = TEXT("Prototype_Component_Texture_HpBar");
	imgDesc.fX = fX+5;
	imgDesc.fY = fY+25;
	imgDesc.fSizeX = 90;
	imgDesc.fSizeY = 8;
	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Image"), &imgDesc, &pObj)))
		return E_FAIL;
	m_pHpBar = static_cast<UIImage*>(pObj);

	imgDesc.TextureName = TEXT("Prototype_Component_Texture_MpBar");
	imgDesc.fY = fY+35;
	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Image"), &imgDesc, &pObj)))
		return E_FAIL;
	m_pMpBar = static_cast<UIImage*>(pObj);


	//string to tchar
	string str = "Prototype_Component_Texture_Status_" + strChName;
	const char* szTexture = str.c_str();
	int len = 1 + strlen(szTexture);
	size_t cn;
	_tchar* TextureName = new _tchar[len];
	mbstowcs_s(&cn, TextureName, len, szTexture, len);

	imgDesc.TextureName = TextureName;
	imgDesc.fX = fX;
	imgDesc.fY = fY;
	imgDesc.fSizeX = 128;
	imgDesc.fSizeY = 128;
	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Image"), &imgDesc,&pObj)))
		return E_FAIL;
	m_pFrameImg = static_cast<UIImage*>(pObj);

	RELEASE_INSTANCE(GameInstance);

	m_pHpBar->Rotation(9.f);
	m_pMpBar->Rotation(9.f);
	m_pHpBar->SetSlider(true);
	m_pMpBar->SetSlider(true);

	return S_OK;
}

StatusUI * Client::StatusUI::Create(CHARACTER chName, string strChName, _float fX, _float fY)
{
	StatusUI* pInstance = new StatusUI(chName);
	if (FAILED(pInstance->Ready(strChName, fX, fY)))
	{
		MSGBOX("Failed to Creating Status UI");
		return nullptr;
	}
	return pInstance;
}

void Client::StatusUI::SetStatus(_float fHpPer, _float fMpPer)
{
	m_pHpBar->SetPersent(fHpPer);
	m_pMpBar->SetPersent(fMpPer);
}

void Client::StatusUI::Free()
{
}