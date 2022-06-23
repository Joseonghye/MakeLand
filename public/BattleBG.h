#pragma once
#include "Client_Define.h"
#include "GameObject.h"

namespace Engine
{
	class Renderer;
	class Model;
}
namespace Client
{
	class BattleBG final: public GameObject
	{
	protected:
		explicit			BattleBG(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		explicit			BattleBG(const GameObject& rhs);
		virtual				~BattleBG() = default;
	public:
		virtual HRESULT		Ready_Prototype() override;
		virtual HRESULT		Ready(void* pArg) override;

		virtual	_int		Update(_double TimeDelta) override;
		virtual _int		LateUpdate(_double TimeDelta) override;

		virtual HRESULT		Render() override;

	private:
		HRESULT				SetUpComponents();

	public:
		static BattleBG*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		virtual GameObject* Clone(void* pArg) override;
		virtual void		Free() override;

	private:
		Renderer*			m_pRenderer; 
		Model*				m_pModel;
	};
}