#include "..\public\GameInstance.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(GameInstance)

GameInstance::GameInstance()
	:m_pGraphicDevice(Graphic_Device::GetInstance()),
	m_pTimerMgr(TimerMgr::GetInstance()),
	m_pSceneMgr(SceneMgr::GetInstance()),
	m_pObjectMgr(ObjectMgr::GetInstance()),
	m_pComponentMgr(ComponentMgr::GetInstance()),
	m_pPipeLine(PipeLine::GetInstance()),
	m_pInputDevice(Input_Device::GetInstance()),
	m_pLightMgr(LightMgr::GetInstance()),
	m_pFontMgr(FontMgr::GetInstance()),
	m_pSoundMgr(SoundManager::GetInstance())
{
	Safe_AddRef(m_pSoundMgr);
	Safe_AddRef(m_pFontMgr);
	Safe_AddRef(m_pLightMgr);
	Safe_AddRef(m_pInputDevice);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pComponentMgr);
	Safe_AddRef(m_pObjectMgr);
	Safe_AddRef(m_pTimerMgr);
	Safe_AddRef(m_pSceneMgr);
	Safe_AddRef(m_pGraphicDevice);
}

HRESULT Engine::GameInstance::Initialize_Engine(bool isTool,HINSTANCE hInst,HWND hWnd, _uint iNumScene, Graphic_Device::WINMODE WinMode, _uint iWinCX, _uint iWinCY, ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppDeviceContextOut)
{
	if (nullptr == m_pGraphicDevice ||
		 nullptr == m_pObjectMgr)
		return E_FAIL;

	if (FAILED(m_pGraphicDevice->Ready_GraphicDevice(hWnd, WinMode, iWinCX, iWinCY, ppDeviceOut, ppDeviceContextOut)))
		return E_FAIL;

	if (!isTool) 
	{
		if (FAILED(m_pInputDevice->Ready_Input_Device(hInst, hWnd)))
			return E_FAIL;
	}

	if (FAILED(m_pSoundMgr->Ready_SoundMgr()))
		return E_FAIL;


	if (FAILED(m_pObjectMgr->ReserveManager(iNumScene)))
		return E_FAIL;

	if (FAILED(m_pComponentMgr->ReserveManager(iNumScene)))
		return E_FAIL;

	return S_OK;
}

_int Engine::GameInstance::Update_Engine(_double TimeDelta)
{
	if (nullptr == m_pSceneMgr ||
		 nullptr == m_pObjectMgr ||
		nullptr == m_pPipeLine ||
		nullptr == m_pInputDevice)
		return -1;

	_int iProgress = 0;

	//키보드 마우스 상태 보관
	if (FAILED(m_pInputDevice->SetUp_InputDeviceState()))
		return -1;

	iProgress = m_pSoundMgr->Update_SoundMgr();
	if (0 > iProgress)
		return -1;

	iProgress = m_pSceneMgr->Update(TimeDelta);
	if (0 > iProgress)
		return -1;

	iProgress= m_pObjectMgr->Update(TimeDelta);
	if (0 > iProgress)
		return -1;

	m_pPipeLine->Update();

	iProgress = m_pObjectMgr->LateUpdate(TimeDelta);
	if (0 > iProgress)
		return -1;

	return _int();
}

HRESULT Engine::GameInstance::Render_Engine()
{
	if (nullptr == m_pSceneMgr)
		return E_FAIL;

	m_pSceneMgr->Render();

	return S_OK;
}

HRESULT Engine::GameInstance::Clear_Engine(_uint iSceneIndex)
{
	if (nullptr == m_pObjectMgr)
		return E_FAIL;

	m_pObjectMgr->ClearLayer(iSceneIndex);

	return S_OK;
}

HRESULT Engine::GameInstance::Clear_BackBufferView(XMFLOAT4 vClearColor)
{
	if (nullptr == m_pGraphicDevice)
		return E_FAIL;

	return m_pGraphicDevice->Clear_BackBufferView(vClearColor);
}

HRESULT Engine::GameInstance::Clear_DepthStencilView()
{
	if (nullptr == m_pGraphicDevice)
		return E_FAIL;

	return m_pGraphicDevice->Clear_DepthStencilView();
}

