#include "stdafx.h"
#include "..\public\BtlMgr.h"
#include "DataManager.h"
#include "GameInstance.h"
#include "Turn.h"
#include "Amenosagiri.h"
#include "Persona.h"
#include "BattleCamera.h"
#include "UIButton.h"
#include "SkillBtn.h"
#include "BtlStateUI.h"
#include "RePlayPanel.h"

#include "SceneGamePlay.h"
#include "SceneLogo.h"
#include "SceneBattle.h"

#include "UIInGame.h"

IMPLEMENT_SINGLETON(BtlMgr)

Client::BtlMgr::BtlMgr()
	:m_VisibleBtn(false), m_VisibleSelect(false), m_bPageDown(false)
{

}

HRESULT Client::BtlMgr::ReadyBattle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, string strBattleType)
{
	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	// 위치 데이터 로딩 
	DataManager* DataMgr = GET_INSTANCE(DataManager);

	DataMgr->GetBattlePosData(strBattleType, m_vPositions);
	if (m_vPositions == nullptr)
		return E_FAIL;

	RELEASE_INSTANCE(DataManager);

	//전투 시작 상태 셋팅 
	m_eCurBattleState = BATTLE_END;
	m_iCurTurn = 0;
	m_isSelect = false;

	m_ChoiceTarget = false;
	m_iTargetNum = 0;
	m_iBtnIndex = 0;
	
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	//UI 셋팅 -------------------------------------------
	list<GameObject*> pUIs = pGameInstance->GetGameObjectsToLayer(SCENE_BATTLE, TEXT("Layer_UI"));
	if (pUIs.size() == 0)
		return E_FAIL;

	for (auto& pUI : pUIs)
	{
	if (pUI->GetName() != "UIButton")
	{

		m_pBtnBack = pUI;
		Safe_AddRef(m_pBtnBack);
		m_pBtnBack->SetActive(false);

		continue;
	}

	pUI->SetActive(false);
	m_pButtons.push_back(static_cast<UIButton*>(pUI));

	Safe_AddRef(pUI);
	}

	//Select UI 셋팅 -------------------------------------------
	list<GameObject*> pSelectUIs = pGameInstance->GetGameObjectsToLayer(SCENE_BATTLE, TEXT("Layer_SelectUI"));
	if (pSelectUIs.size() == 0)
		return E_FAIL;

	m_pSelectPanel = pSelectUIs.front();
	m_pSelectPanel->SetActive(false);
	Safe_AddRef(m_pSelectPanel);
	pSelectUIs.clear();

	m_pSkillButtons.reserve(5);
	for (_int i = 0; i < 5; i++)
	{
		SkillBtn* Btn = SkillBtn::Create(700, 220 + (66 * i));
		m_pSkillButtons.push_back(Btn);
	}

	m_pBtlStateUI = BtlStateUI::Create(640, 100);
	m_pReplayPanel = RePlayPanel::Create();

	//Turn 셋팅(플레이어)-------------------------------
	list<GameObject*> pTurns = pGameInstance->GetGameObjectsToLayer(SCENE_BATTLE, TEXT("Layer_Player"));
	if (pTurns.size() == 0)
		return E_FAIL;

	m_pPlayerTurns.reserve(pTurns.size());
	_int index = 0;
	for (auto& pTurn : pTurns)
	{
		m_pPlayerTurns.emplace_back(static_cast<Turn*>(pTurn));
		Safe_AddRef(m_pPlayerTurns[index]);

		//위치 셋팅 
		m_pPlayerTurns[index]->SetPosition(XMLoadFloat4(&m_vPositions[index]), XMLoadFloat4(&m_vPositions[4]));

		m_pPlayerTurns[index]->SetTurnNum(index);

		index++;
	}
	pTurns.clear();

	//적 셋팅 ----------------------------------------------------------
	list<GameObject*> pEnemys = pGameInstance->GetGameObjectsToLayer(SCENE_BATTLE, TEXT("Layer_Enemy"));
	if (pEnemys.size() == 0)
		return E_FAIL;

	index = 0;
	for (auto& pEnemy : pEnemys)
	{
		m_pEnemyTurns.emplace_back(static_cast<Turn*>(pEnemy));
		m_pEnemyTurns[index]->SetPosition(XMLoadFloat4(&m_vPositions[4]), XMVectorSet(0, 0, -1, 0));
		index++;
	}

	//카메라 셋팅
	list<GameObject*> pCamera = pGameInstance->GetGameObjectsToLayer(SCENE_BATTLE, TEXT("Layer_Camera"));
	m_pCamera = static_cast<BattleCamera*>(pCamera.front());
	Safe_AddRef(m_pCamera);

	//순서 (속도 정렬) --임의
	for (int i = 0; i < 5; i++)
	{
		m_PlayTurn.push_back(i);
	}

	//전투시작
	m_eNextBattleState = BATTLE_START;

	RELEASE_INSTANCE(GameInstance);
	return S_OK;

}

