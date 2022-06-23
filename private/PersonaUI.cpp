#include "stdafx.h"
#include "..\public\PersonaUI.h"
#include "GameInstance.h"
#include "UIImage.h"

Client::PersonaUI::PersonaUI()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	GameObject* pOut = nullptr;
	//페르소나 스테이터스 배경 이미지 
	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Entry"), nullptr, &pOut)))
		return;
	m_pStatusImg = static_cast<UIImage*>(pOut);
	Safe_AddRef(m_pStatusImg);
	m_pStatusImg->SetActive(false);

	//페르소나 주인 이름

	//페르소나 이름

	//페르소나 레벨

	//페르소나 속성 

	//페르소나 스탯 숫자 

	RELEASE_INSTANCE(GameInstance);
}

void Client::PersonaUI::SetVisible(_bool visible)
{
	m_pStatusImg->SetActive(visible);
}

void PersonaUI::Free()
{
	Safe_Release(m_pStatusImg);
}
