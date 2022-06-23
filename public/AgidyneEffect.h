#pragma once
#include "UIInGame.h"

namespace Client
{
	class AgidyneEffect : public UIInGame
	{
	public:
		explicit AgidyneEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		explicit AgidyneEffect(const UIInGame& rhs);
		virtual ~AgidyneEffect() = default;

	public:
		virtual HRESULT Ready_Prototype();
		virtual HRESULT Ready(void* pArg);
		virtual _int	Update(_double TimeDelta);
		virtual _int	LateUpdate(_double TimeDelta);
		virtual HRESULT Render();

	private:
		HRESULT SetUpComponents();
		virtual void Free() override;

	public:
		static AgidyneEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		virtual AgidyneEffect* Clone(void* pArg);
	};

}