#pragma once
#include "Client_Define.h"
#include "Engine_Define.h"

namespace Client
{
	class UIImage;
	class UIButton;
	class UIText;
	class SkillBtn
	{
	private:
		enum SKILLICON { ICON_PHY, ICON_FIRE, ICON_ICE, ICON_ELECTRIC, ICON_WIND, ICON_LIGHT, ICON_DARK, ICON_BUF, ICON_END };
	public:
		SkillBtn();
		~SkillBtn() = default;

	public:
		HRESULT Ready(_int fX,_int fY);
	
		void	SetButton(_bool visible, SKILLTYPE eSkillType = SKILL_END, string strSkillName = "", _int iUseStat = 0);
		void	UseButton(_bool isUsing);

	public:
		static SkillBtn* Create(_int fX, _int fY);
		void Free();

	private:
		UIButton*	m_pBtn;
		UIImage*	m_pIcon;
		UIText*		m_pNameTxt;
		UIText*		m_pCostTxt;

		_int		m_eSkillIcon;

	};

}