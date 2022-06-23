#pragma once
#include "Client_Define.h"
#include "Base.h"

namespace Client
{
	class DataManager final: public Base
	{
		DECLARE_SINGLETON(DataManager)

	public:
		explicit DataManager();
		virtual ~DataManager() = default;

	public:
		void GetBattlePosData(string key, _float4* pOut);
		void GetPersonaData(string PersonaID, PERSONADATA** pOut);
		void GetPlayerData(string PlayerID, PLAYERDATA** pOut);
		void GetEnemyData(string EnemyID, ENEMYDATA** pOut);
		void GetSkillData(string EnemyID, SKILLDATA** pOut);

	public:
		HRESULT Ready();

	private:
		HRESULT ReadBattlePos();
		HRESULT ReadPersona();
		HRESULT ReadPlayer();
		HRESULT ReadEnemy();
		HRESULT ReadSkill();

	public:
		virtual void		Free() override;

	private:
		typedef vector<_float4>			BATTLEPOSITIONS;
		map<string, BATTLEPOSITIONS>	m_BattlePosData;

		typedef map<string, PERSONADATA>	PERSONADATAMAP;
		PERSONADATAMAP						m_PersonaData;

		typedef map<string, PLAYERDATA>		PLAYERDATAMAP;
		PLAYERDATAMAP						m_PlayerData;

		typedef map<string, ENEMYDATA>		ENEMYDATAMAP;
		ENEMYDATAMAP						m_EnemyData;
	
		typedef map<string, SKILLDATA>		SKILLDATAMAP;
		SKILLDATAMAP						m_SkillData;
	};

}