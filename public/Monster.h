#pragma once
#include "Client_Define.h"
#include "GameObject.h"

namespace Engine
{
	class Renderer;
	class Model;
	class Transform;
	class Collider;
}

namespace Client
{
	class Monster final : public GameObject
	{
	protected:
		explicit			Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		explicit			Monster(const GameObject& rhs);
		virtual				~Monster() = default;
	public:
		virtual HRESULT		Ready_Prototype() override;
		virtual HRESULT		Ready(void* pArg) override;

		virtual	_int		Update(_double TimeDelta) override;
		virtual _int		LateUpdate(_double TimeDelta) override;

		virtual HRESULT		Render() override;

	private:
		virtual HRESULT		SetUpComponents();
		void				SetState();

	public:
		static Monster*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		virtual GameObject* Clone(void* pArg) override;
		virtual void		Free() override;

	private:
		Renderer*			m_pRenderer; //> 검색을 통한게 아닌, 빠른 사용을 위한 보관
		Model*				m_pModel;
		Transform*			m_pTransform;
		Collider*			m_pCollider;

	private:
		STATE				m_eCurState;
		STATE				m_eNextState;
	};

}