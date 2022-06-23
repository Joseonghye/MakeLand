#include "stdafx.h"
#include "..\public\Turn.h"
#include "GameInstance.h"
#include "Persona.h"
#include "HierarchyNode.h"
#include "BtlMgr.h"
#include "PlayMgr.h"
#include "DataManager.h"

Client::Turn::Turn(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:GameObject(pDevice, pDeviceContext),
	m_pBtlMgr(BtlMgr::GetInstance())
{
	Safe_AddRef(m_pBtlMgr);
}

Client::Turn::Turn(const Turn & rhs)
	: GameObject(rhs), m_pWeapon(nullptr), m_Time(0), 
	m_isPlaying(false), m_PlayAni(false), m_isDead(false),
	m_pBtlMgr(BtlMgr::GetInstance())
{
	Safe_AddRef(m_pBtlMgr);
}

HRESULT Client::Turn::Ready_Prototype()
{
	if (FAILED(__super::Ready_Prototype()))
		return E_FAIL;

	m_eState = TS_END;

	m_eCurAct = ACT_END;
	m_eNextAct = ACT_END;

	return S_OK;
}

HRESULT Client::Turn::Ready(void * pArg)
{
	if (FAILED(__super::Ready(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		m_Status = *(CHARPLAYERDATA*)pArg;
		if (FAILED(SetUpComponents()))
			return E_FAIL;
	}

	m_eTurn = BATTLE_PLAYER;
	m_pModel->SetUpAnimationIndex((int)P_IDLE);

	return S_OK;
}

_int Client::Turn::Update(_double TimeDelta)
{
	if (!m_isPlaying)
	{
		if(m_eState != TS_ESCAPE)
			return _int();
	}

	switch (m_eState)
	{
	case Client::Turn::TS_ATTACK:
		Attack(TimeDelta);
		break;
	case Client::Turn::TS_DEFENCE:
		Defence();
		break;
	case Client::Turn::TS_SKILL:
		UsingSkill(TimeDelta);
		break;
	case Client::Turn::TS_ESCAPE:
		Escape(TimeDelta);
		break;
	}
	if (m_eState == TS_CHANGESCENE)
		return 1;

//	if (GetKeyState('E') & 0x8000)
//		UsingItem();

	return _int();
}

_int Client::Turn::LateUpdate(_double TimeDelta)
{
	if (nullptr != m_pRenderer)
		m_pRenderer->AddRenderGroup(Renderer::RENDER_NONALPHA, this);

	m_pModel->UpdateCombinedTransformationMatrix(TimeDelta);

	if (m_isDead&& m_PlayAni && m_pModel->GetCurrentAnimationEnd())
	{
		m_PlayAni = false;
		m_pModel->SetUpAnimationIndex((_int)P_DEAD);
	}

	if (m_PlayAni && m_pModel->GetCurrentAnimationEnd())
	{
		m_PlayAni = false;
		m_pModel->SetUpAnimationIndex((_int)P_IDLE);
	}

	if(m_pWeapon != nullptr)
		SetUpWeapon();

	return _int();
}

HRESULT Client::Turn::Render()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

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

	if (m_pWeapon != nullptr) 
	{
		m_pWeapon->SetUpValueOnShader("g_WorldMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_WeaponMat)), sizeof(_matrix));
		m_pWeapon->SetUpValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pGameInstance->GetTransform(PipeLine::D3DTS_VIEW)), sizeof(_matrix));
		m_pWeapon->SetUpValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pGameInstance->GetTransform(PipeLine::D3DTS_PROJECTION)), sizeof(_matrix));

		if (FAILED(m_pWeapon->BindBuffers()))
			return E_FAIL;

		for (_uint i = 0; i < m_pWeapon->GetNumMeshContainer(); ++i)
		{
			m_pWeapon->SetUpTextureOnShader("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
			m_pWeapon->Render(i, 0);
		}
	}

	RELEASE_INSTANCE(GameInstance);
	return S_OK;
}

_fvector Client::Turn::GetPosition()
{
	return m_pTransform->GetState(Transform::STATE_POSITION);
}

