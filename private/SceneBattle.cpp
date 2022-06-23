#include "stdafx.h"
#include "..\public\SceneBattle.h"
#include "DynamicCamera.h"
#include "GameInstance.h"
#include "Turn.h"
#include "UIImage.h"
#include "BtlMgr.h"
#include "DataManager.h"
#include "PlayMgr.h"


SceneBattle::SceneBattle(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:Scene(pDevice, pDeviceContext), m_pPlayMgr(PlayMgr::GetInstance()), m_pBtlMgr(BtlMgr::GetInstance())
{
}

HRESULT SceneBattle::Ready()
{
	if (FAILED(__super::Ready()))
		return E_FAIL;
	m_pPlayMgr->ReadyToPlay();

	if (FAILED(Ready_LightDesc()))
		return E_FAIL;
	if (FAILED(Ready_LayerPlayer(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_LayerEnemy(TEXT("Layer_Enemy"))))
		return E_FAIL;

	if (FAILED(Ready_LayerBackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_LayerUI(TEXT("Layer_UI"))))
		return E_FAIL;

	if (FAILED(Ready_LayerSelectUI(TEXT("Layer_SelectUI"))))
		return E_FAIL;
	
	if (FAILED(Ready_LayerCamera(TEXT("Layer_Camera"))))
		return E_FAIL;


	m_pBtlMgr->ReadyBattle(m_pDevice, m_pDeviceContext, "Boss");

	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	pGameInstance->PlayBGM(L"Battle");

	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

_int SceneBattle::Update(_double TimeDelta)
{
	if (0 > (__super::Update(TimeDelta)))
		return -1;

	m_pPlayMgr->Update();
	if (0 > m_pBtlMgr->Update(TimeDelta))
		return -1;

	if (0 > m_pBtlMgr->LateUpdate(TimeDelta))
		return -1;

	return _int(0);
}

HRESULT SceneBattle::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT SceneBattle::Ready_LayerPlayer(const _tchar * pLayerTag)
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);
	PlayMgr* pPlayMgr = GET_INSTANCE(PlayMgr);

	vector<PLAYERDATA*> players = pPlayMgr->GetPlayerDatas();
	for (int i = 0; i < players.size(); ++i)
	{
		CHARPLAYERDATA ChData;
		ChData.strName = players[i]->strName.c_str();
		ChData.strWeapon = players[i]->strWeapon.c_str();
		ChData.strWeaponBone = players[i]->strWeaponBone.c_str();
		ChData.strPersonaID = players[i]->strPersonaID.c_str();

		ChData.eAttackType = players[i]->eAttackType;
		ChData.iLevel = players[i]->iLevel;
		ChData.iMaxHp = players[i]->iMaxHp;
		ChData.iMaxMp = players[i]->iMaxMp;
		ChData.iHp = players[i]->iHp;
		ChData.iMp = players[i]->iMp;

		if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_BATTLE, pLayerTag, TEXT("Prototype_GameObject_Turn"), &ChData)))
			return E_FAIL;
	}

	RELEASE_INSTANCE(GameInstance);
	return S_OK;
}

HRESULT Client::SceneBattle::Ready_LayerEnemy(const _tchar * pLayerTag)
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);
	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_BATTLE, pLayerTag, TEXT("Prototype_GameObject_Amenosagiri"))))
		return E_FAIL;
	RELEASE_INSTANCE(GameInstance);
	return S_OK;
}


HRESULT SceneBattle::Ready_LayerBackGround(const _tchar * pLayerTag)
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_BATTLE, pLayerTag, TEXT("Prototype_GameObject_Cube"))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_BATTLE, pLayerTag, TEXT("Prototype_GameObject_BattleBG"))))
		return E_FAIL;

	RELEASE_INSTANCE(GameInstance);
	return S_OK;
}

