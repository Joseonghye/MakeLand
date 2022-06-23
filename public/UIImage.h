#pragma once
#include "Client_Define.h"
#include "BaseUI.h"

namespace Client
{
	class UIImage :public BaseUI
	{
	public:
		explicit UIImage(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		explicit UIImage(const UIImage& rhs);
		virtual ~UIImage() = default;

	public:
		void SetSlider(_bool isSlider) { m_isSlider = isSlider; }
		void SetPersent(_float per) { m_fPersent = per; }
		void Rotation(_float angle);

		void SetTextureIndex(_int Index) { m_iTextureIndex = Index;	}

	public:
		virtual HRESULT			Ready_Prototype();
		virtual HRESULT			Ready(void* pArg);
		virtual _int			Update(_double TimeDelta);
		virtual _int			LateUpdate(_double TimeDelta);
		virtual HRESULT			Render();

	private:
		virtual HRESULT		SetUpComponents();

	public:
		static UIImage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		virtual GameObject* Clone(void* pArg);
		virtual void Free() override;

	private:
		UIDESC			m_ImgDesc;

		_float			m_fPersent;
		_bool			m_isSlider;
		_float			m_fTime;
		_int			m_iTextureIndex;
	};

}