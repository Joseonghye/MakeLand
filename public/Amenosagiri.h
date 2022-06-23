#pragma once
#include "Turn.h"

namespace Client
{
	class UIText;
	class Amenosagiri final :public Turn
	{
	private:
		enum AME_PATTEN { PATTEN_ONE, PATTEN_TWO, PATTEN_THREE, PATTEN_FOUR,PATTEN_END };
		enum AME_Ani { AMEANI_IDLE, AMEANI_SKILL0, AMEANI_SKILL1, AMEANI_NEBRO, AMEANI_FOG, AMEANI_FOGIDLE, AMEANI_CONCENT, AMEANI_DEATH };
		enum AME_ACT { AMEACT_START, AMEACT_WAIT, AMEACT_TARGET, AMEACT_ATTACK, AMEACT_END };
	public:
		explicit	Amenosagiri(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		explicit	Amenosagiri(const Amenosagiri& rhs);
		virtual		~Amenosagiri() = default;

	public:
		virtual HRESULT		Ready_Prototype() override;
		virtual HRESULT		Ready(void* pArg) override;

		virtual _int		Update(_double TimeDelta) override;
		virtual _int		LateUpdate(_double TimeDelta) override;

		virtual HRESULT		Render()override;
	
		virtual void		SetPlay(_bool isturn) override;
		virtual _bool		Damaged(_float fEnemyStr, SKILLDATA* pSkill=nullptr);
		
		void				Dead(_int iNum);

	private:
		void				PattenOne(_double TimeDelta);
		void				PattenTwo(_double TimeDelta);
		void				PattenThree(_double TimeDelta);
		void				PattenFour(_double TimeDelta);

		

	protected:
		virtual HRESULT		SetUpComponents();

	public:
		static Amenosagiri*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		virtual Amenosagiri*	Clone(void* pArg) override;

	private:
		vector<SKILLDATA*>  m_pSkills;
		ENEMYDATA*			m_pEnemyStaus;

		AME_PATTEN			m_ePatten;
		AME_ACT				m_eAct;
		AME_ACT				m_eNextAct;

		_int				m_iDeadNum = 0;
		_int				m_iTurnCnt;
		_int				m_iSkillIndex;
		_int				m_iTarget;

		_float				m_ChkTime;

		_int				m_iBufCnt;
		_bool				m_useEffect;
		_bool				m_useFog;

		UIText*				m_pHpText;
		_bool				m_bFirst=true;

	};

}