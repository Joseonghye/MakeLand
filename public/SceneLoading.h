#pragma once
#include "Client_Define.h"
#include "Scene.h"

namespace Client
{
	class SceneLoading final : public Scene
	{
	private:
		 explicit				SceneLoading(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		 virtual				~SceneLoading() = default;

	public:
		virtual HRESULT			Ready(SCENE eNextScene);
		virtual _int			Update(_double TimeDelta);
		virtual HRESULT			Render();

	private:
		HRESULT					Ready_LayerBackGround(const _tchar* pLayerTag);
		HRESULT					Ready_LayerUI(const _tchar* pLayerTag);

		HRESULT					Ready_LightDesc();
	private:
		HRESULT					OpenScene();

	public:
		static SceneLoading*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, SCENE eNextScene);
		virtual	void			Free() override;

	private:
		SCENE					m_eNextScene;
		class Loader*			m_pLoader;
		
		HWND	m_hVideo;
	};

}