HRESULT SceneBattle::Ready_LayerUI(const _tchar * pLayerTag)
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	UIImage::UIDESC imgDesc;
	imgDesc.TextureSceneID = SCENE_BATTLE;
	imgDesc.TextureName = TEXT("Prototype_Componenet_Texture_BtnAnalysis");
	imgDesc.fX = 128;
	imgDesc.fY = 505;
	imgDesc.fSizeX = 256;
	imgDesc.fSizeY = 28;
	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_BATTLE, pLayerTag, TEXT("Prototype_GameObject_Button"), &imgDesc)))
		return E_FAIL;

	imgDesc.TextureName = TEXT("Prototype_Componenet_Texture_BtnTactics");
	imgDesc.fY = 530;
	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_BATTLE, pLayerTag, TEXT("Prototype_GameObject_Button"), &imgDesc)))
		return E_FAIL;

	imgDesc.TextureName = TEXT("Prototype_Componenet_Texture_BtnGuard");
	imgDesc.fY = 555;
	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_BATTLE, pLayerTag, TEXT("Prototype_GameObject_Button"), &imgDesc)))
		return E_FAIL;

	imgDesc.TextureName = TEXT("Prototype_Componenet_Texture_BtnAttack");
	imgDesc.fY = 580;
	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_BATTLE, pLayerTag, TEXT("Prototype_GameObject_Button"), &imgDesc)))
		return E_FAIL;

	imgDesc.TextureName = TEXT("Prototype_Componenet_Texture_BtnSkill");
	imgDesc.fY = 605;
	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_BATTLE, pLayerTag, TEXT("Prototype_GameObject_Button"), &imgDesc)))
		return E_FAIL;

	imgDesc.TextureName = TEXT("Prototype_Componenet_Texture_BtnPersona");
	imgDesc.fY = 630;
	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_BATTLE, pLayerTag, TEXT("Prototype_GameObject_Button"), &imgDesc)))
		return E_FAIL;

	imgDesc.TextureName = TEXT("Prototype_Componenet_Texture_BtnItem");
	imgDesc.fY = 655;
	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_BATTLE, pLayerTag, TEXT("Prototype_GameObject_Button"), &imgDesc)))
		return E_FAIL;

	imgDesc.TextureName = TEXT("Prototype_Componenet_Texture_BtnEscape");
	imgDesc.fY = 680;
	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_BATTLE, pLayerTag, TEXT("Prototype_GameObject_Button"), &imgDesc)))
		return E_FAIL;


	imgDesc.TextureName = TEXT("Prototype_Componenet_Texture_BtlPanel");
	imgDesc.fX = 128;
	imgDesc.fY = 592;
	imgDesc.fSizeX = 256;
	imgDesc.fSizeY = 256;
	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_BATTLE, pLayerTag, TEXT("Prototype_GameObject_Image"),&imgDesc)))
		return E_FAIL;

	//imgDesc.TextureName = TEXT("Prototype_Componenet_Texture_Frame");
	//imgDesc.fX = 500;
	//imgDesc.fY = 700;
	//imgDesc.fSizeX = 900;
	//imgDesc.fSizeY = 700;
	//if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_BATTLE, pLayerTag, TEXT("Prototype_GameObject_Image"), &imgDesc)))
	//	return E_FAIL;

	RELEASE_INSTANCE(GameInstance);
	return S_OK;
}

HRESULT Client::SceneBattle::Ready_LayerSelectUI(const _tchar * pLayerTag)
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	UIImage::UIDESC imgDesc;
	imgDesc.TextureSceneID = SCENE_BATTLE;
/*	imgDesc.TextureName = TEXT("Prototype_Componenet_Texture_SelectBtn");
	imgDesc.fX = 700;
	imgDesc.fSizeX = 800;
	imgDesc.fSizeY = 65;
	for (int i = 0; i < 5; ++i) 
	{
		imgDesc.fY = 220 + (66*i);
	
		if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_BATTLE, pLayerTag, TEXT("Prototype_GameObject_Button"), &imgDesc)))
			return E_FAIL;
	}*/

	imgDesc.TextureName = TEXT("Prototype_Componenet_Texture_SelectPanel");
	imgDesc.fX = 700;
	imgDesc.fY = 350;
	imgDesc.fSizeX = 800;
	imgDesc.fSizeY = 420;
	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_BATTLE, pLayerTag, TEXT("Prototype_GameObject_Image"), &imgDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(GameInstance);
	return S_OK;
}

HRESULT SceneBattle::Ready_LayerCamera(const _tchar * pLayerTag)
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	Camera::CAMERADESC CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(Camera::CAMERADESC));

	CameraDesc.vEye = _float3(0.f, 10.f, -7.f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.vAxisY = _float3(0.f, 1.f, 0.f);

	CameraDesc.fFovy = XMConvertToRadians(60.f);
	CameraDesc.fAspect = _float(g_iWinCX) / g_iWinCY; 
	CameraDesc.fNear = 0.2f; 
	CameraDesc.fFar = 300.f;

	CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(120.f);

	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_BATTLE, pLayerTag, TEXT("Prototype_GameObject_BattleCamera"), &CameraDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(GameInstance);
	return S_OK;
}

HRESULT Client::SceneBattle::Ready_LightDesc()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eType = LIGHTDESC::TYPE_POINT;

	LightDesc.vPosition = _float3(5.f, 3.f, 5.f);
	LightDesc.fRange = 7.f;

	LightDesc.vDiffuse = _float4(1.f, 0.0f, 0.0f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);

	if (FAILED(pGameInstance->AddLight(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;


	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eType = LIGHTDESC::TYPE_POINT;

	LightDesc.vPosition = _float3(10.f, 3.f, 5.f);
	LightDesc.fRange = 7.f;

	LightDesc.vDiffuse = _float4(0.0f, 1.f, 0.0f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);

	if (FAILED(pGameInstance->AddLight(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float3(1.f, -1.f, 1.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);

	if (FAILED(pGameInstance->AddLight(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(GameInstance);
	return S_OK;
}

SceneBattle * SceneBattle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	SceneBattle* pInstance = new SceneBattle(pDevice, pDeviceContext);
	if (FAILED(pInstance->Ready()))
	{
		MSGBOX("Failed to Creating Battle Scene ");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void SceneBattle::Free()
{
	__super::Free();
	Safe_Release(m_pBtlMgr);
}
