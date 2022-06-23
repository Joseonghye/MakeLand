#pragma once
#include "Client_Define.h"
#include "GameObject.h"

namespace Engine
{
	class Renderer;
}
namespace Client
{
	class UIText :public GameObject
	{
	private:
		explicit UIText(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		explicit UIText(const UIText& rhs);
		virtual ~UIText() = default;

	public:
		virtual HRESULT Ready_Prototype();
		virtual HRESULT Ready(void* pArg);
		virtual _int	Update(_double TimeDelta);
		virtual _int	LateUpdate(_double TimeDelta);
		virtual HRESULT Render();

	public:
		void	SetText(string str);
		void	SetText(_tchar* str);
		void	SetPosition(_float2 vPos) { m_vPosition = vPos; }
		void	SetColor(_fvector vColor) { XMStoreFloat4(&m_vColor, vColor); }

	public:
		static UIText*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		virtual UIText* Clone(void* pArg) override;
		virtual void Free() override;

	private:
		_tchar*			m_pText = TEXT("");
		_float2			m_vPosition;
		_float4			m_vColor;

		Renderer* m_pRenderer;


	};

}