_int Client::BtlMgr::Update(_double TimeDelta)
{
	if (m_bWait)
	{
		m_Time += TimeDelta;
		if (m_Time >= 2.5f)
		{
			m_Time = 0;
			m_bWait = false;
			m_iCameraCnt++;
		}
	}

	if (m_isEnemyDead)
		return _int(0);

	if (m_isPlayerDead)
	{
		GameInstance* pGameInstance = GET_INSTANCE(GameInstance);
		if (pGameInstance->GetKeyDown('A'))
		{
			m_iBtnIndex--;
			if (m_iBtnIndex < 0)
				m_iBtnIndex = 1;
			m_pReplayPanel->Replay(m_iBtnIndex);
		}
		else if (pGameInstance->GetKeyDown('D'))
		{
			m_iBtnIndex++;
			if (m_iBtnIndex > 1)
				m_iBtnIndex = 0;
			m_pReplayPanel->Replay(m_iBtnIndex);
		}
		else if (pGameInstance->GetKeyDown(VK_SPACE))
		{
			if (m_iBtnIndex == 0)
			{
				if (FAILED(pGameInstance->OpenScene(SCENE_BATTLE, 0, SceneBattle::Create(m_pDevice, m_pDeviceContext))))
				{
					MSGBOX("RePlay Error");
					return -1;
				}
			}
			else
			{
				ReturnGameScene();
			}
		}

	
		RELEASE_INSTANCE(GameInstance);
		return _int();
	}

	if (m_eCurBattleState != m_eNextBattleState)
		m_eCurBattleState = m_eNextBattleState;

	if (!m_isSelect)
	{
		BattleTurn();
	}
	else if(m_VisibleBtn)
	{
		if (m_eCurBattleState != BATTLE_PLAYER)
			return _int();

		SelectAction();
	}
	else if (m_VisibleSelect)
	{
		if (m_eCurBattleState != BATTLE_PLAYER)
			return _int();

		SelectSkill();
	}

	return _int();
}

_int Client::BtlMgr::LateUpdate(_double TimeDelta)
{
	return _int();
}

_bool Client::BtlMgr::TurnIsDead(_int iIndex)
{
	return m_pPlayerTurns[iIndex]->isDead();
}

void Client::BtlMgr::NextTurn()
{
	m_isSelect = false;
	m_iCurTurn++;

	//카메라 이동 멈춤 
	m_pCamera->MovingStop();

	if (m_iCurTurn >= (m_pPlayerTurns.size() + m_pEnemyTurns.size()))
	{
		m_iCurTurn = 0;
		//순서 재지정
		//
	//	m_eNextBattleState = BATTLE_PLAYER;
	//	return;
	}
	
	if (m_PlayTurn[m_iCurTurn] >= m_pPlayerTurns.size())
	{
		m_eNextBattleState = BATTLE_ENEMY;
	}
	else
	{
		if (m_pPlayerTurns[m_PlayTurn[m_iCurTurn]]->isDead())
		{
			NextTurn();
			return;
		}

		m_eNextBattleState = BATTLE_PLAYER;
	
	}
}

