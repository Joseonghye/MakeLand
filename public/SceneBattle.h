#pragma once
#include "Client_Define.h"
#include "Scene.h"

namespace Client
{
	class SceneBattle : public Scene
	{
	private:
		explicit SceneBattle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		virtual ~SceneBattle() = default;

	public:
		virtual HRESULT			Ready();
		virtual _int			Update(_double TimeDelta);
		virtual HRESULT			Render();

	private:
		HRESULT					Ready_LayerPlayer(const _tchar* pLayerTag);
		HRESULT					Ready_LayerEnemy(const _tchar* pLayerTag);
		HRESULT					Ready_LayerBackGround(const _tchar* pLayerTag);
		HRESULT					Ready_LayerUI(const _tchar* pLayerTag);
		HRESULT					Ready_LayerSelectUI(const _tchar* pLayerTag);
		HRESULT					Ready_LayerCamera(const _tchar* pLayerTag);

		HRESULT					Ready_LightDesc();
	public:
		static SceneBattle*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		virtual	void			Free() override;

	private:
		class PlayMgr*			m_pPlayMgr;
		class BtlMgr*			m_pBtlMgr;
	};
}