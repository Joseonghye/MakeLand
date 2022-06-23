#pragma once
#include "Client_Define.h"
#include "Base.h"

namespace Engine
{
	class GameObject;
}

namespace Client 
{
	class BtlMgr final :public Base
	{
		DECLARE_SINGLETON(BtlMgr)
		
	public:
		enum BtnName { BN_ANALYSIS, BN_TATCTICS, BN_GUARD, BN_ATTACK, BN_SKILL, BN_PERSONA, BN_ITEM, BN_ESCAPE, BN_END };
	public:
		explicit BtlMgr();
		virtual ~BtlMgr() = default;

	public:
		HRESULT		ReadyBattle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, string strBattleType);
		_int		Update(_double TimeDelta);
		_int		LateUpdate(_double TimeDelta);

		_bool		TurnIsDead(_int iIndex);
		void		NextTurn();
		
		void		MovingCamera(_fvector vPos, _fvector vTarget,_fvector vDir = VEC_ZERO);
		void		MovingCamera(_int vTargetIndex, _fvector vTarget, _fvector vDir = VEC_ZERO);
		void		TotalCamera(_int iNum,_fvector vDir= VEC_UP);
		void		LookTurn();

		void		StartSelectTarget();
		void		ChangeTarget();
		_fvector	SelectedTarget();

		void		Escape();
		void		PlayerDead();
		void		EnemyDead();
		void		ReturnGameScene();

		void		End();

		void		EnemyUsingSkill(_float fEnemyStr,SKILLDATA* pSkill, int iIndex);
		void		TurnUsingSkill(_float fTurnStr, SKILLDATA* pSkill = nullptr);
	
		void		SetSkillSelect(_bool visible, _int iStartIndex = 0 , vector<SKILLDATA> skills = vector<SKILLDATA>());
		void		SetStateUI(_bool visible, string strText = "");

	private:
		void		CreateSkillEffect(SKILLTYPE eSkillType,_vector vPos);

		void		SetVisibleBtnUI(_bool visible);
		void		BattleTurn();
		void		SelectAction();
		void		SelectSkill();

	private:
		virtual void Free() override;

	private:
		ID3D11Device*			m_pDevice;
		ID3D11DeviceContext*	m_pDeviceContext;

	private:
		class BtlStateUI*		m_pBtlStateUI;
		vector<class UIButton*> m_pButtons;
		GameObject*				m_pBtnBack;
//		GameObject*				m_pBattleFrameImg;
		_int					m_iBtnIndex;
		_bool					m_VisibleBtn;

	private:
		GameObject*				m_pSelectPanel;
		vector<class SkillBtn*> m_pSkillButtons;
		vector<SKILLDATA>		m_TurnSkills;
		_bool					m_VisibleSelect;
		_bool					m_bPageDown;

	private:
		class RePlayPanel*		m_pReplayPanel;

	private:
		class BattleCamera*		m_pCamera;
		_float4 				m_vPositions[5];
		vector<class Turn*>		m_pPlayerTurns;
		vector<class Turn*>		m_pEnemyTurns;
		vector<_int>			m_PlayTurn;

		_bool				m_bStart = false;
		_int	m_iCameraCnt = 0;
		_bool m_bWait = false;
		_float m_Time = 0;

		_bool					m_isEnemyDead = false;
		_bool					m_isPlayerDead = false;

		BATTLE					m_eCurBattleState;	//전투상태
		BATTLE					m_eNextBattleState;
		_int					m_iCurTurn;			//현재 턴
		_bool					m_isSelect;			//플레이어가 선택중인가 

		_bool					m_ChoiceTarget;		// 타겟 선택중?
		_int					m_iTargetNum;		// 선택된 타겟 
	};
}