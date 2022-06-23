#include "stdafx.h"
#include "DataManager.h"
#include <fstream>
#include <sstream>

IMPLEMENT_SINGLETON(DataManager)

Client::DataManager::DataManager()
{

}

void Client::DataManager::GetBattlePosData(string key,_float4* pOut)
{
	auto iter = m_BattlePosData.find(key);

	if (iter == m_BattlePosData.end())
		pOut = nullptr;

	for (int i = 0; i < 5; ++i)
	{
		pOut[i] = iter->second[i];
	}
}

void Client::DataManager::GetPersonaData(string PersonaID, PERSONADATA ** pOut)
{
	auto iter = m_PersonaData.find(PersonaID);

	if (iter == m_PersonaData.end())
		pOut = nullptr;
	else
		*pOut = &(iter->second);
}

void Client::DataManager::GetPlayerData(string PlayerID, PLAYERDATA ** pOut)
{
	auto iter = m_PlayerData.find(PlayerID);

	if (iter == m_PlayerData.end())
		pOut = nullptr;

	*pOut = &(iter->second);
}

void Client::DataManager::GetEnemyData(string EnemyID, ENEMYDATA ** pOut)
{
		auto iter = m_EnemyData.find(EnemyID);

		if (iter == m_EnemyData.end())
			pOut = nullptr;

		*pOut = &(iter->second);
}

void Client::DataManager::GetSkillData(string EnemyID, SKILLDATA ** pOut)
{
	auto iter = m_SkillData.find(EnemyID);

	if (iter == m_SkillData.end())
		pOut = nullptr;

	*pOut = &(iter->second);
}

HRESULT Client::DataManager::Ready()
{
	if (FAILED(ReadBattlePos()))
		return E_FAIL;
	
	if (FAILED(ReadSkill()))
		return E_FAIL;

	if (FAILED(ReadPersona()))
		return E_FAIL;

	if(FAILED(ReadPlayer()))
		return E_FAIL;

	if (FAILED(ReadEnemy()))
		return E_FAIL;



	return S_OK;
}

HRESULT Client::DataManager::ReadBattlePos()
{
	fstream fs;
	fs.open("../bin/Resources/Data/BattlePosData.csv", ios::in);

	string strline;
	string strName;
	string strBuf;
	string strPos;
	BATTLEPOSITIONS vPositions;
	vPositions.resize(5);

	if (!fs.is_open())
		return E_FAIL;

	//첫줄 건너뛰기 
	getline(fs, strline);

	while (!fs.eof())
	{
		getline(fs, strline);
		istringstream LineSS(strline);
		getline(LineSS, strName, ',');

		for (int i = 0; i < 5; ++i)
		{
			getline(LineSS, strBuf, ',');

			istringstream ss(strBuf);
			getline(ss, strPos, '/');
			vPositions[i].x = stof(strPos);
			getline(ss, strPos, '/');
			vPositions[i].y = stof(strPos);
			getline(ss, strPos, '/');
			vPositions[i].z = stof(strPos);
			getline(ss, strPos, '/');
			vPositions[i].w = stof(strPos);
		}
		m_BattlePosData.emplace(strName, vPositions);
	}
	fs.close();

	return S_OK;
}

HRESULT Client::DataManager::ReadPersona()
{
	fstream fs;
	fs.open("../bin/Resources/Data/PersonaData.csv", ios::in);

	string strline;
	string strID;
	string strBuf;
	string strState;

	if (!fs.is_open())
		return E_FAIL;

	//첫줄 건너뛰기 
	getline(fs, strline);

	while (!fs.eof())
	{
		getline(fs, strline);
		if (strline == "")
			break;

		PERSONADATA persona;
		istringstream LineSS(strline);
		getline(LineSS, strID, ',');

		//이름
		getline(LineSS, strBuf, ',');
		persona.strName = strBuf;

		//레벨
		getline(LineSS, strBuf, ',');
		persona.iLevel = stof(strBuf);

		//스탯
		getline(LineSS, strBuf, ',');

		istringstream ss(strBuf);
		getline(ss, strState, '/');
		persona.iStr = stof(strState);
		
		getline(ss, strState, '/');
		persona.iInt = stof(strState);

		getline(ss, strState, '/');
		persona.iDef = stof(strState);

		getline(ss, strState, '/');
		persona.iDex = stof(strState);

		getline(ss, strState, '/');
		persona.iLuk = stof(strState);
		//-----------------------------
		//내성
		getline(LineSS, strBuf, ',');
		persona.iTolerance = stoi(strBuf);
		//무효
		getline(LineSS, strBuf, ',');
		persona.iInvalidity = stoi(strBuf);
		//약점
		getline(LineSS, strBuf, ',');
		persona.iWeakness = stoi(strBuf);

	//====스킬=============================
		getline(LineSS, strBuf, ',');

		string strChk = strBuf;
		istringstream stream(strBuf);

		vector<string> SkillIDs;
		while (true) 
		{
			getline(stream, strState, '/');
			string skillID = strState;
			if (skillID == "") break;

			if (skillID == strChk) break;
			else
				strChk = skillID;

			SkillIDs.push_back(skillID);

		
		}
		int iCnt = SkillIDs.size();
		persona.iSkillCnt = iCnt;
		if (iCnt == 1)
		{
			GetSkillData(SkillIDs.front(), &persona.Skills);
		}
		else
		{
			SKILLDATA* skillData = new SKILLDATA[iCnt];
			for (int i = 0; i < iCnt; i++)
			{
				SKILLDATA* pSkill;
				GetSkillData(SkillIDs[i], &pSkill);
				skillData[i] = *pSkill;
			}
		
			persona.Skills = skillData;
		}
	

		//메쉬 경로,이름 
		getline(LineSS, strBuf, ',');
		persona.strModelName = strBuf;

		m_PersonaData.emplace(strID, persona);
	}
	fs.close();

	return S_OK;
}

