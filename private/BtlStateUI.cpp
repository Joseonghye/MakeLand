#include "stdafx.h"
#include "..\public\BtlStateUI.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "UIImage.h"
#include "UIText.h"

Client::BtlStateUI::BtlStateUI()
{
}

HRESULT Client::BtlStateUI::Ready(_int fX, _int fY)
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	GameObject* pObj;
	UIImage::UIDESC imgDesc;
	imgDesc.TextureSceneID = SCENE_BATTLE;
	imgDesc.TextureName = TEXT("Prototype_Componenet_Texture_BtlSate");
	imgDesc.fX = fX;
	imgDesc.fY = fY;
	imgDesc.fZ = 0.3f;
	imgDesc.fSizeX = 500;
	imgDesc.fSizeY = 50;

	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_BATTLE, TEXT("Layer_StateUI"), TEXT("Prototype_GameObject_Image"), &imgDesc, &pObj)))
		return E_FAIL;
	m_pFrameImg = static_cast<UIImage*>(pObj);

	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_BATTLE, TEXT("Layer_StateUI"), TEXT("Prototype_GameObject_Text"), &_float2(fX-20, fY-10), &pObj)))
		return E_FAIL;
	m_pText = static_cast<UIText*>(pObj);
	m_pText->SetColor(Colors::White);

	Safe_AddRef(m_pFrameImg);
	Safe_AddRef(m_pText);

	m_pFrameImg->SetActive(false);
	m_pText->SetActive(false);
	RELEASE_INSTANCE(GameInstance);
	return S_OK;
}

void Client::BtlStateUI::SetVisible(_bool visible)
{
	m_pFrameImg->SetActive(visible);
	m_pText->SetActive(visible);
}

void Client::BtlStateUI::SetText(string str)
{
	m_pText->SetText(str);
}

BtlStateUI * Client::BtlStateUI::Create(_int fX, _int fY)
{
	BtlStateUI* pInstance = new BtlStateUI();
	if (FAILED(pInstance->Ready(fX, fY)))
	{
		MSGBOX("Failed to Creating BtlStateUI");
		return nullptr;
	}
	return pInstance;
}

void Client::BtlStateUI::Free()
{
	Safe_Release(m_pText);
	Safe_Release(m_pFrameImg);
}