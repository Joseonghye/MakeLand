#include "stdafx.h"
#include "..\public\PlayerStatus.h"
#include "DataManager.h"
#include "StatusUI.h"
#include "UIImage.h"

PlayerStatus::PlayerStatus()
{
}


PlayerStatus::~PlayerStatus()
{
}

HRESULT Client::PlayerStatus::Ready()
{
	DataManager* pDataMgr = GET_INSTANCE(DataManager);
	
	PLAYERDATA* pPlayerData;
	pDataMgr->GetPlayerData("p_00", &pPlayerData);
	m_vecStatus.push_back(pPlayerData);
	pDataMgr->GetPlayerData("p_10", &pPlayerData);
	m_vecStatus.push_back(pPlayerData);
	pDataMgr->GetPlayerData("p_20", &pPlayerData);
	m_vecStatus.push_back(pPlayerData);
	pDataMgr->GetPlayerData("p_30", &pPlayerData);
	m_vecStatus.push_back(pPlayerData);


	//m_vecStatusUIs
	StatusUI* pUI = StatusUI::Create(CH_YU, "Yu", 1200, 80);
	m_vecStatusUIs.push_back(pUI);

	StatusUI* Yukiko = StatusUI::Create(CH_YUKIKO, "Yukiko", 1200, 210);
	m_vecStatusUIs.push_back(Yukiko);

	StatusUI* Kanji = StatusUI::Create(CH_KANJI, "Kanji", 1200, 340);
	m_vecStatusUIs.push_back(Kanji);

	StatusUI* Naoto = StatusUI::Create(CH_NAOTO, "Naoto", 1200, 470);
	m_vecStatusUIs.push_back(Naoto);


	RELEASE_INSTANCE(DataManager);
	return S_OK;
}

void Client::PlayerStatus::Update()
{
	//for (int i = 0; i < 4; ++i)
	//{
	//	m_vecStatusUIs[i]->SetStatus(GetHpPer(i), GetMpPer(i));
	//}
}

_bool Client::PlayerStatus::DamagedPlayer(_int iIndex, _int iDamage)
{
	if (m_vecStatus[iIndex]->iHp <= iDamage)
	{
		m_vecStatus[iIndex]->iHp = 0;
		m_vecStatusUIs[iIndex]->SetStatus(0, GetMpPer(iIndex));
		return true;
	}
	m_vecStatus[iIndex]->iHp -= iDamage;

	m_vecStatusUIs[iIndex]->SetStatus(GetHpPer(iIndex), GetMpPer(iIndex));

	return false;
}

_int Client::PlayerStatus::UsingSp(_int iIndex, _int iSp)
{
	m_vecStatus[iIndex]->iMp -= iSp;

	m_vecStatusUIs[iIndex]->SetStatus(GetHpPer(iIndex), GetMpPer(iIndex));

	return m_vecStatus[iIndex]->iMp;
}

_int Client::PlayerStatus::AddHp(_int iIndex)
{
	_int Add = m_vecStatus[iIndex]->iMaxHp *0.1f;
	m_vecStatus[iIndex]->iHp += Add;
	if (m_vecStatus[iIndex]->iHp > m_vecStatus[iIndex]->iMaxHp)
		m_vecStatus[iIndex]->iHp = m_vecStatus[iIndex]->iMaxHp;

	m_vecStatusUIs[iIndex]->SetStatus(GetHpPer(iIndex), GetMpPer(iIndex));

	return m_vecStatus[iIndex]->iHp;
}

_float Client::PlayerStatus::GetHpPer(_int iIndex)
{
	return (_float)(m_vecStatus[iIndex]->iHp) / (_float)(m_vecStatus[iIndex]->iMaxHp);
}

_float Client::PlayerStatus::GetMpPer(_int iIndex)
{
	return (_float)(m_vecStatus[iIndex]->iMp) / (_float)(m_vecStatus[iIndex]->iMaxMp);
}