_fvector Client::Turn::GetLook()
{
	return m_pTransform->GetState(Transform::STATE_LOOK);
}

void Client::Turn::SetPosition(_vector vPos,_vector vLookTarget)
{
	m_vOrignPos = vPos;
	m_pTransform->SetState(Transform::STATE_POSITION, vPos);

	if(m_eTurn == BATTLE_PLAYER)
		m_pTransform->LookAt(vLookTarget);
}

void Client::Turn::SetPlay(_bool isturn)
{
	m_isPlaying = isturn;

	if (m_eState == TS_DEFENCE)
	{
		m_eState = TS_END;
		m_pModel->SetUpAnimationIndex((_int)P_IDLE);
	}
}

void Client::Turn::SeletedSkill(_int iUsingSkill)
{
	m_iUsingSkill = iUsingSkill;
	m_eCurAct = ACT_TARGET;
}

void Client::Turn::StartEscape()
{
	m_eState = TS_ESCAPE;
	m_eCurAct = ACT_TARGET;
	m_eNextAct = ACT_TARGET;
}

void Client::Turn::ChangePersona()
{
}

void Client::Turn::UsingSkill(_double TimeDelta)
{
	if (m_eState != TS_SKILL)
	{
		m_eState = TS_SKILL;

		//상태 셋팅
		m_PlayAni = false;
		//UI 띄우기 
		m_pBtlMgr->SetSkillSelect(true, 0, m_pPersona->GetPersonaSkills());
		m_eCurAct = ACT_END;

	}
	switch (m_eCurAct)
	{
	case ACT_SELECT:
	{
		m_eCurAct = ACT_TARGET;
		m_pBtlMgr->StartSelectTarget();
	}
		break;
	case ACT_TARGET:
	{
		SelectTarget();
	}
		break;
	case ACT_MOVE:
		if (!m_PlayAni)
		{
			m_PlayAni = true;

			PlayMgr* pPlayMgr = GET_INSTANCE(PlayMgr);
			PERSONADATA* pPersonaData = m_pPersona->GetPersonaData();
			_int sp = pPlayMgr->UsingSP(m_iTurnNum, pPersonaData->Skills[m_iUsingSkill].iCost);
			m_Status.iMp = sp;

			RELEASE_INSTANCE(PlayMgr);
			//애니메이션
			m_pModel->SetUpAnimationIndex((_int)P_USINGCARD);

			//카메라 설정 
			if (m_Status.eAttackType == ATK_NEAR)
			{ 
				//나 바라보기 
				_vector vPos = XMVectorSetY(m_vOrignPos,1);
				_vector vCam = m_vOrignPos + VEC_ONE;
				m_pBtlMgr->MovingCamera(vCam, vPos, -VEC_RIGHT);
			}
			else
			{
				//몸 전체 보여주기 
				_vector vPos = XMVectorSetY(m_vOrignPos, 1);
				_vector vCam = m_vOrignPos + (VEC_LOOK * 2) - (VEC_RIGHT * 2) + (VEC_UP * 3);
				m_pBtlMgr->MovingCamera(vCam, vPos, -VEC_RIGHT);
			}
			//이펙트 

			m_eNextAct = ACT_MOVE;
		}
		else
		{
			m_pPersona->SetVisible(true);
			if (m_Status.eAttackType == ATK_NEAR) 
			{
				//몸 전체 보여주기 
				_vector vPos = XMVectorSetY(m_vOrignPos, 1);
				_vector vCam = m_vOrignPos + (VEC_LOOK * 2) - (VEC_RIGHT * 2) + (VEC_UP * 3);
				m_pBtlMgr->MovingCamera(vCam, vPos, -VEC_RIGHT);
			}
			else
			{
				//나 바라보기 
				_vector vPos = XMVectorSetY(m_vOrignPos, 1);
				_vector vCam = m_vOrignPos - VEC_RIGHT + VEC_LOOK + VEC_UP;
				m_pBtlMgr->MovingCamera(vCam, vPos, -VEC_RIGHT);
			}
			m_eNextAct = ACT_ATTACK;
		}
		m_eCurAct = ACT_WAIT;
		break;
	case ACT_WAIT:
	{
		m_Time += TimeDelta;
		if (m_Time >= 1.f)
		{
			m_Time = 0;
			m_eCurAct = m_eNextAct;
		}
	}
	break;
	case ACT_ATTACK:
	{

		//몬스터 데미지 
		PERSONADATA* pPersonaData = m_pPersona->GetPersonaData();
		m_pBtlMgr->TurnUsingSkill(pPersonaData->iStr, &(pPersonaData->Skills[m_iUsingSkill]));

		if (pPersonaData->Skills[m_iUsingSkill].eSkillType == SKILL_UP)
		{
			m_pBtlMgr->TotalCamera(0, VEC_UP);
		}
		else 
		{
			_vector target = XMVectorSet(0, 0, 0, XMVectorGetW(m_vTargetPos) + 4);
			_vector vPos = m_vOrignPos + VEC_RIGHT - VEC_LOOK + (VEC_UP * 2);

			m_pBtlMgr->MovingCamera(vPos, target, -(VEC_RIGHT*0.5f));
		}
		m_eNextAct = ACT_USE;
		m_eCurAct = ACT_WAIT;
	}
	break;
	case ACT_USE:
	{
		m_isPlaying = false;

		m_eCurAct = ACT_END;
		m_eNextAct = ACT_END;
		m_eState = TS_END;
		m_Time = 0.f;

		m_pBtlMgr->NextTurn();
	}
	break;
	
	}

}

