#pragma once

#include "Client_Define.h"
#include "Scene.h"

namespace Client
{

	class SceneLogo final : public Scene
	{
	private:
		explicit SceneLogo(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		virtual ~SceneLogo() = default;

	public:
		virtual HRESULT Ready();
		virtual _int	Update(_double TimeDelta);
		virtual HRESULT	Render();

	private:
		HRESULT			Ready_LayerUI(const _tchar* pLayerTag);

	public:
		static SceneLogo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		virtual	void	Free() override;

	};

}