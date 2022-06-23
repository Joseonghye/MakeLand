#include "stdafx.h"
#include "..\public\Amenosagiri.h"
#include "GameInstance.h"
#include "BtlMgr.h"
#include "DataManager.h"
#include "UIText.h"

Client::Amenosagiri::Amenosagiri(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:Turn(pDevice, pDeviceContext)
{
}

Client::Amenosagiri::Amenosagiri(const Amenosagiri & rhs)
	: Turn(rhs), m_iTurnCnt(0), m_ChkTime(0), m_useFog(false)
{
}

HRESULT Client::Amenosagiri::Ready_Prototype()
{
	if (FAILED(__super::Ready_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT Client::Amenosagiri::Ready(void * pArg)
{
	if (FAILED(SetUpComponents()))
		return E_FAIL;

	m_pModel->SetUpAnimationIndex(AMEANI_IDLE);

	DataManager* pDataMgr = GET_INSTANCE(DataManager);

	pDataMgr->GetEnemyData("Em_01", &m_pEnemyStaus);
	
	//스킬셋팅 0~4 단일스킬 5~10 광역스킬 11~14
	string strsk = "sk_";
	SKILLDATA* skill;
	for (int i = 0; i < 4; i++)
	{
		string strID = strsk + "0" +to_string(i);
		pDataMgr->GetSkillData(strID, &skill);
		m_pSkills.push_back(skill);
		skill = nullptr;
	}
	pDataMgr->GetSkillData("sk_10", &skill);
	m_pSkills.push_back(skill);
	skill = nullptr;

	for (int i = 4; i < 8; i++)
	{
		string strID = strsk + "0" + to_string(i);
		pDataMgr->GetSkillData(strID, &skill);
		m_pSkills.push_back(skill);
		skill = nullptr;
	}

	for (int i = 11; i < 17; i++)
	{
		SKILLDATA* skill;
		string strID = strsk + to_string(i);
		pDataMgr->GetSkillData(strID, &skill);
		m_pSkills.push_back(skill);
	}

	RELEASE_INSTANCE(DataManager);

	//m_pEnemyStaus->iHp = 100;

	m_ePatten = PATTEN_ONE;

	return S_OK;
}

_int Client::Amenosagiri::Update(_double TimeDelta)
{
	//string strHp = m_pEnemyStaus->iHp + " / " + m_pEnemyStaus->iMaxHp;
	
	_tchar szHp[260];
	wsprintf(szHp, TEXT("%d / %d"), m_pEnemyStaus->iHp, m_pEnemyStaus->iMaxHp);
	m_pHpText->SetText(szHp);
	
	if (m_isDead)
	{
		m_Time += TimeDelta;
		if (m_Time >= 3)
		{
			Dead(m_iDeadNum);
		}
	}

	if (!m_isPlaying)
		return _int();

	switch (m_ePatten)
	{
	case Client::Amenosagiri::PATTEN_ONE:
		PattenOne(TimeDelta);
		break;
	case Client::Amenosagiri::PATTEN_TWO:
		PattenTwo(TimeDelta);
		break;
	case Client::Amenosagiri::PATTEN_THREE:
		PattenThree(TimeDelta);
		break;
	case Client::Amenosagiri::PATTEN_FOUR:
		PattenFour(TimeDelta);
		break;
	}

	return _int();
}

_int Client::Amenosagiri::LateUpdate(_double TimeDelta)
{
	if (nullptr != m_pRenderer)
		m_pRenderer->AddRenderGroup(Renderer::RENDER_NONALPHA, this);

	m_pModel->UpdateCombinedTransformationMatrix(TimeDelta);

	if (m_PlayAni)
	{
		if (m_pModel->GetCurrentAnimationEnd())
		{
			m_PlayAni = false;
			//if(!m_useFog)
				m_pModel->SetUpAnimationIndex(AMEANI_IDLE);
		/*	else
				m_pModel->SetUpAnimationIndex(AMEANI_FOGIDLE);*/
		}
	}

	return _int();
}

HRESULT Client::Amenosagiri::Render()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	//_tchar szHp[260];
	//wsprintf(szHp, TEXT("%d / %d"), m_pEnemyStaus->iHp, m_pEnemyStaus->iMaxHp);
	//if (FAILED(pGameInstance->Render_Font(TEXT("Font_Persona"), XMFLOAT2(100, 100), szHp, Colors::Red)))
	//	return E_FAIL;


	m_pModel->SetUpValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransform->GetWorldMatrix()), sizeof(_matrix));
	m_pModel->SetUpValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pGameInstance->GetTransform(PipeLine::D3DTS_VIEW)), sizeof(_matrix));
	m_pModel->SetUpValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pGameInstance->GetTransform(PipeLine::D3DTS_PROJECTION)), sizeof(_matrix));

	if (FAILED(m_pModel->BindBuffers()))
		return E_FAIL;

	for (_uint i = 0; i < m_pModel->GetNumMeshContainer(); ++i)
	{
		m_pModel->SetUpTextureOnShader("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		m_pModel->Render(i, 1);
	}

	RELEASE_INSTANCE(GameInstance);
	return S_OK;
}