HRESULT Client::DataManager::ReadPlayer()
{
	fstream fs;
	fs.open("../bin/Resources/Data/PlayerData.csv", ios::in);

	string strline;
	string strID;
	string strBuf;
	string strState;

	if (!fs.is_open())
		return E_FAIL;

	//첫줄 건너뛰기 
	getline(fs, strline);

	while (!fs.eof())
	{
		getline(fs, strline);
		if (strline == "")
			break;

		istringstream LineSS(strline);
		getline(LineSS, strID, ',');

		PLAYERDATA	PlayerData;

		//이름
		getline(LineSS, strBuf, ',');
		PlayerData.strName = strBuf;

		//레벨
		getline(LineSS, strBuf, ',');
		PlayerData.iLevel = stof(strBuf);

		//무기 이름 
		getline(LineSS, strBuf, ',');
		PlayerData.strWeapon = strBuf;

		getline(LineSS, strBuf, ',');
		PlayerData.strWeaponBone = strBuf;

		//HP
		getline(LineSS, strBuf, ',');
		PlayerData.iMaxHp = stoi(strBuf);
		PlayerData.iHp = PlayerData.iMaxHp;
		//MP
		getline(LineSS, strBuf, ',');
		PlayerData.iMaxMp = stoi(strBuf);
		PlayerData.iMp = PlayerData.iMaxMp;

		//페르소나 
		getline(LineSS, strBuf, ',');
		PlayerData.strPersonaID = strBuf;

		getline(LineSS, strBuf, ',');
		PlayerData.eAttackType = (ATKTYPE)stoi(strBuf);

		m_PlayerData.emplace(strID, PlayerData);
	}
	fs.close();

	return S_OK;
}

HRESULT Client::DataManager::ReadEnemy()
{
	fstream fs;
	fs.open("../bin/Resources/Data/EnemyData.csv", ios::in);

	string strline;
	string strID;
	string strBuf;
	string strState;

	if (!fs.is_open())
		return E_FAIL;

	//첫줄 건너뛰기 
	getline(fs, strline);

	while (!fs.eof())
	{
		getline(fs, strline);
		if (strline == "")
			break;

		ENEMYDATA Enemy;

		istringstream LineSS(strline);
		getline(LineSS, strID, ',');

		//이름
		getline(LineSS, strBuf, ',');
		Enemy.strName = strBuf;

		//레벨
		getline(LineSS, strBuf, ',');
		Enemy.strLevel = strBuf;

		getline(LineSS, strBuf, ',');
		Enemy.iMaxHp = stof(strBuf);
		Enemy.iHp = Enemy.iMaxHp;

		//스탯
		getline(LineSS, strBuf, ',');

		istringstream ss(strBuf);
		getline(ss, strState, '/');
		Enemy.iStr = stof(strState);

		getline(ss, strState, '/');
		Enemy.iInt = stof(strState);

		getline(ss, strState, '/');
		Enemy.iDef = stof(strState);

		getline(ss, strState, '/');
		Enemy.iDex = stof(strState);

		getline(ss, strState, '/');
		Enemy.iLuk = stof(strState);
		//-----------------------------
		//내성
		getline(LineSS, strBuf, ',');
		Enemy.iTolerance = stoi(strBuf);
		//무효
		getline(LineSS, strBuf, ',');
		Enemy.iInvalidity = stoi(strBuf);
		//약점
		getline(LineSS, strBuf, ',');
		Enemy.iWeakness = stoi(strBuf);

		m_EnemyData.emplace(strID, Enemy);
	}
	fs.close();
	return S_OK;
}

HRESULT Client::DataManager::ReadSkill()
{
	fstream fs;
	fs.open("../bin/Resources/Data/SkillData.csv", ios::in);

	string strline;
	string strID;
	string strBuf;
	string strState;

	if (!fs.is_open())
		return E_FAIL;

	//첫줄 건너뛰기 
	getline(fs, strline);

	while (!fs.eof())
	{
		getline(fs, strline);
		if (strline == "")
			break;

		istringstream LineSS(strline);
		getline(LineSS, strID, ',');

		SKILLDATA	SkillData;

		//이름
		getline(LineSS, strBuf, ',');
		SkillData.strName = strBuf;

		getline(LineSS, strBuf, ',');
		SkillData.eSkillType = (SKILLTYPE)stoi(strBuf);

		getline(LineSS, strBuf, ',');
		SkillData.iCost = stoi(strBuf);

		getline(LineSS, strBuf, ',');
		SkillData.iTarget = stoi(strBuf);

		getline(LineSS, strBuf, ',');
		SkillData.iAttCount = stoi(strBuf);

		getline(LineSS, strBuf, ',');
		SkillData.fDamage = stof(strBuf);
		getline(LineSS, strBuf, ',');
		SkillData.fAccuracy = stof(strBuf);
		getline(LineSS, strBuf, ',');
		SkillData.fCritical = stof(strBuf);

		getline(LineSS, strBuf, ',');
		if (strBuf != "")
			SkillData.EffectDesc = strBuf;

		getline(LineSS, strBuf, ',');
		if (strBuf != "")
			SkillData.strSkillDesc = strBuf;

		m_SkillData.emplace(strID, SkillData);
	}
	fs.close();

	return S_OK;
}

void Client::DataManager::Free()
{
	//__super::Free();

	m_BattlePosData.clear();
	m_PersonaData.clear();
	m_PlayerData.clear();
	m_EnemyData.clear();

}
