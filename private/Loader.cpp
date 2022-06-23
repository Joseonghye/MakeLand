#include "stdafx.h"
#include "..\public\Loader.h"

#include "Terrain.h"
#include "Cube.h"
#include "DynamicCamera.h"
#include "BattleCamera.h"
#include "TargetCamera.h"
#include "Player.h"
#include "Monster.h"
#include "DungeonEntry.h"
#include "BattleBG.h"
#include "Turn.h"
#include "GameInstance.h"
#include "DataManager.h"
#include "Persona.h"

#include "ZiodyneEffect.h"
#include "AgidyneEffect.h"
#include "Bufudyne.h"
#include "Galdyne.h"

#include "Amenosagiri.h"

Client::Loader::Loader(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:m_pDevice(pDevice), m_pDeviceContext(pDeviceContext),
	 m_isFinished(false), m_eNextScene(SCENE_END)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

_uint APIENTRY Thread_Main(void* pArg)
{

	Loader*		pLoader = (Loader*)pArg;

	EnterCriticalSection(&pLoader->GetCriticalSection());

	HRESULT			hr = 0;

	switch (pLoader->GetNextSceneID())
	{
	case SCENE_LOGO:
		hr = pLoader->Loading_ForLogo();
		break;
	case SCENE_GAMEPLAY:
		hr = pLoader->Loading_ForGamePlay();
		break;
	case SCENE_BATTLE:
		hr = pLoader->Loading_ForGamePlay();
		break;
	}

	if (FAILED(hr))
		return -1;

	LeaveCriticalSection(&pLoader->GetCriticalSection());

	return 0;
}


HRESULT Client::Loader::Ready(SCENE eNextScene)
{
	m_eNextScene = eNextScene;

	InitializeCriticalSection(&m_CS);

	/* 스레드 생성 -> 진입점함수를 정의 */
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_Main, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT Client::Loader::Loading_ForLogo()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	// 컴포넌트 원형을 생성
	wsprintf(m_szLoading, TEXT("버퍼를 생성한다. "));
	wsprintf(m_szLoading, TEXT("텍스쳐를 생성한다. "));

	if (FAILED(pGameInstance->AddPrototype(SCENE_LOGO, TEXT("Prototype_Component_Texture_LOGO"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Logo/Logo.png")))))
		return E_FAIL;

	wsprintf(m_szLoading, TEXT("객체를 생성한다. "));
	
	DataManager* DataMgr = GET_INSTANCE(DataManager);
	DataMgr->Ready();

	RELEASE_INSTANCE(DataManager);
	wsprintf(m_szLoading, TEXT("로딩이 완료되었습니다. "));

	RELEASE_INSTANCE(GameInstance);
	m_isFinished = true;

	return S_OK;
}

HRESULT Client::Loader::Loading_ForGamePlay()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	// 컴포넌트 원형을 생성
	wsprintf(m_szLoading, TEXT("버퍼를 생성한다. "));
	if (FAILED(pGameInstance->AddPrototype(SCENE_GAMEPLAY, TEXT("Prototype_Componenet_TerrainBuffer"), TerrainBuffer::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Terrain.hlsl"),TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_GAMEPLAY, TEXT("Prototype_Component_CubeBuffer"), CubeBuffer::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Cube.hlsl")))))
		return E_FAIL;

	wsprintf(m_szLoading, TEXT("텍스쳐를 생성한다. "));
	
	if (FAILED(pGameInstance->AddPrototype(SCENE_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/SkyBox/Skybox2.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->AddPrototype(SCENE_GAMEPLAY, TEXT("Prototype_Component_Texture_Status_Yu"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Player_0.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_GAMEPLAY, TEXT("Prototype_Component_Texture_Status_Kanji"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Player_1.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_GAMEPLAY, TEXT("Prototype_Component_Texture_Status_Naoto"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Player_3.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_GAMEPLAY, TEXT("Prototype_Component_Texture_Status_Yukiko"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Player_4.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_GAMEPLAY, TEXT("Prototype_Component_Texture_HpBar"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/HpBar.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_GAMEPLAY, TEXT("Prototype_Component_Texture_MpBar"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/MpBar.png")))))
		return E_FAIL;

	_matrix PivotMat =XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->AddPrototype(SCENE_GAMEPLAY, TEXT("Prototype_Component_Model_Yu"), Model::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Yu/", "Yu.fbx", TEXT("../Bin/ShaderFiles/Shader_Model.hlsl"),PivotMat,Model::TYPE_ANIM))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_GAMEPLAY, TEXT("Prototype_Component_Model_Kanji"), Model::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Kanji/", "Kanji.fbx", TEXT("../Bin/ShaderFiles/Shader_Model.hlsl"), PivotMat, Model::TYPE_ANIM))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_GAMEPLAY, TEXT("Prototype_Component_Model_Naoto"), Model::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Naoto/", "Naoto.fbx", TEXT("../Bin/ShaderFiles/Shader_Model.hlsl"), PivotMat, Model::TYPE_ANIM))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_GAMEPLAY, TEXT("Prototype_Component_Model_Yukiko"), Model::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Yukiko/", "Yukiko.fbx", TEXT("../Bin/ShaderFiles/Shader_Model.hlsl"), PivotMat, Model::TYPE_ANIM))))
		return E_FAIL;

	if (FAILED(pGameInstance->AddPrototype(SCENE_GAMEPLAY, TEXT("Prototype_Component_Model_W_Yu"), Model::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Weapon/Yu/", "Weapon.fbx", TEXT("../Bin/ShaderFiles/Shader_Model.hlsl"), XMMatrixIdentity() , Model::TYPE_NONE))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_GAMEPLAY, TEXT("Prototype_Component_Model_W_Kan"), Model::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Weapon/Kanji/", "W_Kan.fbx", TEXT("../Bin/ShaderFiles/Shader_Model.hlsl"), XMMatrixIdentity(), Model::TYPE_NONE))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_GAMEPLAY, TEXT("Prototype_Component_Model_W_Yukiko"), Model::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Weapon/Yukiko/", "W_Yukiko.fbx", TEXT("../Bin/ShaderFiles/Shader_Model.hlsl"), XMMatrixIdentity(), Model::TYPE_NONE))))
		return E_FAIL;

	if (FAILED(pGameInstance->AddPrototype(SCENE_GAMEPLAY, TEXT("Prototype_Component_Model_Monster"), Model::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Monster/", "Monster.fbx", TEXT("../Bin/ShaderFiles/Shader_Model.hlsl"), PivotMat, Model::TYPE_ANIM))))
		return E_FAIL;
	/*if (FAILED(pGameInstance->AddPrototype(SCENE_GAMEPLAY, TEXT("Prototype_Component_Model_Entry"), Model::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Entry/", "Entry.fbx", TEXT("../Bin/ShaderFiles/Shader_Model.hlsl"), PivotMat, Model::TYPE_NONE))))
		return E_FAIL;*/

	PivotMat = XMMatrixScaling(0.01f, 0.01f, 0.01f)*XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->AddPrototype(SCENE_GAMEPLAY, TEXT("Prototype_Component_Collider"), Collider::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	//--------------
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_BtlPanel"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Battle/btlpanel.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_Frame"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Battle/Frame.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_SelectPanel"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Battle/SelectPanel.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_SelectBtn"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Battle/SelectBtn_%d.png"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_SkillIcon"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Battle/SkillIcon/SkillIcon_%d.png"), 16))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_BtlSate"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Battle/BtlState.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_BtnAnalysis"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Battle/BtlButton/Analysis_%d.png"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_BtnAttack"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Battle/BtlButton/Attack_%d.png"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_BtnEscape"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Battle/BtlButton/Escape_%d.png"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_BtnGuard"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Battle/BtlButton/Guard_%d.png"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_BtnItem"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Battle/BtlButton/Item_%d.png"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_BtnPersona"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Battle/BtlButton/Persona_%d.png"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_BtnSkill"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Battle/BtlButton/Skill_%d.png"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_BtnTactics"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Battle/BtlButton/Tactics_%d.png"), 2))))
		return E_FAIL;

	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_Black"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Battle/RePlay/Black.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_No"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Battle/RePlay/No_%d.png"),2))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_Yes"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Battle/RePlay/Yes_%d.png"),2))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_String"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Battle/RePlay/String.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_End"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Battle/RePlay/End.png")))))
		return E_FAIL;

	//Effect
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_Ziodyne"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Battle/Effect/Ziodyne.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_Agidyne"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Battle/Effect/Agidyne.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_Bufudyne"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Battle/Effect/Bufudyne.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_Galdyne"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Battle/Effect/Galdyne.png")))))
		return E_FAIL;
	

	//_matrix PivotMat = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Component_Model_Izanagi"), Model::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Persona/Izanagi/", "Izanagi.fbx", TEXT("../Bin/ShaderFiles/Shader_Model.hlsl"), PivotMat, Model::TYPE_ANIM))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Component_Model_TakeMikazuchi"), Model::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Persona/TakeMikazuchi/", "TakeMikazuchi.fbx", TEXT("../Bin/ShaderFiles/Shader_Model.hlsl"), PivotMat, Model::TYPE_ANIM))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Component_Model_SukunaHikona"), Model::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Persona/SukunaHikona/", "SukunaHikona.fbx", TEXT("../Bin/ShaderFiles/Shader_Model.hlsl"), PivotMat, Model::TYPE_ANIM))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Component_Model_KonohanaSakuya"), Model::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Persona/KonohanaSakuya/", "KonohanaSakuya.fbx", TEXT("../Bin/ShaderFiles/Shader_Model.hlsl"), PivotMat, Model::TYPE_ANIM))))
		return E_FAIL;
	PivotMat = XMMatrixScaling(0.03f, 0.03f, 0.03f)*XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Component_Model_Amenosagiri"), Model::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Monster/Amenosagiri/", "Amenosagiri.fbx", TEXT("../Bin/ShaderFiles/Shader_Model.hlsl"), PivotMat, Model::TYPE_ANIM))))
		return E_FAIL;
	PivotMat = XMMatrixScaling(0.01f, 0.01f, 0.01f)* XMMatrixTranslation(1, 0, 10);
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Component_Model_TurnBack"), Model::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/TurnBack/", "TurnBack.fbx", TEXT("../Bin/ShaderFiles/Shader_Model.hlsl"), PivotMat, Model::TYPE_NONE))))
		return E_FAIL;


	//객체원형을 생성
	wsprintf(m_szLoading, TEXT("객체를 생성한다. "));

	//Effect
	if (FAILED(pGameInstance->AddPrototype(TEXT("Prototype_GameObject_ZiodyneEffect"), ZiodyneEffect::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(TEXT("Prototype_GameObject_AgidyneEffect"), AgidyneEffect::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(TEXT("Prototype_GameObject_BufudyneEffect"), Bufudyne::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(TEXT("Prototype_GameObject_GaldyneEffect"), Galdyne::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	//
	if (FAILED(pGameInstance->AddPrototype(TEXT("Prototype_GameObject_Cube"), Cube::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->AddPrototype(TEXT("Prototype_GameObject_Camera"), TargetCamera::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->AddPrototype(TEXT("Prototype_GameObject_Player"), Player::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->AddPrototype(TEXT("Prototype_GameObject_Monster"), Monster::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	//if (FAILED(pGameInstance->AddPrototype(TEXT("Prototype_GameObject_Entry"), DungeonEntry::Create(m_pDevice, m_pDeviceContext))))
	//	return E_FAIL;

	//----------------
	if (FAILED(pGameInstance->AddPrototype(TEXT("Prototype_GameObject_BattleCamera"), BattleCamera::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->AddPrototype(TEXT("Prototype_GameObject_Turn"), Turn::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(TEXT("Prototype_GameObject_Persona"), Persona::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(TEXT("Prototype_GameObject_BattleBG"), BattleBG::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->AddPrototype(TEXT("Prototype_GameObject_Amenosagiri"), Amenosagiri::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	wsprintf(m_szLoading, TEXT("로딩이 완료되었습니다. "));

	m_isFinished = true;

	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

HRESULT Client::Loader::Loading_ForBattle()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	wsprintf(m_szLoading, TEXT("Create Texture"));

	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_BtlPanel"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Battle/btlpanel.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_BtnAnalysis"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Battle/Button/Analysis_%d.png"),2))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_BtnAttack"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Battle/Button/Attack_%d.png"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_BtnEscape"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Battle/Button/Escape_%d.png"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_BtnGuard"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Battle/Button/Guard_%d.png"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_BtnItem"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Battle/Button/Item_%d.png"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_BtnPersona"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Battle/Button/Persona_%d.png"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_BtnSkill"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Battle/Button/Skill_%d.png"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Componenet_Texture_BtnTactics"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Battle/Button/Tactics_%d.png"), 2))))
		return E_FAIL;


	_matrix PivotMat = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Component_Model_Izanagi"), Model::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Persona/Izanagi/", "Izanagi.fbx", TEXT("../Bin/ShaderFiles/Shader_Model.hlsl"), PivotMat, Model::TYPE_ANIM))))
		return E_FAIL;
	PivotMat = XMMatrixScaling(0.01f, 0.01f, 0.01f)* XMMatrixTranslation(1, 0, 10);
	if (FAILED(pGameInstance->AddPrototype(SCENE_BATTLE, TEXT("Prototype_Component_Model_TurnBack"), Model::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/TurnBack/", "TurnBack.fbx", TEXT("../Bin/ShaderFiles/Shader_Model.hlsl"), PivotMat, Model::TYPE_NONE))))
		return E_FAIL;


	wsprintf(m_szLoading, TEXT("객체를 생성한다. "));
	if (FAILED(pGameInstance->AddPrototype(TEXT("Prototype_GameObject_BattleCamera"), BattleCamera::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->AddPrototype(TEXT("Prototype_GameObject_Turn"), Turn::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(TEXT("Prototype_GameObject_Persona"), Persona::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->AddPrototype(TEXT("Prototype_GameObject_BattleBG"), BattleBG::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	RELEASE_INSTANCE(GameInstance);
	wsprintf(m_szLoading, TEXT("BattleScene"));

	m_isFinished = true;
	return S_OK;
}

void Client::Loader::PrintLoaderText()
{
	SetWindowText(g_hWnd, m_szLoading);
}

Loader * Client::Loader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, SCENE eNextScene)
{
	Loader* pInsatnce = new Loader(pDevice, pDeviceContext);
	if (FAILED(pInsatnce->Ready(eNextScene)))
	{
		MSGBOX("Failed to Creationg Lodaer");
		Safe_Release(pInsatnce);
	}

	return pInsatnce;
}

void Client::Loader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);
	DeleteCriticalSection(&m_CS);
	DeleteObject(m_hThread);
	CloseHandle(m_hThread);

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