void Client::Amenosagiri::SetPlay(_bool isturn)
{
	m_isPlaying = isturn;
	m_eAct = AMEACT_START;
}

_bool Client::Amenosagiri::Damaged(_float fEnemyStr, SKILLDATA * pSkill)
{
	if (m_useFog)
		return false;

	int iDamage = 0;
	if (pSkill == nullptr)
	{
		//데미지 계산
		iDamage = 5 * sqrt(fEnemyStr / m_pEnemyStaus->iDef) + rand() % 30 + 15;
	}
	else {
		//스킬타입확인
		int iType = (6 - (int)pSkill->eSkillType);

		//본인 상성확인
		//무효
		if ((m_pEnemyStaus->iInvalidity & iType) > 0)
			return false;
		//내성
		else if ((m_pEnemyStaus->iTolerance & iType) > 0)
		{

		}
		//약점
		else if ((m_pEnemyStaus->iWeakness & iType) > 0)
		{

		}

		//데미지 계산
		iDamage = 5 * sqrt(fEnemyStr / m_pEnemyStaus->iDef * pSkill->fDamage) * 1 * pSkill->iAttCount;
		//체력계산
	}

	m_pEnemyStaus->iHp -= iDamage;
	if (m_pEnemyStaus->iHp <= 0)
	{
		m_pEnemyStaus->iHp = 0;
		m_pModel->SetUpAnimationIndex((_int)AMEANI_DEATH);
		return true;
	}

	if (m_pEnemyStaus->iHp <= m_pEnemyStaus->iMaxHp * 0.25f)
	{
		if (m_ePatten == PATTEN_FOUR && m_bFirst)
		{
			m_ePatten = PATTEN_THREE;
			m_bFirst = false;
		}
	}
	else if (m_pEnemyStaus->iHp <= m_pEnemyStaus->iMaxHp * 0.5f)
	{
		if (m_ePatten == PATTEN_TWO)
			m_ePatten = PATTEN_THREE;
	}
	else if (m_pEnemyStaus->iHp <= ((_float)m_pEnemyStaus->iMaxHp *0.75f))
	{
		if(m_ePatten == PATTEN_ONE)
			m_ePatten = PATTEN_TWO;
	}

	return false;
}

