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
	class DungeonEntry final: public GameObject
	{
	protected:
		explicit			DungeonEntry(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		explicit			DungeonEntry(const GameObject& rhs);
		virtual				~DungeonEntry() = default;
	public:
		virtual HRESULT		Ready_Prototype() override;
		virtual HRESULT		Ready(void* pArg) override;

		virtual	_int		Update(_double TimeDelta) override;
		virtual _int		LateUpdate(_double TimeDelta) override;

		virtual HRESULT		Render() override;

	private:
		HRESULT				SetUpComponents();

	public:
		static DungeonEntry*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		virtual GameObject* Clone(void* pArg) override;
		virtual void		Free() override;

	private:
		Renderer*			m_pRenderer; 
		Model*				m_pModel;
	};
}