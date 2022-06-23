#pragma once
#include "Client_Define.h"
#include "Camera.h"

namespace Client {
	class BattleCamera : public Camera
	{
	public:
		explicit BattleCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		explicit BattleCamera(const BattleCamera& rhs);
		virtual ~BattleCamera() = default;

	public:
		virtual HRESULT			Ready_Prototype();
		virtual HRESULT			Ready(void* pArg);
		virtual _int			Update(_double TimeDelta);
		virtual _int			LateUpdate(_double TimeDelta);
		virtual HRESULT			Render();

	public:
		void					MovingStop() { m_isMove = false; }
		void					Moving(_fvector vPos, _fvector vTarget, _fvector vDir);
		void					SetCamera(_vector vPos, _vector vLook , _float vecScale = 1);
		void					LookAt(_vector vTarget);

	public:
		static BattleCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		virtual GameObject * Clone(void * pArg) override;
		virtual void Free() override;

	private:
		_bool					m_isMove = false;
		_float3					m_vMoveDir;
	};

}