void Client::Turn::UsingItem()
{
	//목록

	//애니메이션
	m_pModel->SetUpAnimationIndex((_int)P_USINGITEM);

	//카메라

	//효과적용 

	m_isPlaying = false;
}

void Client::Turn::Attack(_double TimeDelta)
{
	if (m_eState != TS_ATTACK)
	{
		m_eState = TS_ATTACK;

		//상태 셋팅
		m_eCurAct = ACT_TARGET;

		//대상 선택
		m_pBtlMgr->StartSelectTarget();
	}
	//else if (m_eCurAct != m_eNextAct)
	//	m_eCurAct = m_eNextAct;
	
	switch (m_eCurAct)
	{
	case ACT_TARGET:
	{
		SelectTarget();
	}
	break;
	case ACT_MOVE:
	{
		//공격 모션 
		if (!m_PlayAni)
		{
			m_pModel->SetUpAnimationIndex((_int)P_ATTACK);
			m_PlayAni = true;
			//카메라 

			m_eCurAct = ACT_WAIT;
			m_eNextAct = ACT_ATTACK;
		}
	}
		break;
	case ACT_WAIT:
	{
		m_Time += TimeDelta;
		if (m_Time >= 2)
		{
			m_eCurAct = m_eNextAct;
		}
	}
	break;
	case ACT_ATTACK:
	{
		//몬스터 데미지 
		PERSONADATA* pPersonaData = m_pPersona->GetPersonaData();
		m_pBtlMgr->TurnUsingSkill(pPersonaData->iStr);

		m_eCurAct = ACT_WAIT;
		m_eNextAct = ACT_USE;
	}
	break;
	case ACT_USE:
	{
		//결과 처리 
		m_isPlaying = false;

		m_eCurAct = ACT_END;
		m_eNextAct = ACT_END;
		m_eState = TS_END;
		m_Time = 0.f;

		m_pBtlMgr->NextTurn();
	}
	break;
	}
}

void Client::Turn::Defence()
{
	if (TS_DEFENCE != m_eState) 
	{
		m_eState = TS_DEFENCE;
		m_pModel->SetUpAnimationIndex((_int)P_DEFENCE);
	}

	if (m_pModel->GetCurrentAnimationEnd())
	{
		m_eNextAct = ACT_END;
		m_isPlaying = false;
		m_pBtlMgr->NextTurn();
	}
}

