#pragma once
#include "Client_Define.h"
#include "Base.h"

namespace Client
{
	class UIImage;
	class UIText;
	class BtlStateUI : public Base
	{
	public:
		BtlStateUI();
		virtual ~BtlStateUI() = default;

	public:
		HRESULT Ready(_int fX, _int fY);

		void SetVisible(_bool visible);
		void SetText(string str);

	public:
		static BtlStateUI* Create(_int fX, _int fY);
		void Free();

	private:
		UIImage*	m_pFrameImg = nullptr;
		UIText*		m_pText = nullptr;
	};

}