void Client::BtlMgr::MovingCamera(_fvector vPos, _fvector vTarget, _fvector vDir)
{
	_vector vTargetPos = vTarget;
	if (XMVectorGetW(vTarget) >= 4)
	{
		int index = XMVectorGetW(vTarget) - 4;
		vTargetPos = m_pEnemyTurns[index]->GetPosition() + (VEC_UP * 3);
	}
	m_pCamera->Moving(vPos, vTargetPos, vDir);
}

void Client::BtlMgr::MovingCamera(_int vTargetIndex, _fvector vTarget, _fvector vDir)
{
	_vector vPos;
	if (vTargetIndex >= 4)
	{
		vTargetIndex -=4;
		vPos = m_pEnemyTurns[vTargetIndex]->GetPosition() - (VEC_LOOK * 3) + (VEC_UP * 10);
	}
	else
		 vPos = XMLoadFloat4(&m_vPositions[vTargetIndex]) - (VEC_LOOK * 3) + VEC_UP;

	_vector vTargetPos = vTarget;
	if (XMVectorGetW(vTarget) == 0)
	{
		vTargetPos = m_pEnemyTurns[vTargetIndex]->GetPosition() + (VEC_UP * 3);
	}

	m_pCamera->Moving(vPos, vTargetPos, vDir);
}

void Client::BtlMgr::TotalCamera(_int iNum, _fvector vDir)
{
	_vector vPos, vTarget;

	if (iNum == 0) 
	{//가운데 상단
		vPos = XMLoadFloat4(&m_vPositions[4]) -(VEC_LOOK * 18) + (VEC_UP * 5);
		vTarget = XMLoadFloat4(&m_vPositions[4]) + (VEC_UP * 2);
	}
	else if(iNum == 1)
	{//좌측하단
		vPos = XMLoadFloat4(&m_vPositions[4]) -(VEC_LOOK * 18) + VEC_UP - (VEC_RIGHT * 10);
		vTarget = XMLoadFloat4(&m_vPositions[4]) + (VEC_UP * 3);
	}
	else
	{	//우측 상단 
		vPos = XMLoadFloat4(&m_vPositions[4]) -(VEC_LOOK * 18) + (VEC_UP*5) + (VEC_RIGHT * 10);
		vTarget = XMLoadFloat4(&m_vPositions[4]) + (VEC_UP * 3);
	}
	m_pCamera->Moving(vPos, vTarget, vDir);
}

void Client::BtlMgr::LookTurn()
{
	//플레이어를 바라보는 카메라 
	m_pCamera->SetCamera(m_pPlayerTurns[m_PlayTurn[m_iCurTurn]]->GetPosition(), -m_pPlayerTurns[m_PlayTurn[m_iCurTurn]]->GetLook(),3);
}

void Client::BtlMgr::StartSelectTarget()
{
	m_ChoiceTarget = true;
	m_iTargetNum = 0;

	//카메라 시선
	m_pCamera->LookAt(m_pEnemyTurns[m_iTargetNum]->GetPosition());

	//Ui
	//	m_pTurns[m_EnemyNums[m_iTargetNum]]->GetPosition();
}

void Client::BtlMgr::ChangeTarget()
{
	++m_iTargetNum;
	if (m_iTargetNum >= m_pEnemyTurns.size())
		m_iTargetNum = 0;

	//카메라 시선 
	m_pCamera->LookAt(m_pEnemyTurns[m_iTargetNum]->GetPosition());
	//UI
	//m_pTurns[m_EnemyNums[m_iTargetNum]]->GetPosition();
}

_fvector Client::BtlMgr::SelectedTarget()
{
	_vector Target = m_pEnemyTurns[m_iTargetNum]->GetPosition();
	Target = XMVectorSetW(Target, m_iTargetNum);
	return Target;
}

