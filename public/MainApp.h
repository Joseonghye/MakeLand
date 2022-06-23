#pragma once
#include "Client_Define.h"
#include "Base.h"

namespace Engine
{
	class GameInstance;
	class Renderer;
}

namespace Client 
{
	class MainApp final :public Base
	{
	private:
		explicit				MainApp();
		virtual					~MainApp() = default;

	public:
		HRESULT					Ready();
		_int					Update(_double TimeDelta);
		HRESULT					Render();

	private:
		HRESULT					SetUp_StartScene(SCENE eScene);

		HRESULT					Ready_Font();
		HRESULT					Ready_ComponentPrototype();
		HRESULT					Ready_GameObjectPrototype();

	public:
		static MainApp*			Create();
		virtual void			Free() override;

	private:
		GameInstance*			m_pGameInstance;
		ID3D11Device*			m_pDevice = nullptr;
		ID3D11DeviceContext*	m_pDeviceContext = nullptr;

		Renderer*				m_pRenderer;

	private:
		_double				m_TimeAcc = 0.0;
		_uint				m_iNumRender = 0;
		_tchar				m_szFPS[MAX_PATH] = TEXT("");
	};

}
