#include "stdafx.h"
#include "../public/SceneGamePlay.h"
#include "DynamicCamera.h"
#include "GameInstance.h"
#include "SceneLoading.h"
#include "SceneBattle.h"
#include "PlayMgr.h"
Client::SceneGamePlay::SceneGamePlay(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:Scene(pDevice, pDeviceContext), m_pPlayMgr(PlayMgr::GetInstance())
{
}

HRESULT Client::SceneGamePlay::Ready()
{
	if (FAILED(__super::Ready()))
		return E_FAIL;

	if (FAILED(Ready_LightDesc()))
		return E_FAIL;

	if (FAILED(Ready_LayerPlayer(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_LayerBackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_LayerCamera(TEXT("Layer_Camera"))))
		return E_FAIL;

	m_pPlayMgr->ReadyToPlay();

	return S_OK;
}

_int Client::SceneGamePlay::Update(_double TimeDelta)
{
	if (0 > (__super::Update(TimeDelta)))
		return -1;

	m_pPlayMgr->Update();

	if (GetKeyState(VK_CONTROL) < 0)
	{
		GameInstance* pGameInstance = GET_INSTANCE(GameInstance);
		//if (FAILED(pGameInstance->OpenScene(SCENE_LOADING, SCENE_BATTLE,SceneLoading::Create(m_pDevice, m_pDeviceContext, SCENE_BATTLE))))
		if (FAILED(pGameInstance->OpenScene(SCENE_BATTLE, SCENE_BATTLE, SceneBattle::Create(m_pDevice, m_pDeviceContext))))
			return -1;

		RELEASE_INSTANCE(GameInstance);
		return 0;
	}

	return _int(0);
}

HRESULT Client::SceneGamePlay::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT Client::SceneGamePlay::Ready_LayerPlayer(const _tchar * pLayerTag)
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	GameObject* pOut;
	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Player"),nullptr,&pOut)))
		return E_FAIL;

	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_BufudyneEffect"), nullptr, &pOut)))
		return E_FAIL;

	RELEASE_INSTANCE(GameInstance);
	return S_OK;
}

HRESULT Client::SceneGamePlay::Ready_LayerBackGround(const _tchar * pLayerTag)
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	GameObject* pOut;

	//if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Entry"),nullptr, &pOut)))
	//	return E_FAIL;

	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Cube"), nullptr, &pOut)))
		return E_FAIL;

	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Monster"), nullptr, &pOut)))
		return E_FAIL;

	RELEASE_INSTANCE(GameInstance);
	return S_OK;
}

HRESULT Client::SceneGamePlay::Ready_LayerCamera(const _tchar * pLayerTag)
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	Camera::CAMERADESC CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(Camera::CAMERADESC));

	Transform* target = (Transform*)pGameInstance->GetComponent(SCENE_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform"));

	_vector vPos = target->GetState(Transform::STATE_POSITION);

	CameraDesc.vEye = _float3(0.f, 10.f, -7.f);
	CameraDesc.vAt= _float3(0.f, 0.f, 0.f);
	//XMStoreFloat3(&CameraDesc.vAt, vPos);//
	CameraDesc.vAxisY = _float3(0.f, 1.f, 0.f);

	CameraDesc.fFovy = XMConvertToRadians(60.f);
	CameraDesc.fAspect = _float(g_iWinCX) / g_iWinCY;
	CameraDesc.fNear = 0.2f; 
	CameraDesc.fFar = 300.f;

	CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(120.f);

	GameObject* pOut;

	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Camera"), &CameraDesc, &pOut)))
		return E_FAIL;

	RELEASE_INSTANCE(GameInstance);
	return S_OK;
}

HRESULT Client::SceneGamePlay::Ready_LightDesc()
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

SceneGamePlay * Client::SceneGamePlay::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	SceneGamePlay* pInstance = new SceneGamePlay(pDevice, pDeviceContext);

	if (FAILED(pInstance->Ready()))
	{
		MSGBOX("Failed to Creating Scene GamePlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::SceneGamePlay::Free()
{
	__super::Free();
	Safe_Release(m_pPlayMgr);
}
