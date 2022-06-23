#pragma once
#include "Client_Define.h"
#include "Camera.h"

namespace Client {
	class DynamicCamera final :public Camera
	{
	private:
		explicit				DynamicCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		explicit				DynamicCamera(const DynamicCamera& rhs);
		virtual					~DynamicCamera() = default;

	public:
		virtual HRESULT			Ready_Prototype();
		virtual HRESULT			Ready(void* pArg);
		virtual _int			Update(_double TimeDelta);
		virtual _int			LateUpdate(_double TimeDelta);
		virtual HRESULT			Render();

	public:
		static DynamicCamera*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		virtual GameObject*		Clone(void* pArg) override;
		virtual void			Free() override;
	};

}