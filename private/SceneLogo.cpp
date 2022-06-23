#include "stdafx.h"
#include "..\public\SceneLogo.h"
#include "GameInstance.h"
#include "SceneLoading.h"
#include "UIImage.h"

Client::SceneLogo::SceneLogo(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:Scene(pDevice, pDeviceContext)
{
}

HRESULT Client::SceneLogo::Ready()
{

	if (FAILED(__super::Ready()))
		return E_FAIL;

	if (FAILED(Ready_LayerUI(TEXT("Layer_UI"))))
		return E_FAIL;

	return S_OK;
}

_int Client::SceneLogo::Update(_double TimeDelta)
{
	if (0 > (__super::Update(TimeDelta)))
		return -1;
	
	if (GetKeyState(VK_SPACE) < 0) 
	{
		GameInstance* pGameInstance = GET_INSTANCE(GameInstance);
	//	if (FAILED(pGameInstance->OpenScene(SCENE_LOADING, SCENE_GAMEPLAY,SceneLoading::Create(m_pDevice, m_pDeviceContext,SCENE_GAMEPLAY))))
	//		return -1;

		if (FAILED(pGameInstance->OpenScene(SCENE_LOADING, SCENE_BATTLE, SceneLoading::Create(m_pDevice, m_pDeviceContext, SCENE_BATTLE))))
					return -1;

		RELEASE_INSTANCE(GameInstance);
		return 0;
	}
	return _int(0);
}

HRESULT Client::SceneLogo::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT Client::SceneLogo::Ready_LayerUI(const _tchar * pLayerTag)
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	UIImage::UIDESC imgDesc;
	imgDesc.TextureSceneID = SCENE_LOGO;
	imgDesc.TextureName = TEXT("Prototype_Component_Texture_LOGO");
	imgDesc.fX = 600;
	imgDesc.fY = 360;
	imgDesc.fSizeX = 673;
	imgDesc.fSizeY = 512;

	GameObject* pOut;
	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_LOGO, pLayerTag, TEXT("Prototype_GameObject_Image"),&imgDesc, &pOut)))
		return E_FAIL;

	RELEASE_INSTANCE(GameInstance);
}

SceneLogo * Client::SceneLogo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	SceneLogo* pInstance = new SceneLogo(pDevice, pDeviceContext);

	if (FAILED(pInstance->Ready()))
	{
		MSGBOX("Failed to Creating Scene Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::SceneLogo::Free()
{
	__super::Free();
}
