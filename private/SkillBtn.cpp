#include "stdafx.h"
#include "SkillBtn.h"
#include "GameInstance.h"
#include "UIButton.h"
#include "UIImage.h"
#include "UIText.h"

Client::SkillBtn::SkillBtn()
{
}

HRESULT Client::SkillBtn::Ready(_int fX, _int fY)
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);
	
	GameObject* pObj;
	UIImage::UIDESC imgDesc;
	imgDesc.TextureSceneID = SCENE_BATTLE;
	imgDesc.TextureName = TEXT("Prototype_Componenet_Texture_SkillIcon");
	imgDesc.fX = fX-300 ;//700;
	imgDesc.fY = fY;
	imgDesc.fZ = 0.3f;
	imgDesc.fSizeX = 65;
	imgDesc.fSizeY = 65;

	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_BATTLE, TEXT("Layer_SelectUI"), TEXT("Prototype_GameObject_Image"), &imgDesc, &pObj)))
		return E_FAIL;
	m_pIcon = static_cast<UIImage*>(pObj);

	imgDesc.TextureName = TEXT("Prototype_Componenet_Texture_SelectBtn");
	imgDesc.fX = fX;//700;
	imgDesc.fY = fY;
	imgDesc.fZ = 0.4f;
	imgDesc.fSizeX = 800;
	imgDesc.fSizeY = 65;

	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_BATTLE, TEXT("Layer_SelectUI"), TEXT("Prototype_GameObject_Button"), &imgDesc, &pObj)))
		return E_FAIL;
	m_pBtn = static_cast<UIButton*>(pObj);

	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_BATTLE, TEXT("Layer_SelectUI"), TEXT("Prototype_GameObject_Text"), &_float2(fX - 50, fY), &pObj)))
		return E_FAIL;
	m_pNameTxt = static_cast<UIText*>(pObj);
	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_BATTLE, TEXT("Layer_SelectUI"), TEXT("Prototype_GameObject_Text"), &_float2(fX + 100, fY), &pObj)))
		return E_FAIL;
	m_pCostTxt = static_cast<UIText*>(pObj);

	Safe_AddRef(m_pBtn);
	Safe_AddRef(m_pIcon);
	Safe_AddRef(m_pNameTxt);
	Safe_AddRef(m_pCostTxt);

	m_pBtn->SetActive(false);
	m_pIcon->SetActive(false);
	m_pNameTxt->SetActive(false);
	m_pCostTxt->SetActive(false);

	RELEASE_INSTANCE(GameInstance);
	return S_OK;
}

void Client::SkillBtn::SetButton(_bool visible,SKILLTYPE eSkillType, string strSkillName, _int iUseStat)
{
	if (visible)
	{
		m_eSkillIcon = (int)eSkillType;
		if (m_eSkillIcon > (int)ICON_BUF)
			m_eSkillIcon = ICON_BUF;

		m_pIcon->SetTextureIndex(m_eSkillIcon);
		m_pNameTxt->SetText(strSkillName);
		m_pCostTxt->SetText(to_string(iUseStat));
		m_pNameTxt->SetColor(Colors::White);
		m_pCostTxt->SetColor(Colors::White);
		m_pBtn->SetBtnState(UIButton::BTNSTATE::BTN_NORMAL);
	}
		m_pIcon->SetActive(visible);
		m_pBtn->SetActive(visible);
		m_pNameTxt->SetActive(visible);
		m_pCostTxt->SetActive(visible);

}

void Client::SkillBtn::UseButton(_bool isUsing)
{
	if (isUsing) 
	{
		m_pBtn->SetBtnState(UIButton::BTNSTATE::BTN_OVER);
		m_pIcon->SetTextureIndex((m_eSkillIcon + ICON_END));
		m_pNameTxt->SetColor(Colors::Black);
		m_pCostTxt->SetColor(Colors::Black);
	}
	else
	{
		m_pBtn->SetBtnState(UIButton::BTNSTATE::BTN_NORMAL);
		m_pIcon->SetTextureIndex(m_eSkillIcon);
		m_pNameTxt->SetColor(Colors::White);
		m_pCostTxt->SetColor(Colors::White);
	}
}

SkillBtn * Client::SkillBtn::Create(_int fX, _int fY)
{
	SkillBtn* pInstance = new SkillBtn();
	if (FAILED(pInstance->Ready(fX, fY)))
	{
		MSGBOX("Failed to Creating SkillBtn");
		return nullptr;
	}
	return pInstance;
}

void Client::SkillBtn::Free()
{
	Safe_Release(m_pBtn);
	Safe_Release(m_pIcon);
	Safe_Release(m_pNameTxt);
	Safe_Release(m_pCostTxt);
}