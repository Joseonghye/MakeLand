#pragma once
#include "Client_Define.h"
#include "BaseUI.h"

namespace Client
{
	class UIButton :public BaseUI
	{
	public:
		enum BTNSTATE { BTN_NORMAL, BTN_OVER, BTN_CLICK, BTN_END };
	public:
		explicit UIButton(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		explicit UIButton(const UIButton& rhs);
		virtual ~UIButton() = default;

	public:
		void	SetBtnState(BTNSTATE _state) { m_eBtnState = _state; }

	public:
		virtual HRESULT			Ready_Prototype();
		virtual HRESULT			Ready(void* pArg);
		virtual _int			Update(_double TimeDelta);
		virtual _int			LateUpdate(_double TimeDelta);
		virtual HRESULT			Render();

	private:
		virtual HRESULT		SetUpComponents();

	public:
		static UIButton* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		virtual GameObject* Clone(void* pArg);
		virtual void Free() override;

	private:
		UIDESC*			m_ImgDesc;
		BTNSTATE		m_eBtnState;
	};

}