void Client::BtlMgr::Escape()
{
	//플레이어 도망 
	for (auto& pTurn : m_pPlayerTurns)
	{
		if (BATTLE_PLAYER == pTurn->GetWhoTurn())
		{
			pTurn->StartEscape();
		}
	}

	//카메라 셋팅 

}

void Client::BtlMgr::PlayerDead()
{
	m_isPlayerDead = true;
	m_iBtnIndex = 0;
	m_pReplayPanel->RePlayActive();

	for (int i = 0; i < m_pPlayerTurns.size(); i++)
	{
		m_pPlayerTurns[i]->SetPlay(false);
	}
	m_pEnemyTurns[0]->SetPlay(false);

	SetVisibleBtnUI(false);
	SetStateUI(false);
	SetSkillSelect(false);

	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);
	pGameInstance->OnSuspending();
	RELEASE_INSTANCE(GameInstance);
}

void Client::BtlMgr::EnemyDead()
{
	m_isEnemyDead = true;
	for (int i = 0; i < m_pPlayerTurns.size(); i++)
	{
		m_pPlayerTurns[i]->SetPlay(false);
	}
	m_pEnemyTurns[0]->SetPlay(false);

	SetVisibleBtnUI(false);
	SetStateUI(false);
	SetSkillSelect(false);
}

void Client::BtlMgr::ReturnGameScene()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);
	if (FAILED(pGameInstance->OpenScene(SCENE_LOGO, 0, SceneLogo::Create(m_pDevice, m_pDeviceContext))))
	{
		MSGBOX("RunAway Error");
	}

	RELEASE_INSTANCE(GameInstance);
}

void Client::BtlMgr::End()
{
	m_iBtnIndex = 0;
	m_pReplayPanel->RePlayActive();
}

void Client::BtlMgr::EnemyUsingSkill(_float fEnemyStr,SKILLDATA* pSkill, int iIndex)
{
	//전체공격
	if (pSkill->iTarget == 4)
	{
		for (int i = 0; i < m_pPlayerTurns.size(); ++i)
		{
			if (m_pPlayerTurns[i]->isDead())
				continue;
			_vector vPos = XMLoadFloat4(&m_vPositions[i]) + VEC_UP - (VEC_LOOK);
			CreateSkillEffect(pSkill->eSkillType, vPos);
			m_pPlayerTurns[i]->Damaged(fEnemyStr, pSkill);
		}
	}
	else
	{
		if (m_pPlayerTurns[iIndex]->isDead())
			return;

		_vector vPos = XMLoadFloat4(&m_vPositions[iIndex]) + VEC_UP - (VEC_LOOK);
		CreateSkillEffect(pSkill->eSkillType, vPos);
		m_pPlayerTurns[iIndex]->Damaged(fEnemyStr, pSkill);
		
	}

}

void Client::BtlMgr::TurnUsingSkill(_float fTurnStr, SKILLDATA * pSkill)
{
	if (pSkill == nullptr)
	{
		if (static_cast<Amenosagiri*>(m_pEnemyTurns[m_iTargetNum])->Damaged(fTurnStr))
		{
			//적 죽음
			static_cast<Amenosagiri*>(m_pEnemyTurns[m_iTargetNum])->Dead(0);
		}
		return;
	}

	if (pSkill->eSkillType == SKILL_UP)
	{
		for (int i = 0; i < 4; i++)
		{
			if (m_pPlayerTurns[i]->isDead())
				continue;
			m_pPlayerTurns[i]->AddHP();
		}
	}

	_vector vPos = XMLoadFloat4(&m_vPositions[4]) + (VEC_UP*3) - (VEC_LOOK *8);
	CreateSkillEffect(pSkill->eSkillType, vPos);
	if (static_cast<Amenosagiri*>(m_pEnemyTurns[m_iTargetNum])->Damaged(fTurnStr, pSkill))
	{
		//적 죽음
		//적 죽음
		static_cast<Amenosagiri*>(m_pEnemyTurns[m_iTargetNum])->Dead(0);
	}

	
}