void Client::Amenosagiri::PattenOne(_double TimeDelta)
{
	switch (m_eAct)
	{
	case Client::Amenosagiri::AMEACT_START:
	{
		//스킬 랜덤 0~4 단일스킬 
		m_iSkillIndex = rand() % 5;
		//타겟 랜덤
		while (true) {
			m_iTarget = rand() % 4;
			if (m_pBtlMgr->TurnIsDead(m_iTarget) == false)
				break;
		}
	//	if (m_iTurnCnt == 0)

			m_PlayAni = true;
			m_pModel->SetUpAnimationIndex(AMEANI_SKILL0);
	//	else
	//		m_pModel->SetUpAnimationIndex(AMEANI_SKILL1);

		m_pBtlMgr->SetStateUI(true, m_pSkills[m_iSkillIndex]->strName);
		//1초정도 좌측 상단
		_vector vPos = XMVectorSetY(m_vOrignPos, 5.f);
		_vector vCamPos = m_vOrignPos - (VEC_RIGHT *5) - (VEC_LOOK *10);
		vCamPos = XMVectorSetY(vCamPos, 7.f);
		_vector vDir = XMVector3Normalize(VEC_RIGHT - VEC_LOOK);

		m_pBtlMgr->MovingCamera(vCamPos, vPos, vDir);

		m_eAct = AMEACT_WAIT;
		m_eNextAct = AMEACT_TARGET;
		m_ChkTime = 4;
	}
	break;
	case Client::Amenosagiri::AMEACT_WAIT:
	{
		m_Time += TimeDelta;
		if (m_Time >= m_ChkTime)
		{
			m_Time = 0;
			m_eAct = m_eNextAct;
		}
	}
	break;
	case Client::Amenosagiri::AMEACT_TARGET:
	{
		m_pBtlMgr->SetStateUI(false);

		_vector vPos = XMVectorSetY(m_vOrignPos, 2.f);

		// 맞는 캐릭터 보기 
		m_pBtlMgr->MovingCamera(m_iTarget, vPos, -VEC_LOOK);
		m_eAct = AMEACT_WAIT;
		m_eNextAct = AMEACT_ATTACK;
		m_ChkTime = 2;
	}
	break;
	case Client::Amenosagiri::AMEACT_ATTACK:
	{
		m_pBtlMgr->EnemyUsingSkill(m_pEnemyStaus->iStr, m_pSkills[m_iSkillIndex], m_iTarget);
		m_eAct = AMEACT_WAIT;
		m_eNextAct = AMEACT_END;
	}
	break;
	case Client::Amenosagiri::AMEACT_END:
	{
		m_iTurnCnt++;

		if (m_iTurnCnt >= 2)
		{
			m_iTurnCnt = 0;
			m_isPlaying = false;
			m_pBtlMgr->NextTurn();
		}
		else
		{
			m_eAct = AMEACT_START;
		}
	}
	break;
	}
}

void Client::Amenosagiri::PattenTwo(_double TimeDelta)
{
	switch (m_eAct)
	{
	case Client::Amenosagiri::AMEACT_START:
	{
		//스킬 랜덤 5~10 광역스킬 
		m_iSkillIndex = rand() % 6 + 5;

		m_PlayAni = true;
		if (m_pSkills[m_iSkillIndex]->strName == "네브라오쿠르스")
		{
			m_pModel->SetUpAnimationIndex(AMEANI_NEBRO);

			_vector vPos = XMVectorSetY(m_vOrignPos, 5.f);
			_vector vCamPos = m_vOrignPos - (m_pTransform->GetState(Transform::STATE_RIGHT) * 7) - (m_pTransform->GetState(Transform::STATE_LOOK) * 10);
			vCamPos = XMVectorSetY(vCamPos, 7.f);
			_vector vDir = XMVector3Normalize(VEC_RIGHT - VEC_LOOK);

			m_pBtlMgr->MovingCamera(vCamPos, vPos, vDir);
			m_ChkTime = 2;
		}
		else
		{
			m_pModel->SetUpAnimationIndex(0);
			m_pBtlMgr->TotalCamera(0);

			_vector vPos = XMVectorSetY(m_vOrignPos, 5.f);
			_vector vCamPos = m_vOrignPos - (VEC_LOOK * 10) + (VEC_UP * 5);
			vCamPos = XMVectorSetY(vCamPos, 7.f);
			_vector vDir = XMVector3Normalize(-VEC_RIGHT);

			m_pBtlMgr->MovingCamera(vCamPos, vPos, vDir);

			m_ChkTime = 3;
		}

		m_pBtlMgr->SetStateUI(true, m_pSkills[m_iSkillIndex]->strName);

		m_eAct = AMEACT_WAIT;
		m_eNextAct = AMEACT_TARGET;
	}
	break;
	case Client::Amenosagiri::AMEACT_WAIT:
	{
		m_Time += TimeDelta;
		if (m_Time >= m_ChkTime)
		{
			m_Time = 0;
			m_eAct = m_eNextAct;
		}
	}
	break;
	case Client::Amenosagiri::AMEACT_TARGET:
	{
		m_pBtlMgr->SetStateUI(false);

		if (m_pSkills[m_iSkillIndex]->strName == "네브라오쿠르스")
		{
			m_pBtlMgr->TotalCamera(0, VEC_RIGHT - VEC_LOOK);
		}
		else
			m_pBtlMgr->TotalCamera(0, VEC_LOOK);

		m_ChkTime = 1;
		m_eAct = AMEACT_WAIT;
		m_eNextAct = AMEACT_ATTACK;
	}
		break;
	case Client::Amenosagiri::AMEACT_ATTACK:
	{
		m_pBtlMgr->EnemyUsingSkill(m_pEnemyStaus->iStr, m_pSkills[m_iSkillIndex], 0);
		
		m_eAct = AMEACT_WAIT;
		m_eNextAct = AMEACT_END;
	}
		break;
	case Client::Amenosagiri::AMEACT_END:
	{
		m_iTurnCnt++;

		if (m_iTurnCnt >= 2)
		{
			m_iTurnCnt = 0;
			m_isPlaying = false;
			m_pBtlMgr->NextTurn();
		}
		else
		{
			m_eAct = AMEACT_START;
		}
	}
		break;
	}
}

