#pragma once
#include "UIInGame.h"

namespace Client
{
	class ZiodyneEffect : public UIInGame
	{
	public:
		explicit ZiodyneEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		explicit ZiodyneEffect(const UIInGame& rhs);
		virtual ~ZiodyneEffect() = default;

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
		static ZiodyneEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		virtual ZiodyneEffect* Clone(void* pArg);
	};
}