void Client::BtlMgr::SetSkillSelect(_bool visible, _int iStartIndex,vector<SKILLDATA> skills)
{
	if (!visible)
	{
		m_iBtnIndex = 0;
		m_TurnSkills.clear();

		m_VisibleSelect = false;
		m_bPageDown = false;

		m_pSelectPanel->SetActive(false);
		for (int i = 0; i < 5; i++)
		{
			m_pSkillButtons[i]->SetButton(false);
		}
			//m_pSelectButtons[i]->SetActive(false);

		return;
	}

	if(skills.size() != 0)
		m_TurnSkills = skills;

	int iSkillCnt = m_TurnSkills.size() - iStartIndex;

	m_VisibleSelect = true;
	m_pSelectPanel->SetActive(true);
	if (iSkillCnt < 5)
	{
		for (int i = 0; i < 5; i++)
		{
			if (i >= iSkillCnt) 
			{
				m_pSkillButtons[i]->SetButton(false);
			//	m_pSelectButtons[i]->SetActive(false);
				continue;
			}

			SKILLDATA skill = m_TurnSkills[(iStartIndex + i)];
			m_pSkillButtons[i]->SetButton(true, skill.eSkillType, skill.strName, skill.iCost);
			//m_pSelectButtons[i]->SetActive(true);
		}
	}
	else
	{
		for (int i = 0; i < 5; i++)
		{
			SKILLDATA skill = m_TurnSkills[i];
			m_pSkillButtons[i]->SetButton(true, skill.eSkillType, skill.strName, skill.iCost);
		//	m_pSelectButtons[i]->SetActive(true);
			//아이콘
			//이름
			//HP SP 
		}
	}

	m_pSkillButtons[0]->UseButton(true);
}

void Client::BtlMgr::SetStateUI(_bool visible, string strText)
{
	m_pBtlStateUI->SetVisible(visible);
	if (strText != "")
	{
		m_pBtlStateUI->SetText(strText);
	}
}

