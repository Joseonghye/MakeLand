#pragma once
#include "Client_Define.h"
#include "GameObject.h"

namespace Engine
{
	class Transform;
	class RectBuffer;
	class Texture;
	class Renderer;
}
namespace Client
{
	class UIInGame : public GameObject
	{
	public:
		struct PosData
		{
			_float4 vPos;
		};

	public:
		explicit UIInGame(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		explicit UIInGame(const UIInGame& rhs);
		virtual ~UIInGame() = default;

	public:
		virtual HRESULT Ready_Prototype();
		virtual HRESULT Ready(void* pArg);
		virtual _int	Update(_double TimeDelta);
		virtual _int	LateUpdate(_double TimeDelta);
		virtual HRESULT Render();

	public:
		static UIInGame* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		virtual UIInGame* Clone(void* pArg);
		virtual void Free() override;

	protected:	
		Transform*	m_pTransform = nullptr;
		RectBuffer*	m_pRect = nullptr;
		Texture*	m_pTexture = nullptr;
		Renderer*	m_pRenderer = nullptr;

		_float	m_fFrame = 0.f;
		_uint	m_iAllFrameCount = 0; //이미지장수
		_float	m_fEffectPlaySpeed = 0.f; //재생속도 
		_uint	m_iCountX = 0;	//x장수
		_uint	m_iCountY = 0;	//Y장수 
	};

}