HRESULT Engine::GameInstance::Present()
{
	if (nullptr == m_pGraphicDevice)
		return E_FAIL;

	return m_pGraphicDevice->Present();
}

_byte Engine::GameInstance::Get_DIKeyState(_ubyte byKeyID) const
{
	if (nullptr == m_pInputDevice)
		return 0;

	return m_pInputDevice->Get_DIKeyState(byKeyID);
}

_long Engine::GameInstance::Get_MouseMoveState(Input_Device::MOUSEMOVESTATE eMoveState) const
{
	if (nullptr == m_pInputDevice)
		return 0;

	return m_pInputDevice->Get_MouseMoveState(eMoveState);
}

_byte Engine::GameInstance::Get_MouseButtonState(Input_Device::MOUSEBUTTONSTATE eButtonState) const
{
	if (nullptr == m_pInputDevice)
		return 0;

	return m_pInputDevice->Get_MouseButtonState(eButtonState);
}

_bool Engine::GameInstance::GetKeyDown(_ubyte key) const
{
	if (nullptr == m_pInputDevice)
		return 0;

	return m_pInputDevice->GetKeyDown(key);
}

_double Engine::GameInstance::GetTimeDelta(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimerMgr)
		return 0.0;

	return m_pTimerMgr->GetTimeDelta(pTimerTag);
}

void Engine::GameInstance::Update_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimerMgr)
		return;

	return m_pTimerMgr->Update_Timer(pTimerTag);
}

HRESULT Engine::GameInstance::Ready_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimerMgr)
		return E_FAIL;

	return m_pTimerMgr->Ready_Timer(pTimerTag);
}

HRESULT Engine::GameInstance::OpenScene(_uint iSceneIndex,_uint iNextSceneIndex , Scene * pOpenScene)
{
	if (nullptr == m_pSceneMgr)
		return E_FAIL;

	return m_pSceneMgr->OpenScene(iSceneIndex, iNextSceneIndex,pOpenScene);
}

Component * Engine::GameInstance::GetComponent(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iIndex)
{
	return m_pObjectMgr->GetComponent(iLevelIndex, pLayerTag, pComponentTag, iIndex);
}

list<class GameObject*> Engine::GameInstance::GetGameObjectsToLayer(_uint iLevelIndex, const _tchar * pLayerTag)
{
	return m_pObjectMgr->GetGameObjectsToLayer(iLevelIndex, pLayerTag);
}

HRESULT Engine::GameInstance::AddPrototype(const _tchar * pPrototypeTag, GameObject * pPrototype)
{
	if (nullptr == m_pObjectMgr || nullptr == pPrototype)
		return E_FAIL;

	return m_pObjectMgr->AddPrototype(pPrototypeTag, pPrototype);
}

HRESULT Engine::GameInstance::AddGameObjectToLayer(_uint iSceneIndex, const _tchar * pLayerTag, const _tchar * pPrototypeTag, void * pArg, GameObject** pOut)
{
	if (nullptr == m_pObjectMgr)
		return E_FAIL;

	return m_pObjectMgr->AddGameObjectToLayer(iSceneIndex, pLayerTag, pPrototypeTag, pArg,pOut);
}

HRESULT Engine::GameInstance::AddPrototype(_uint iSceneIndex, const _tchar * pPrototypeTag, Component * pPrototype)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	return m_pComponentMgr->AddPrototype(iSceneIndex, pPrototypeTag, pPrototype);
}

Component * Engine::GameInstance::CloneComponent(_uint iSceneIndex, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pComponentMgr)
		return nullptr;

	return m_pComponentMgr->CloneComponent(iSceneIndex, pPrototypeTag, pArg);
}

Camera * Engine::GameInstance::GetMainCamera()
{
	return m_pPipeLine->GetMainCamera();
}

_fmatrix Engine::GameInstance::GetTransform(PipeLine::TRANSFORMSTATEMATRIX eType)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->GetTransform(eType);
}

_fvector Engine::GameInstance::GetCamPos()
{
	if (nullptr == m_pPipeLine)
		return XMVectorZero();

	return m_pPipeLine->GetCamPos();
}

void Engine::GameInstance::SetMainCamera(Camera * cam)
{
	m_pPipeLine->SetMainCamera(cam);
}