void Client::Amenosagiri::PattenThree(_double TimeDelta)
{
	switch (m_eAct)
	{
	case AMEACT_START:
	{
		if (m_iTurnCnt == 0)
		{
			m_useFog = true;
			m_pBtlMgr->SetStateUI(true, "혼미의 안개");
			m_pModel->SetUpAnimationIndex(AMEANI_FOG);
		}
		else if (m_iTurnCnt == 4)
		{
			m_useFog = 0;
			m_iBufCnt++;
		}
		else
		{
			switch (m_iTurnCnt)
			{
			case 1://컨센트레이트
				m_pBtlMgr->SetStateUI(true, "컨센트레이트");
				m_useEffect = true;
				m_pModel->SetUpAnimationIndex(AMEANI_CONCENT);
				break;
			case 2://타루카쟈
				m_pBtlMgr->SetStateUI(true, "타루카쟈");
				m_pEnemyStaus->iDex *= 1.25;
				m_pModel->SetUpAnimationIndex(AMEANI_SKILL0);
				break;
			case 3://스쿠카쟈
				m_pBtlMgr->SetStateUI(true, "스쿠카쟈");
				m_pEnemyStaus->iStr *= 1.25f;
				m_pEnemyStaus->iInt *= 1.25f;
				m_useEffect = true;
				m_iBufCnt++;
				m_pModel->SetUpAnimationIndex(AMEANI_SKILL0);
				break;
			}
		}
		
		m_PlayAni = true;
		_vector vPos = XMVectorSetY(m_vOrignPos, 5.f);
		_vector vCamPos = m_vOrignPos - (VEC_RIGHT * 5) - (VEC_LOOK * 10) + (VEC_UP * 2);
		m_pBtlMgr->MovingCamera(vCamPos, vPos, (VEC_RIGHT + VEC_UP));
		m_ChkTime = 2;

		m_eAct = AMEACT_WAIT;
		m_eNextAct = AMEACT_TARGET;
	}
		break;
	case AMEACT_WAIT:
	{
		m_Time += TimeDelta;
		if (m_Time >= m_ChkTime)
		{
			if (m_iTurnCnt == 0)
				m_pBtlMgr->SetStateUI(true, "혼미의 안개가 발동했다");
			else if(m_iTurnCnt == 1)
				m_pBtlMgr->SetStateUI(true, "힘을 축적했다");
			else if(m_iTurnCnt == 2)
				m_pBtlMgr->SetStateUI(true, "방어력이 상승했다");
			else if (m_iTurnCnt == 3)
				m_pBtlMgr->SetStateUI(true, "공격력이 상승했다");
			else if(m_iTurnCnt == 4)
				m_pBtlMgr->SetStateUI(true, "혼미의 안개가 해제됐다");

			m_Time = 0;
			m_eAct = m_eNextAct;
		}
	}
	break;
	case AMEACT_TARGET:
		if(m_iTurnCnt == 0 || 4)
			m_pBtlMgr->TotalCamera(2, -VEC_RIGHT);
		else
		{
			m_pBtlMgr->TotalCamera(0, VEC_LOOK);
			m_pBtlMgr->SetStateUI(false);
		}
		//버프적용
		m_eAct = AMEACT_WAIT;
		m_eNextAct = AMEACT_ATTACK;
		m_ChkTime = 2;
		break;
	case AMEACT_ATTACK:
	{
		m_pBtlMgr->SetStateUI(false);
		++m_iTurnCnt;
		if (m_iTurnCnt % 2 == 0)
		{
			
			m_isPlaying = false;
			m_pBtlMgr->NextTurn();
		}
		else if (m_iTurnCnt == 5)
		{
			m_iTurnCnt = 0;
			m_ePatten = PATTEN_FOUR;

			m_isPlaying = false;
			m_pBtlMgr->NextTurn();
		}
		else
			m_eAct = AMEACT_START;
	}
	break;
	}
}

