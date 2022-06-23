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
	class Galdyne :public GameObject
	{
	public:
		explicit Galdyne(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		explicit Galdyne(const Galdyne& rhs);
		virtual ~Galdyne() = default;

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
		static Galdyne* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		virtual Galdyne* Clone(void* pArg);


	private:
		Texture*				m_pTexture = nullptr;
		Renderer*				m_pRenderer = nullptr;
		Transform*				m_pTransform = nullptr;
		PointInstanceBuffer*	m_pBuffer = nullptr;
		_float	m_Time = 0;

		_float	m_fFrame = 0.f;
		_uint	m_iAllFrameCount = 0; //이미지장수
		_float	m_fEffectPlaySpeed = 0.f; //재생속도 
		_uint	m_iCountX = 0;	//x장수
		_uint	m_iCountY = 0;	//Y장수 
	};

}