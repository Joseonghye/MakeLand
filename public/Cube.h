#pragma once
#include "Client_Define.h"
#include "GameObject.h"

namespace Engine
{
	class Texture;
	class Renderer;
	class CubeBuffer;
	class Transform;
}

namespace Client
{
	class Cube final : public GameObject
	{
	protected:
		explicit Cube(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		explicit Cube(const GameObject& rhs);
		virtual ~Cube() = default;
	public:
		virtual HRESULT Ready_Prototype() override;
		virtual HRESULT Ready(void* pArg) override;

		virtual	_int	Update(_double TimeDelta) override;
		virtual _int	LateUpdate(_double TimeDelta) override;

		virtual HRESULT		Render() override;

	private:
		virtual HRESULT			SetUpComponents();

	public:
		static Cube* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		virtual GameObject* Clone(void* pArg) override;
		virtual void Free() override;

	private:
		Renderer*				m_pRenderer; //> 검색을 통한게 아닌, 빠른 사용을 위한 보관
		CubeBuffer*				m_pCube;
		Texture*				m_pTexture;
		Transform*				m_pTransform;
	};

}