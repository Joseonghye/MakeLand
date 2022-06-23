#include "stdafx.h"
#include "..\public\PlayMgr.h"
#include "PlayerStatus.h"

IMPLEMENT_SINGLETON(PlayMgr)

PlayMgr::PlayMgr()
	:m_pPlayerStatus(nullptr)
{
}

HRESULT Client::PlayMgr::ReadyToPlay()
{
	if (m_pPlayerStatus != nullptr)
		return S_OK;

	m_pPlayerStatus = new PlayerStatus();
	if (m_pPlayerStatus->Ready())
		return E_FAIL;

	return S_OK;
}

void Client::PlayMgr::Update()
{
	m_pPlayerStatus->Update();
}

_bool Client::PlayMgr::DamagedPlayer(_int iIndex, _int iDamage)
{
	return m_pPlayerStatus->DamagedPlayer(iIndex, iDamage);
}

_int Client::PlayMgr::UsingSP(_int iIndex, _int sp)
{
	return m_pPlayerStatus->UsingSp(iIndex, sp);
}

_int Client::PlayMgr::AddHp(_int iIndex)
{
	return m_pPlayerStatus->AddHp(iIndex);
}

vector<PLAYERDATA*> Client::PlayMgr::GetPlayerDatas()
{
	return m_pPlayerStatus->GetPlayerDatas();
}

void Client::PlayMgr::Free()
{
}
