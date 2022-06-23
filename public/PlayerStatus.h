#pragma once
#include "Client_Define.h"
#include "Engine_Define.h"

namespace Client
{
	class Persona;
	class StatusUI;
	class UIImage;

	class PlayerStatus
	{
	public:
		PlayerStatus();
		~PlayerStatus();
	public:
		HRESULT Ready();
		void	Update();

	public:
		_bool		DamagedPlayer(_int iIndex, _int iDamage);
		_int		UsingSp(_int iIndex, _int iSp);
		_int		AddHp(_int iIndex);

		vector<PLAYERDATA*> GetPlayerDatas() { return m_vecStatus; }
		_int				GetPlayerMP(_int iIndex) { return m_vecStatus[iIndex]->iMp; }

		void		AddPlayerStatus(PLAYERDATA* status) { m_vecStatus.push_back(status); }

	private:
		_float			GetHpPer(_int iIndex);
		_float			GetMpPer(_int iIndex);


	private:
		vector<PLAYERDATA*> m_vecStatus;
		vector<StatusUI*>	m_vecStatusUIs;
		UIImage*			m_pNowImg;

	};

}