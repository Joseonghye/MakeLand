#pragma once
#include "Client_Define.h"
#include "Base.h"

namespace Client
{
	// Æ¯Á¤ ¾ÀÀÇ ¿øÇü°´Ã¼ »ý¼º 
	class Loader final : public Base
	{
	private:
		explicit Loader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		virtual ~Loader() = default;

	public:
		CRITICAL_SECTION		GetCriticalSection() const { return m_CS; }
		SCENE					GetNextSceneID() const { return m_eNextScene; }
		_bool					GetFinished() const { return m_isFinished; }

	public:
		HRESULT					Ready(SCENE eNextScene);
		HRESULT					Loading_ForLogo();
		HRESULT					Loading_ForGamePlay();
		HRESULT					Loading_ForBattle();

	public:
		void					PrintLoaderText();

	public:
		static Loader*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, SCENE eNextScene);
		virtual void			Free() override;

	private:
		ID3D11Device*			m_pDevice = nullptr;
		ID3D11DeviceContext*	m_pDeviceContext = nullptr;

	private:
		HANDLE					m_hThread;
		CRITICAL_SECTION		m_CS;

		_tchar					m_szLoading[MAX_PATH] = TEXT("");
		_bool					m_isFinished;

	private:
		SCENE					m_eNextScene;

	};

}