#pragma once
#include "Client_Define.h"
#include "GameObject.h"

namespace Engine
{
	class Model;
	class Transform;
	class Renderer;
}
namespace Client {

	class Persona : public GameObject
	{
	public:
		explicit Persona(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		explicit Persona(Persona& rhs);
		virtual ~Persona() = default;
	
	public:
		PERSONADATA*	GetPersonaData() { return m_pPersonaData; }
		vector<SKILLDATA> GetPersonaSkills();

		void	SetVisible(_bool visible) { m_Visible = visible; }
		void	SetPosition(_vector vPos,_vector vLook);

	public:
		virtual HRESULT		Ready_Prototype() override;
		virtual HRESULT		Ready(void* pArg) override;

		virtual	_int		Update(_double TimeDelta) override;
		virtual _int		LateUpdate(_double TimeDelta) override;

		virtual HRESULT		Render() override;

	private:
		virtual HRESULT		SetUpComponents();

	public:
		static Persona*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		virtual Persona* Clone(void* pArg) override;
		virtual void		Free() override;

	private:
		PERSONADATA*	m_pPersonaData;

		Transform*		m_pTransform;
		Model*			m_pModel;
		Renderer*		m_pRenderer;

		_bool			m_Visible;
	};

}