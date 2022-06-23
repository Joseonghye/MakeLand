#pragma once
#include "Client_Define.h"
#include "Base.h"

namespace Client
{
	class PlayerStatus;

	class PlayMgr final : public Base
	{
		DECLARE_SINGLETON(PlayMgr)

	public:
		PlayMgr();
		~PlayMgr() = default;

	public:
		HRESULT	ReadyToPlay();
		void Update();

	public:
		_bool DamagedPlayer(_int iIndex, _int iDamage);
		_int UsingSP(_int iIndex, _int sp);
		_int AddHp(_int iIndex);

		vector<PLAYERDATA*> GetPlayerDatas();

	public:
		virtual void Free() override;

	private:
		PlayerStatus*		m_pPlayerStatus;

	};

}