#pragma once
#include "Client_Define.h"
#include "GameObject.h"

namespace Engine
{
	class Renderer;
	class Model;
	class Transform;
	class HierarchyNode;
}

namespace Client
{
	class Persona;
	class BtlMgr;

	class Turn : public GameObject
	{
	public:
		enum TURNSTATE{ TS_ATTACK,TS_DEFENCE, TS_SKILL, TS_ESCAPE, TS_CHANGESCENE, TS_END};
		enum TRUNACT { ACT_SELECT,ACT_TARGET, ACT_MOVE,ACT_WAIT, ACT_ATTACK, ACT_USE, ACT_END };

	public:
		explicit				Turn(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		explicit				Turn(const Turn& rhs);
		virtual					~Turn() = default;

	public:
		BATTLE				GetWhoTurn() { return m_eTurn; }
		_bool				isDead() { return m_isDead; }
		_bool				isPlaying() { return m_isPlaying; }
		_fvector			GetPosition();
		_fvector			GetLook();

		void				SetPosition(_vector vPos,_vector vLookTarget);
		virtual void		SetPlay(_bool isturn);
		void				SetTurnNum(_int num) { m_iTurnNum = num; }

		void				SeletedSkill(_int iUsingSkill);

		void				StartEscape();
	
	public:
		virtual HRESULT		Ready_Prototype() override;
		virtual HRESULT		Ready(void* pArg) override;

		virtual _int		Update(_double TimeDelta) override;
		virtual _int		LateUpdate(_double TimeDelta) override;

		virtual HRESULT		Render() override;

	public:
		void				ChangePersona();
		void				UsingSkill(_double TimeDelta);
		void				UsingItem();
		void				Attack(_double TimeDelta);
		void				Defence();
		void				Escape(_double Timedelta);

		virtual _bool		Damaged(_float fEnemyStr,SKILLDATA* pSkill);
		void				AddHP();

	private:
		void				SelectTarget();

	protected:
		virtual HRESULT		SetUpComponents();
		HRESULT				SetUpWeapon();

	public:
		static Turn*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		virtual Turn*		Clone(void* pArg) override;
		virtual void		Free() override;

	protected:
		BATTLE				m_eTurn;
		_bool				m_isPlaying;
		_bool				m_PlayAni;		// 애니메이션 시작 
		_bool				m_isDead;

		_vector				m_vOrignPos;	// 원래 위치 
		_float				m_Time;
		
	protected:
		Renderer*			m_pRenderer;
		Model*				m_pModel;
		Transform*			m_pTransform;

		BtlMgr*				m_pBtlMgr;

	private:
		CHARPLAYERDATA		m_Status;
		Persona*			m_pPersona;		// 보유한 페르소나, 주인공은 배열로 

		TURNSTATE			m_eState;
		TRUNACT				m_eCurAct;
		TRUNACT				m_eNextAct;

		_vector				m_vTargetPos;	//공격 대상 

		_int				m_iTurnNum;
		_int				m_iUsingSkill;
	
		Model*				m_pWeapon;
		HierarchyNode*		m_pWeaponNode;
		_float4x4			m_WeaponMat;
	};

}