#pragma once
#include "Client_Define.h"
#include "Camera.h"

namespace Client {
	class TargetCamera final :public Camera
	{
	private:
		explicit				TargetCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		explicit				TargetCamera(const TargetCamera& rhs);
		virtual					~TargetCamera() = default;

	public:
		virtual HRESULT			Ready_Prototype();
		virtual HRESULT			Ready(void* pArg);
		virtual _int			Update(_double TimeDelta);
		virtual _int			LateUpdate(_double TimeDelta);
		virtual HRESULT			Render();

	public:
		static TargetCamera*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		virtual GameObject*		Clone(void* pArg) override;
		virtual void			Free() override;
	
	private:
		class Transform*	m_pTargetTransform;

		_bool				m_isRot = false;;
		_vector				m_vForward;
		_float				m_fDist= -3.f;
		_float				m_fAngle;
		_float				m_fHeight=5.f;
		_float				m_fRotSpeed = 3.f;

	};

}