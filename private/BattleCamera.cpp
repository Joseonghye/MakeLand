#include "stdafx.h"
#include "..\public\BattleCamera.h"
#include "Turn.h"
#include "DataManager.h"
#include "GameInstance.h"
#include "SceneLoading.h"

Client::BattleCamera::BattleCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:Camera(pDevice, pDeviceContext)
{
}

Client::BattleCamera::BattleCamera(const BattleCamera & rhs)
	: Camera(rhs)
{
//	m_eCurBattleState = BATTLE_END;
//	m_eNextBattleState = BATTLE_END;
}

HRESULT Client::BattleCamera::Ready_Prototype()
{
	if (FAILED(__super::Ready_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT Client::BattleCamera::Ready(void * pArg)
{
	if (FAILED(__super::Ready(pArg)))
		return E_FAIL;
	/*
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	list<GameObject*> pTurns = pGameInstance->GetGameObjectsToLayer(SCENE_BATTLE, TEXT("Layer_Player"));

	if (pTurns.size() == 0)
		return E_FAIL;

	m_pTurns.reserve(pTurns.size());
	_int index = 0;
	for (auto& pTurn : pTurns)
	{
		m_pTurns.emplace_back(static_cast<Turn*>(pTurn));
		
		Safe_AddRef(m_pTurns[index]);
		m_pTurns[index]->SetPosition(XMLoadFloat4(&m_vPositions[index]), XMLoadFloat4(&m_vPositions[4]));
	
		m_pTurns[index]->SetCamera(this);

		if (TURN::TURN_ENEMY == m_pTurns[index]->GetWhoTurn())
			m_EnemyNums.push_back(index);

		index++;
	}

	//순서 (속도 정렬)

	pGameInstance->SetMainCamera(this);
	RELEASE_INSTANCE(GameInstance);
	
	*/
	return S_OK;
}

_int Client::BattleCamera::Update(_double TimeDelta)
{
#ifdef _DEBUG
	GameInstance*	pGameInstance = GET_INSTANCE(GameInstance);
	_long	MouseMove = 0; //마우스 감도

	//if (MouseMove = pGameInstance->Get_MouseMoveState(Input_Device::MMS_X))
	//{
	//	m_pTransform->RotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * MouseMove * 0.1f);
	//}

	//if (MouseMove = pGameInstance->Get_MouseMoveState(Input_Device::MMS_Y))
	//{
	//	m_pTransform->RotationAxis(m_pTransform->GetState(Transform::STATE_RIGHT), TimeDelta * MouseMove * 0.1f);
	//}

	RELEASE_INSTANCE(GameInstance);
#endif // _DEBUG

	if (m_isMove)
	{
		_vector vPos = m_pTransform->GetState(Transform::STATE_POSITION) + (XMLoadFloat3(&m_vMoveDir)*TimeDelta*0.1f);
		m_pTransform->SetState(Transform::STATE_POSITION, vPos);
	}

	return Camera::Update(TimeDelta);
}

_int Client::BattleCamera::LateUpdate(_double TimeDelta)
{
	/*
	//선택이 끝났는지확인용 
	if (m_isPlaying)
	{
		if (!m_pTurns[m_iCurTurn]->isPlaying())
		{
			m_isPlaying = false;

			//하는 행동에 따른 결과 


			m_iCurTurn++;

			if (m_iCurTurn >= m_pTurns.size())
				m_iCurTurn = 0;

			if (TURN_PLAYER == m_pTurns[m_iCurTurn]->GetWhoTurn())
				m_eNextBattleState = BATTLE_PLAYER;
			else m_eNextBattleState = BATTLE_ENEMY;
		}
	}
	*/
	return _int();
}

HRESULT Client::BattleCamera::Render()
{
	return S_OK;
}

void Client::BattleCamera::Moving(_fvector vPos, _fvector vTarget, _fvector vDir)
{
	m_pTransform->SetState(Transform::STATE_POSITION, vPos);

	_vector vLook = XMVector3Normalize(vTarget - vPos);
	m_pTransform->SetState(Transform::STATE_LOOK, vLook);

	_vector	vRight = XMVector3Cross(XMVectorSet(0, 1, 0, 0), vLook);
	vRight = XMVector3Normalize(vRight);
	_vector	vUp = XMVector3Cross(vLook, vRight);
	vUp = XMVector3Normalize(vUp);

	m_pTransform->SetState(Transform::STATE_RIGHT, vRight);
	m_pTransform->SetState(Transform::STATE_UP, vUp);

	m_isMove = true;
	XMStoreFloat3(&m_vMoveDir, vDir);
}

void Client::BattleCamera::SetCamera(_vector vPos, _vector vLook, _float vecScale)
{
	//_vector vDir = XMVector3Normalize(vChPos - vMonPos);
	//vDir = XMVectorSetY(vDir,1.5f);
	//_vector vPos = vChPos + vDir;
	vPos = vPos - (XMVector3Normalize(vLook) * vecScale);
	vPos = XMVectorSetY(vPos, 1.5f);
	m_pTransform->SetState(Transform::STATE_POSITION, vPos);

	//_vector	vLook = vMonPos - vPos;
	//vLook = XMVectorSetY(vLook, 0);
	m_pTransform->SetState(Transform::STATE_LOOK, XMVector3Normalize(vLook));

	_vector	vRight = XMVector3Cross(XMVectorSet(0, 1, 0, 0), vLook);
	vRight = XMVector3Normalize(vRight);

	_vector	vUp = XMVector3Cross(vLook, vRight);
	vUp = XMVector3Normalize(vUp);

	m_pTransform->SetState(Transform::STATE_RIGHT, vRight);
	m_pTransform->SetState(Transform::STATE_UP, vUp);
}

void Client::BattleCamera::LookAt(_vector vTarget)
{
	m_pTransform->LookAt(vTarget);
}

BattleCamera * Client::BattleCamera::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	BattleCamera* pInstance = new BattleCamera(pDevice, pDeviceContext);
	if (FAILED(pInstance->Ready_Prototype()))
	{
		MSGBOX("Failed to Creating BattleCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

GameObject * BattleCamera::Clone(void * pArg)
{
	BattleCamera* pInstance = new BattleCamera(*this);
	if (FAILED(pInstance->Ready(pArg)))
	{
		MSGBOX("Failed to Creating BattleCamera Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::BattleCamera::Free()
{
	__super::Free();

}