void Client::Turn::Escape(_double Timedelta)
{
	if (m_isPlaying)
	{
		m_Time += Timedelta;
		if (m_Time >= 1.5f)
		{
			m_isPlaying = false;
			
			m_Time = 0;
			m_eState = TS_CHANGESCENE;
			//씬 변경
			m_pBtlMgr->ReturnGameScene();
			return;
		}
	}
	 if (m_eState != TS_ESCAPE)
	{
		m_eState = TS_ESCAPE;
		m_eNextAct = ACT_TARGET;
		
		//카메라 셋팅
		
		//m_pCamera->Escape();
	}
	
	if (m_eCurAct != m_eNextAct)
		m_eCurAct = m_eNextAct;

	//도망 모션 
	if (m_eCurAct == ACT_TARGET)
	{
		_vector vLook = m_pTransform->GetState(Transform::STATE_LOOK);
		_float fRadian = XMVectorGetX(XMVector3Dot(XMVectorSet(0, 0, 1, 0), vLook));
		fRadian = acosf(fRadian);

		m_pTransform->SetUpRotation(XMVectorSet(0, 1, 0, 0), fRadian + XMConvertToRadians(180.f));
		m_pModel->SetUpAnimationIndex((_int)P_RUN);

		m_eNextAct = ACT_MOVE;
	}
	else if (m_eCurAct == ACT_MOVE)
	{
		m_pTransform->GoStright(Timedelta);
	}
}

_bool Client::Turn::Damaged(_float fEnemyStr,SKILLDATA * pSkill)
{
	//스킬타입확인
	int iType = (6-(int)pSkill->eSkillType);
	//본인 상성확인
	PERSONADATA* pPersona = m_pPersona->GetPersonaData();

	//무효
	if ((pPersona->iInvalidity & iType) > 0)
		return false;
	//내성
	else if ((pPersona->iTolerance & iType) > 0)
	{

	}
	//약점
	else if ((pPersona->iWeakness & iType) > 0)
	{

	}

	int iDamage = 0;
	if (pSkill->EffectDesc == "Half")
	{
		iDamage = (_float)m_Status.iHp*0.5f;
	}
	else
	{
		//데미지 계산
		iDamage = 5 * sqrt(fEnemyStr / pPersona->iDef * pSkill->fDamage) * 1 * pSkill->iAttCount;
	}
	//체력계산
	PlayMgr* pPlayMgr = GET_INSTANCE(PlayMgr);
	_bool isDead = pPlayMgr->DamagedPlayer(m_iTurnNum, iDamage);
	m_Status.iHp -= iDamage;

	RELEASE_INSTANCE(PlayMgr);

	if (isDead)
	{
		//죽는모션
		m_isDead = true;
		m_PlayAni = true;
		m_pModel->SetUpAnimationIndex((int)P_DEATH);

		if (m_iTurnNum == 0)
			m_pBtlMgr->PlayerDead();
	}
	else
	{
		m_pModel->SetUpAnimationIndex((_int)P_DAMAGED);
		m_PlayAni = true;

		if (m_eState == TS_DEFENCE)
			m_eState = TS_END;
	}
	return isDead;
}

void Client::Turn::AddHP()
{
	PlayMgr* pPlayMgr = GET_INSTANCE(PlayMgr);
	m_Status.iHp = pPlayMgr->AddHp(m_iTurnNum);
	RELEASE_INSTANCE(PlayMgr);
}

void Client::Turn::SelectTarget()
{
	GameInstance*	pGameInstance = GET_INSTANCE(GameInstance);
	//대상 바꾸기
	if (pGameInstance->GetKeyDown(VK_LEFT))
		m_pBtlMgr->ChangeTarget();

	//대상 확정
	if (pGameInstance->GetKeyDown(VK_SPACE))
	{
		m_vTargetPos = m_pBtlMgr->SelectedTarget();
		//m_eNextAct = ACT_MOVE;
		m_eCurAct = ACT_MOVE;
		m_PlayAni = false;
	}
	RELEASE_INSTANCE(GameInstance);
}

