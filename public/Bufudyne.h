#pragma once
#include "Client_Define.h"
#include "GameObject.h"

namespace Engine
{
	class Transform;
	class PointInstanceBuffer;
	class Texture;
	class Renderer;
}
namespace Client
{
	class Bufudyne :public GameObject
	{
	public:
		explicit Bufudyne(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		explicit Bufudyne(const Bufudyne& rhs);
		virtual ~Bufudyne() = default;

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
		static Bufudyne* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		virtual Bufudyne* Clone(void* pArg);


	private:
		Texture*				m_pTexture = nullptr;
		Renderer*				m_pRenderer = nullptr;
		Transform*				m_pTransform = nullptr;
		PointInstanceBuffer*	m_pBuffer = nullptr;
		_float	m_Time = 0;
	};

}