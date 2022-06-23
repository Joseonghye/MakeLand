#pragma once
#include "Client_Define.h"
#include "GameObject.h"

namespace Engine
{
	class Texture;
	class Renderer;
	class RectBuffer;
}

namespace Client
{
	class BackGround final : public GameObject
	{
	public:
		typedef struct tagBackGroundDesc
		{
			_float		fX, fY, fZ;
		}BACKGROUNDDESC;

	protected:
		explicit BackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		explicit BackGround(const GameObject& rhs);
		virtual ~BackGround() = default;
	public:
		virtual HRESULT Ready_Prototype() override;
		virtual HRESULT Ready(void* pArg) override;

		virtual	_int	Update(_double TimeDelta) override;
		virtual _int	LateUpdate(_double TimeDelta) override;

		virtual HRESULT		Render() override;

	private:
		virtual HRESULT			SetUpComponents();

	public:
		static BackGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		virtual GameObject* Clone(void* pArg) override;
		virtual void Free() override;

	private:
		BACKGROUNDDESC			m_BackGroundDesc;
		Renderer*				m_pRenderer; //> 검색을 통한게 아닌, 빠른 사용을 위한 보관
		RectBuffer*				m_pRect;
		Texture*				m_pTexture;

	//	XMFLOAT4X4				m_WorldMat;
	//	XMFLOAT4X4				m_ProjMat;

		XMMATRIX				m_ProjMatrix;
		XMMATRIX				m_WorldMatrix;

	};

}