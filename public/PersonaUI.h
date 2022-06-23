#pragma once
#include "Base.h"
#include "Client_Define.h"

namespace Client
{
	class UIImage;
	class PersonaUI :public Base
	{
	public:
		explicit PersonaUI();
		virtual ~PersonaUI() = default;
		
	public:
		void SetVisible(_bool visible);

	public:
		virtual void Free() override;

	private:
		UIImage*	m_pStatusImg;
	};

}