HRESULT Client::Turn::SetUpComponents()
{
	Transform::TRANSFORMDESC transDesc;
	transDesc.fSpeedPerSec = 7.f;
	transDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUpComponents(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (Component**)&m_pTransform, &transDesc)))
		return E_FAIL;

	if (FAILED(__super::SetUpComponents(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (Component**)&m_pRenderer)))
		return E_FAIL;

	string str = "Prototype_Component_Model_" + string(m_Status.strName);
	const char* szModel = str.c_str();
	int len = 1 + strlen(szModel);
	size_t cn;
	_tchar* ModelName = new _tchar[len];
	mbstowcs_s(&cn, ModelName, len, szModel, len);

	if (FAILED(__super::SetUpComponents(SCENE_GAMEPLAY, ModelName, TEXT("Com_Model"), (Component**)&m_pModel)))
		return E_FAIL;

	m_eTurn = BATTLE_PLAYER;

	//이름으로 페르소나 오브젝트 찾기
	DataManager* pDataMgr = GET_INSTANCE(DataManager);

	PERSONADATA* pPersonaData = nullptr;
	pDataMgr->GetPersonaData(string(m_Status.strPersonaID), &pPersonaData);

	if (pPersonaData == nullptr)
		return E_FAIL;

	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	GameObject* pOut;
	if (FAILED(pGameInstance->AddGameObjectToLayer(SCENE_BATTLE, TEXT("Layer_Persona"), TEXT("Prototype_GameObject_Persona"), pPersonaData,&pOut)))
		return E_FAIL;

	m_pPersona = static_cast<Persona*>(pOut);
	//_vector vLook = XMVector4Normalize(m_pTransform->GetState(Transform::STATE_LOOK));
	_vector vPos = m_vOrignPos + (VEC_LOOK);//m_pTransform->GetState(Transform::STATE_POSITION) + (vLook * 2);
	m_pPersona->SetPosition(vPos, VEC_LOOK);

	RELEASE_INSTANCE(GameInstance);
	RELEASE_INSTANCE(DataManager);

	//무기 부착
	if ("" != string(m_Status.strWeapon))
	{
		//string to tchar
		str = "Prototype_Component_Model_" + string(m_Status.strWeapon);
		const char* szWeapon = str.c_str();
		len = 1 + strlen(szWeapon);
		size_t count;
		_tchar* WeaponName = new _tchar[len];
		mbstowcs_s(&count, WeaponName, len, szWeapon, len);

		if (FAILED(__super::SetUpComponents(SCENE_GAMEPLAY, WeaponName, TEXT("Com_PWeapon"), (Component**)&m_pWeapon)))
			return E_FAIL;

		m_pWeaponNode = m_pModel->GetBoneNode(m_Status.strWeaponBone);
	}
	else
		m_pWeapon = nullptr;

	return S_OK;
}

HRESULT Client::Turn::SetUpWeapon()
{
	_matrix		TransMatrix = XMMatrixIdentity();
	TransMatrix = XMMatrixRotationAxis(XMVectorSet(1, 0, 0, 0), XMConvertToRadians(90.f))
		*XMMatrixRotationAxis(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(-15.f))
		*XMMatrixTranslation(-55.f, 100.f, 0.f);

	_matrix		BoneMatrix = TransMatrix* m_pWeaponNode->GetOffsetMatrix() * m_pWeaponNode->GetCombinedMatrix()
		* m_pModel->GetPivotMatrix() * m_pTransform->GetWorldMatrix();

	XMStoreFloat4x4(&m_WeaponMat, BoneMatrix);

	return S_OK;
}

Turn * Client::Turn::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	Turn*	pInstance = new Turn(pDevice, pDeviceContext);
	if (FAILED(pInstance->Ready_Prototype()))
	{
		MSGBOX("Failed to Creating Turn");
		Safe_Release(pInstance);
	}
	return pInstance;
}

Turn * Client::Turn::Clone(void * pArg)
{
	Turn*	pInstance = new Turn(*this);
	if (FAILED(pInstance->Ready(pArg)))
	{
		MSGBOX("Failed to Creating Turn Clone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void Client::Turn::Free()
{
	__super::Free();

	Safe_Release(m_pBtlMgr);
//	Safe_Release(m_pPersona);
	//Safe_Release(m_pTransform);
	//Safe_Release(m_pModel);
	//Safe_Release(m_pWeapon);
	//Safe_Release(m_pRenderer);
}
