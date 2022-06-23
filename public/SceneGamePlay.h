#pragma once

#include "Client_Define.h"
#include "Scene.h"

namespace Client
{
	class SceneGamePlay final : public Scene
	{
	private:
		explicit				SceneGamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		virtual					~SceneGamePlay() = default;

	public:
		virtual HRESULT			Ready();
		virtual _int			Update(_double TimeDelta);
		virtual HRESULT			Render();

	private:
		HRESULT					Ready_LayerPlayer(const _tchar* pLayerTag);
		HRESULT					Ready_LayerBackGround(const _tchar* pLayerTag);
		HRESULT					Ready_LayerCamera(const _tchar* pLayerTag);

		HRESULT					Ready_LightDesc();

	public:
		static SceneGamePlay*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		virtual	void			Free() override;

	private:
		class PlayMgr*			m_pPlayMgr;
	};

}