void Engine::GameInstance::SetTransform(PipeLine::TRANSFORMSTATEMATRIX eType, _fmatrix MatTrans)
{
	if (nullptr == m_pPipeLine)
		return;

	m_pPipeLine->SetTransform(eType, MatTrans);
}

const LIGHTDESC * Engine::GameInstance::GetLightDesc(_uint iIndex)
{
	if (nullptr == m_pLightMgr)
		return nullptr;

	return m_pLightMgr->GetLightDesc(iIndex);
}

HRESULT Engine::GameInstance::AddLight(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const LIGHTDESC & LightDesc)
{
	if (nullptr == m_pLightMgr)
		return E_FAIL;

	return m_pLightMgr->AddLight(pDevice, pDeviceContext, LightDesc);
}

HRESULT Engine::GameInstance::AddFont(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar * pFontTag, const _tchar * pFontPath)
{
	if (nullptr == m_pFontMgr)
		return E_FAIL;

	return m_pFontMgr->AddFont(pDevice, pDeviceContext, pFontTag, pFontPath);
}

HRESULT Engine::GameInstance::Render_Font(const _tchar * pFontTag, XMFLOAT2 vPos, const _tchar* pString, _fvector vColor)
{
	if (nullptr == m_pFontMgr)
		return E_FAIL;

	return m_pFontMgr->Render_Font(pFontTag, vPos, pString, vColor);
}
//HRESULT Engine::GameInstance::LoadWaveFile(char * filename)
//{
//	if (nullptr == m_pSoundMgr)
//		return E_FAIL;
//
//	return m_pSoundMgr->LoadWaveFile(filename);
//}
//HRESULT Engine::GameInstance::PlayWavFile()
//{
//	if (nullptr == m_pSoundMgr)
//		return E_FAIL;
//
//	return m_pSoundMgr->PlayWavFile();
//}
//
HRESULT Engine::GameInstance::PlaySound(TCHAR * pSoundKey)
{
	if (nullptr == m_pSoundMgr)
		return E_FAIL;

	m_pSoundMgr->PlaySound(pSoundKey);
	return S_OK;
}

HRESULT Engine::GameInstance::PlayBGM(TCHAR * pSoundKey)
{
	if (nullptr == m_pSoundMgr)
		return E_FAIL;

	m_pSoundMgr->PlayBGM(pSoundKey);
	return S_OK;
}

void Engine::GameInstance::OnSuspending()
{
	if (nullptr == m_pSoundMgr)
		return;
	m_pSoundMgr->OnSuspending();
}

void Engine::GameInstance::OnResuming()
{
	if (nullptr == m_pSoundMgr)
		return;
	m_pSoundMgr->OnResuming();
}

//HRESULT Engine::GameInstance::StopSound(SoundMgr::CHANNELID eID)
//{
//	if (nullptr == m_pSoundMgr)
//		return E_FAIL;
//
//	m_pSoundMgr->StopSound(eID);
//	return S_OK;
//}

//HRESULT Engine::GameInstance::StopAll()
//{
//	if (nullptr == m_pSoundMgr)
//		return E_FAIL;
//
//	m_pSoundMgr->StopAll();
//	return S_OK;
//}

void Engine::GameInstance::Release_Engine()
{
	if (0 != GameInstance::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Release GameInstance");

	if (0 != SceneMgr::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Release SceneMgr");

	if (0 != SoundManager::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Release SoundMgr");

	if (0 != ObjectMgr::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Release ObjectMgr");

	if (0 != ComponentMgr::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Release ComponentMgr");

	if (0 != FontMgr::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Release FontMgr");

	if (0 != LightMgr::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Release LightMgr");

	if (0 != PipeLine::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Release PipeLine");

	if (0 != TimerMgr::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Release TimerMgr");

	if (0 != Input_Device::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Release Input_Device");

	if (0 != Graphic_Device::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Release Graphic_Device");
}

void Engine::GameInstance::Free()
{
	Safe_Release(m_pSoundMgr);
	Safe_Release(m_pFontMgr);
	Safe_Release(m_pLightMgr);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pObjectMgr);
	Safe_Release(m_pSceneMgr);
	Safe_Release(m_pComponentMgr);
	Safe_Release(m_pTimerMgr);
	Safe_Release(m_pInputDevice);
	Safe_Release(m_pGraphicDevice);
}