void Client::BtlMgr::CreateSkillEffect(SKILLTYPE eSkillType, _vector vPos)
{
	UIInGame::PosData pos;
	 XMStoreFloat4(&pos.vPos,vPos);

	 GameInstance* pGameInstance = GET_INSTANCE(GameInstance);
	switch (eSkillType)
	{
	case SKILL_PHY:
		break;
	case SKILL_FIRE:
		pGameInstance->AddGameObjectToLayer(SCENE_BATTLE, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_AgidyneEffect"), &pos);
		break;
	case SKILL_ICE:
	//	 _vector Vec = XMLoadFloat4(&m_vPositions[4]) + (VEC_UP * 3) + (VEC_LOOK*5);
	//	XMStoreFloat4(&pos.vPos, Vec);
		pGameInstance->AddGameObjectToLayer(SCENE_BATTLE, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_BufudyneEffect"), &pos);
		break;
	case SKILL_ELECTRIC:
		pGameInstance->AddGameObjectToLayer(SCENE_BATTLE, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_ZiodyneEffect"), &pos);
		break;
	case SKILL_WIND:
		pGameInstance->AddGameObjectToLayer(SCENE_BATTLE, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_GaldyneEffect"), &pos);
		break;
	case SKILL_LIGHT:
		break;
	case SKILL_DARK:
		break;
	case SKILL_UP:
		break;
	case SKILL_DOWN:
		break;
	}

	RELEASE_INSTANCE(GameInstance);

}

void Client::BtlMgr::SetVisibleBtnUI(_bool visible)
{
	if (m_VisibleBtn == visible) return;

	m_VisibleBtn = visible;
	m_iBtnIndex = 0;
	m_pBtnBack->SetActive(visible);
//	m_pBattleFrameImg->SetActive(visible);
	for (auto& pUI : m_pButtons)
	{
		pUI->SetBtnState(UIButton::BTN_NORMAL);
		pUI->SetActive(visible);
	}
}

void Client::BtlMgr::BattleTurn()
{
	switch (m_eCurBattleState)
	{
	case BATTLE_START:
	{
		//카메라: 상황보여주기 
		if (!m_bStart)
		{
			if (m_bWait)
				break;
			if (m_iCameraCnt == 0)
			{
				m_bWait = true;
				//1초정도 좌측 상단
				TotalCamera(0, VEC_LOOK);
			}
			if (m_iCameraCnt == 1) 
			{
				m_bWait = true;
				//1초정도 좌측 상단
				_vector vPos = XMLoadFloat4(&m_vPositions[4]);
				_vector vCamPos = vPos - (VEC_RIGHT * 5) - (VEC_LOOK * 10);
				vCamPos = XMVectorSetY(vCamPos, 7.f);
				vPos = XMVectorSetY(vPos, 5.f);
				_vector vDir = XMVector3Normalize(VEC_RIGHT - VEC_LOOK);
				MovingCamera(vCamPos, vPos, vDir);
			}
			else if (m_iCameraCnt == 2)
			{
				//우측하단 
				m_bWait = true;
				_vector vPos = XMLoadFloat4(&m_vPositions[4]) - (VEC_LOOK * 13) + VEC_UP + (VEC_RIGHT * 8);
				_vector vTarget = XMLoadFloat4(&m_vPositions[4]) + (VEC_UP * 3);

				MovingCamera(vPos, vTarget, -VEC_RIGHT+ VEC_LOOK);
			}
			else
			{
				m_bStart = true;
				m_pCamera->MovingStop();
			}
		}
		else {
			//누구의 턴?
			if (m_PlayTurn[m_iCurTurn] < m_pPlayerTurns.size())
				m_eNextBattleState = BATTLE_PLAYER;
			else m_eNextBattleState = BATTLE_ENEMY;
		}
		break;
	}
	case BATTLE_PLAYER:
	{
		m_isSelect = true;

		//UI 보이기
		SetVisibleBtnUI(true);
		//카메라 셋팅
		//_vector	vLook = XMLoadFloat4(&m_vPositions[4]) - m_pPlayerTurns[m_PlayTurn[m_iCurTurn]]->GetPosition();
		//vLook = XMVectorSetY(vLook, 0);
		//m_pCamera->SetCamera(m_pPlayerTurns[m_PlayTurn[m_iCurTurn]]->GetPosition(), vLook);

		_vector vPos = XMLoadFloat4(&m_vPositions[m_iCurTurn]) - (VEC_LOOK * 3) - (VEC_RIGHT * 1) + VEC_UP;
		_vector vLook = XMLoadFloat4(&m_vPositions[4]);
		vLook = XMVectorSetY(vLook, 3);

		m_pCamera->SetCamera(vPos, vLook);
		//플레이어 셋팅
		m_pPlayerTurns[m_PlayTurn[m_iCurTurn]]->SetPlay(true);
	}
	break;
	case BATTLE_ENEMY:
	{
		m_isSelect = true;

		SetVisibleBtnUI(false);
		int index = m_PlayTurn[m_iCurTurn] - m_pPlayerTurns.size();
		m_pEnemyTurns[index]->SetPlay(true);
		break;
	}
	case BATTLE_WIN:
		break;
	case BATTLE_LOSE:
		break;
	}
}
void Client::BtlMgr::SelectAction()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);
	//UI선택지 선택 
	if (pGameInstance->GetKeyDown('W'))
	{
		m_pButtons[m_iBtnIndex]->SetBtnState(UIButton::BTNSTATE::BTN_NORMAL);
		m_iBtnIndex--;
		if (m_iBtnIndex < 0)
			m_iBtnIndex = m_pButtons.size() - 1;

		m_pButtons[m_iBtnIndex]->SetBtnState(UIButton::BTNSTATE::BTN_OVER);
	}
	else if (pGameInstance->GetKeyDown('S'))
	{
		m_pButtons[m_iBtnIndex]->SetBtnState(UIButton::BTNSTATE::BTN_NORMAL);
		m_iBtnIndex++;
		if (m_iBtnIndex >= m_pButtons.size())
			m_iBtnIndex = 0;
		m_pButtons[m_iBtnIndex]->SetBtnState(UIButton::BTNSTATE::BTN_OVER);
	}
	else if (pGameInstance->GetKeyDown(VK_SPACE))
	{
		//작동
		switch (m_iBtnIndex)
		{
		case Client::BtlMgr::BN_ANALYSIS:
			//m_pTurns[m_iCurTurn]
			break;
		case Client::BtlMgr::BN_TATCTICS:
			break;
		case Client::BtlMgr::BN_GUARD:
			m_pPlayerTurns[m_PlayTurn[m_iCurTurn]]->Defence();
			break;
		case Client::BtlMgr::BN_ATTACK:
			m_pPlayerTurns[m_PlayTurn[m_iCurTurn]]->Attack(0);
			break;
		case Client::BtlMgr::BN_SKILL:
			m_pPlayerTurns[m_PlayTurn[m_iCurTurn]]->UsingSkill(0);
			break;
		case Client::BtlMgr::BN_PERSONA:
			break;
		case Client::BtlMgr::BN_ITEM:
			break;
		case Client::BtlMgr::BN_ESCAPE:
			Escape();
			//m_pTurns[m_iCurTurn]->Escape(TimeDelta);
			break;
		}
		SetVisibleBtnUI(false);
	}
	RELEASE_INSTANCE(GameInstance);
}

void Client::BtlMgr::SelectSkill()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);
	//UI선택지 선택 
	if (pGameInstance->GetKeyDown('W'))
	{
		int preIndex = m_iBtnIndex;
		m_iBtnIndex--;
		if (m_iBtnIndex < 0)
		{
			m_iBtnIndex = 0;
			if (m_bPageDown)
			{
				SetSkillSelect(true);
				m_bPageDown = false;
			}
		}
		if (preIndex != m_iBtnIndex)
		{
			m_pSkillButtons[preIndex]->UseButton(false);
			m_pSkillButtons[m_iBtnIndex]->UseButton(true);
		}
	}
	else if (pGameInstance->GetKeyDown('S'))
	{
		int preIndex = m_iBtnIndex;
		m_iBtnIndex++;
		if (m_iBtnIndex >= 5)
		{
			if (m_TurnSkills.size() >= 5)
			{
				m_iBtnIndex = 0;
				SetSkillSelect(true, 5);
				m_bPageDown = true;
			}
			else m_iBtnIndex = 5;
		}
		else if (m_iBtnIndex >= m_TurnSkills.size())
		{
			m_iBtnIndex = m_TurnSkills.size() - 1;
		}
		if (preIndex != m_iBtnIndex)
		{
			m_pSkillButtons[preIndex]->UseButton(false);
			m_pSkillButtons[m_iBtnIndex]->UseButton(true);
		}
	}
	else if (pGameInstance->GetKeyDown(VK_SPACE))
	{
		if (m_bPageDown)
			m_iBtnIndex += 5;

		m_pPlayerTurns[m_PlayTurn[m_iCurTurn]]->SeletedSkill(m_iBtnIndex);
		//작동
		SetSkillSelect(false);
	}
	RELEASE_INSTANCE(GameInstance);

}

void Client::BtlMgr::Free()
{
	for (auto& pTurn : m_pPlayerTurns)
		Safe_Release(pTurn);
	m_pPlayerTurns.clear();

	m_pEnemyTurns.clear();

	for (auto& pBtn : m_pButtons)
		Safe_Release(pBtn);
	m_pButtons.clear();

	Safe_Release(m_pCamera);

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
