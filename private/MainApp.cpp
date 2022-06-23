#include "stdafx.h"
#include "..\public\MainApp.h"

#include "GameInstance.h"
#include "SceneLoading.h"
#include "BackGround.h"
#include "UIImage.h"
#include "UIButton.h"
#include "UIText.h"
#include "PointInstanceBuffer.h"

MainApp::MainApp()
	: m_pGameInstance(GameInstance::GetInstance()),
		m_pDevice(nullptr), m_pDeviceContext(nullptr)
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT MainApp::Ready()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	if(FAILED(m_pGameInstance->Initialize_Engine(false,g_hInst,g_hWnd,SCENE_END,Graphic_Device::WINMODE::MODE_WIN, g_iWinCX, g_iWinCY, &m_pDevice, &m_pDeviceContext)))
		return E_FAIL;

	if (FAILED(Ready_Font()))
		return E_FAIL;
	if (FAILED(Ready_ComponentPrototype()))
		return E_FAIL;
	if (FAILED(Ready_GameObjectPrototype()))
		return E_FAIL;

	if (FAILED(SetUp_StartScene(SCENE_LOGO)))
		return E_FAIL;

	return S_OK;
}

_int MainApp::Update(_double TimeDelta)
{
	if (nullptr == m_pGameInstance)
		return -1;

	m_TimeAcc += TimeDelta;

	m_pGameInstance->Update_Engine(TimeDelta);

	return _int();
}

HRESULT MainApp::Render()
{
	if(nullptr == m_pGameInstance)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Clear_BackBufferView(XMFLOAT4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/*if (FAILED(m_pGameInstance->Clear_BackBufferView(XMFLOAT4(0.f, 0.f, 1.f, 1.f))))
		return E_FAIL;*/
	if (FAILED(m_pGameInstance->Clear_DepthStencilView()))
		return E_FAIL;

	//게임을 구성하는 객체들의 렌더함수 호출
	if (FAILED(m_pRenderer->DrawRenderGroup()))
		return E_FAIL;
	// 게임 내의 기타 등등을 렌더 
	if (FAILED(m_pGameInstance->Render_Engine()))
		return E_FAIL;
//
//#ifdef _DEBUG
//
//	++m_iNumRender;
//
//	if (m_TimeAcc >= 1.0)
//	{
//		wsprintf(m_szFPS, TEXT("프레임 : %d"), m_iNumRender);
//		m_iNumRender = 0;
//		m_TimeAcc = 0.0;
//	}
//
//	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Persona"), _float2(0.f,0.f), m_szFPS, XMVectorSet(1.f, 0.f, 0.f, 1.f))))
//		return E_FAIL;
//
//#endif // _DEBUG

	if (FAILED(m_pGameInstance->Present()))
		return E_FAIL;

	return S_OK;
}

HRESULT Client::MainApp::SetUp_StartScene(SCENE eScene)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	HRESULT hr = 0;

	switch (eScene)
	{
	case SCENE_LOGO:
		hr = m_pGameInstance->OpenScene(SCENE_LOADING, SCENE_LOGO, SceneLoading::Create(m_pDevice, m_pDeviceContext, eScene));
		break;
	//case SCENE_GAMEPLAY:
	default:
		return E_FAIL;
	}

	if(FAILED(hr))
		return E_FAIL;

	return S_OK;
}

HRESULT Client::MainApp::Ready_Font()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	//Downloads\DirectXTK - dec2019\MakeSpriteFont\bin\Release\MakeSpriteFont "AuctionGothic" / FontSize:16 / FastPack / CharacterRegion : 0x0020 - 0x00FF / CharacterRegion : 0x3131 - 0x3163 / CharacterRegion : 0xAC00 - 0xD800 / DefaultCharacter : 0xAC00 / TextureFormat : Rgba32 Persona.spritefon
	//Downloads\DirectXTK-dec2019\MakeSpriteFont\bin\Release\MakeSpriteFont "옥션고딕 M" /FontSize:16 /DefaultCharacter:0xAC00 /TextureFormat:Rgba32 /FeatureLevel:FL11_0 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 ..\Persona.spritefont
		// MakeSpriteFont "Gulim" /FontSize:16 /characterregion:0xa960 - 0xa97f Gulim.spritefont /TextureFormat:Rgba32
	//Downloads\DirectXTK-dec2019\MakeSpriteFont\bin\Release\MakeSpriteFont "서울한강체 M" /FontSize:16 /DefaultCharacter:0xAC00 /FastPack /CharacterRegion:0xA960-0xA97f /CharacterRegion:0xAC00-0xD800 Persona.spritefont
	if (FAILED(m_pGameInstance->AddFont(m_pDevice, m_pDeviceContext, TEXT("Font_Persona"), TEXT("../bin/Resources/Fonts/Persona.spritefont"))))
		return E_FAIL;


	return S_OK;
}

HRESULT Client::MainApp::Ready_ComponentPrototype()
{
	//initialize Engine 으로 옮겨도 될듯함.

	if (nullptr == m_pGameInstance)
		return E_FAIL;

	//원형 추가 
	if (FAILED(m_pGameInstance->AddPrototype(SCENE_STATIC,TEXT("Prototype_Component_Renderer"), m_pRenderer = Renderer::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->AddPrototype(SCENE_STATIC, TEXT("Prototype_Component_Transform"), Transform::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->AddPrototype(SCENE_STATIC, TEXT("Prototype_Component_RectBuffer"), RectBuffer::Create(m_pDevice, m_pDeviceContext,TEXT("../bin/ShaderFiles/Shader_Rect.hlsl")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->AddPrototype(SCENE_STATIC, TEXT("Prototype_Component_PointInstanceBuffer"), PointInstanceBuffer::Create(m_pDevice, m_pDeviceContext, TEXT("../bin/ShaderFiles/Shader_PointInstance.hlsl"),20))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->AddPrototype(SCENE_STATIC, TEXT("Prototype_Component_Texture_Loading"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../bin/Resources/Textures/Title.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->AddPrototype(SCENE_STATIC, TEXT("Prototype_Component_Texture_LoadingUI"), Texture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Loading/Loading_%d.png"),3))))
		return E_FAIL;

	return S_OK;
}

HRESULT Client::MainApp::Ready_GameObjectPrototype()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	if (FAILED(m_pGameInstance->AddPrototype(TEXT("Prototype_GameObject_BackGround"), BackGround::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->AddPrototype(TEXT("Prototype_GameObject_Image"), UIImage::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->AddPrototype(TEXT("Prototype_GameObject_Button"), UIButton::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->AddPrototype(TEXT("Prototype_GameObject_Text"), UIText::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	Safe_AddRef(m_pRenderer);

	return S_OK;
}

MainApp * MainApp::Create()
{
	MainApp*	pInstance = new MainApp();

	if (FAILED(pInstance->Ready()))
	{
		MSGBOX("Failed to Creating MainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void MainApp::Free()
{
	Safe_Release(m_pRenderer);

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);

	Safe_Release(m_pGameInstance);
	GameInstance::Release_Engine();
}