void Client::Amenosagiri::PattenFour(_double TimeDelta)
{
	switch (m_eAct)
	{
	case Client::Amenosagiri::AMEACT_START:
	{
		if(m_iBufCnt != 0)
			m_iBufCnt++;
		if (m_iBufCnt == 3)
		{
			//버프해제 
			m_pEnemyStaus->iDex /= 1.25f;
		}
		else if (m_iBufCnt >= 4)
		{
			m_pEnemyStaus->iStr /= 1.25f;
			m_pEnemyStaus->iInt /= 1.25f;
			m_iBufCnt = 0;
		}

		if (m_useEffect)
		{
			m_useEffect = false;

			m_iSkillIndex = 9;

			m_pModel->SetUpAnimationIndex(AMEANI_NEBRO);

			_vector vPos = XMVectorSetY(m_vOrignPos, 5.f);
			_vector vCamPos = m_vOrignPos - (VEC_RIGHT * 7) - (VEC_LOOK * 10);
			vCamPos = XMVectorSetY(vCamPos, 7.f);
			_vector vDir = XMVector3Normalize(VEC_RIGHT - VEC_LOOK);

			m_pBtlMgr->SetStateUI(true, m_pSkills[m_iSkillIndex]->strName);

			m_pBtlMgr->MovingCamera(vCamPos, vPos, vDir);
			m_ChkTime = 2;
		}
		else
		{		//스킬 랜덤 0~10 단일 광역 랜덤 
			m_iSkillIndex = rand() % 11;

			//광역?단일?
			_bool isWide = (m_pSkills[m_iSkillIndex]->iTarget != 4) ? false : true;

			if (!isWide)
			{//단일 스킬
				while (true)
				{
					m_iTarget = rand() % 4;
					if (m_pBtlMgr->TurnIsDead(m_iTarget) == false)
						break;
				}
				_vector vPos = XMVectorSetY(m_vOrignPos, 5.f);
				_vector vCamPos = m_vOrignPos - (VEC_RIGHT * 5) - (VEC_LOOK * 10) + (VEC_UP * 7);
				_vector vDir = XMVector3Normalize(VEC_RIGHT - VEC_LOOK);
				m_pBtlMgr->MovingCamera(vCamPos, vPos, vDir);
			}
			else
			{
				m_pBtlMgr->TotalCamera(0);
			}
			m_pModel->SetUpAnimationIndex(AMEANI_SKILL0);

			m_ChkTime = 4;
		}
		m_pBtlMgr->SetStateUI(true, m_pSkills[m_iSkillIndex]->strName);
		
		m_PlayAni = true;
		m_eAct = AMEACT_WAIT;
		m_eNextAct = AMEACT_TARGET;
	}
	break;
	case Client::Amenosagiri::AMEACT_WAIT:
	{
		m_Time += TimeDelta;
		if (m_Time >= m_ChkTime)
		{
			m_Time = 0;
			m_eAct = m_eNextAct;
		}
	}
	break;
	case Client::Amenosagiri::AMEACT_TARGET:
	{
		m_pBtlMgr->SetStateUI(false);

		if (m_pSkills[m_iSkillIndex]->strName == "네브라오쿠르스")
		{
			m_pBtlMgr->TotalCamera(0, VEC_RIGHT - VEC_LOOK);
			m_pBtlMgr->EnemyUsingSkill(m_pEnemyStaus->iStr, m_pSkills[m_iSkillIndex], 0);
			m_eAct = AMEACT_WAIT;
			m_eNextAct = AMEACT_END;
		}
		else if (m_pSkills[m_iSkillIndex]->iTarget == 4)
		{//광역기
			m_pBtlMgr->EnemyUsingSkill(m_pEnemyStaus->iStr, m_pSkills[m_iSkillIndex], 0);
			m_ChkTime = 3;
			m_eAct = AMEACT_WAIT;
			m_eNextAct = AMEACT_END;
		}
		else 
		{
			_vector vPos = XMVectorSetY(m_vOrignPos, 2.f);

			// 맞는 캐릭터 보기 
			m_pBtlMgr->MovingCamera(m_iTarget, vPos, -VEC_LOOK);
			m_eAct = AMEACT_WAIT;
			m_eNextAct = AMEACT_ATTACK;
			m_ChkTime = 2;
		}
		m_pBtlMgr->SetStateUI(false);
	}
	break;
	case Client::Amenosagiri::AMEACT_ATTACK:
	{
		m_pBtlMgr->EnemyUsingSkill(m_pEnemyStaus->iStr, m_pSkills[m_iSkillIndex], m_iTarget);
		m_eAct = AMEACT_WAIT;
		m_eNextAct = AMEACT_END;
	}
	break;
	case Client::Amenosagiri::AMEACT_END:
	{
		m_iTurnCnt++;

		if (m_iTurnCnt >= 2)
		{
			m_iTurnCnt = 0;
			m_isPlaying = false;
			m_pBtlMgr->NextTurn();
		}
		else
		{
			m_eAct = AMEACT_START;
		}
	}
	break;
	}
}

void Client::Amenosagiri::Dead(_int iNum)
{
	if (iNum==0)
	{	//카메라 아래 셋팅 
		m_Time = 0;
		m_isDead = true;
		m_pBtlMgr->EnemyDead();
		_vector vCamPos = m_vOrignPos - (VEC_LOOK * 10) + VEC_UP;
		_vector vTarget = m_vOrignPos + (VEC_UP * 10);
		m_pBtlMgr->MovingCamera(vCamPos, vTarget, (-VEC_LOOK + (VEC_UP*0.5f)));
		m_iDeadNum++;
	}
	else if (iNum == 1)
	{
		GameInstance* pGameInstance = GET_INSTANCE(GameInstance);
		pGameInstance->OnSuspending();
		RELEASE_INSTANCE(GameInstance);

		m_Time = 0;
		_vector vCamPos = m_vOrignPos - (VEC_RIGHT * 10) - (VEC_LOOK * 15) + (VEC_UP * 10);
		_vector vTarget = m_vOrignPos + (VEC_UP * 3);
		m_pBtlMgr->MovingCamera(vCamPos, vTarget, (-VEC_LOOK - VEC_UP + VEC_RIGHT));
		m_iDeadNum++;
	}
	else
	{
		m_Time = 0;
		m_pBtlMgr->End();
		//m_pBtlMgr->ReturnGameScene();
	}
}

HRESULT Client::Amenosagiri::SetUpComponents()
{
	Transform::TRANSFORMDESC transDesc;
	transDesc.fSpeedPerSec = 7.f;
	transDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(GameObject::SetUpComponents(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (Component**)&m_pTransform, &transDesc)))
		return E_FAIL;

	if (FAILED(GameObject::SetUpComponents(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (Component**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(GameObject::SetUpComponents(SCENE_BATTLE, TEXT("Prototype_Component_Model_Amenosagiri"), TEXT("Com_Model"), (Component**)&m_pModel)))
		return E_FAIL;

	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);
	GameObject* pObj;
	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_BATTLE, TEXT("Layer_StateUI"), TEXT("Prototype_GameObject_Text"), &_float2(100,100), &pObj)))
		return E_FAIL;
	m_pHpText = static_cast<UIText*>(pObj);
	m_pHpText->SetColor(Colors::Red);
	RELEASE_INSTANCE(GameInstance);

	m_eTurn = BATTLE_ENEMY;

	return S_OK;
}

Amenosagiri * Client::Amenosagiri::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	Amenosagiri*	pInstance = new Amenosagiri(pDevice, pDeviceContext);
	if (FAILED(pInstance->Ready_Prototype()))
	{
		MSGBOX("Failed to Creating Amenosagiri");
		Safe_Release(pInstance);
	}
	return pInstance;
}

Amenosagiri * Client::Amenosagiri::Clone(void* pArg)
{
	Amenosagiri* pInstance = new Amenosagiri(*this);

	if (FAILED(pInstance->Ready(pArg)))
	{
		MSGBOX("Failed to Creating Amenosagiri Clone");
		Safe_Release(pInstance);
	}
	return pInstance;
}
