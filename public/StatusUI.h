#pragma once
#include "Client_Define.h"
#include "Engine_Define.h"

namespace Client
{
	class UIImage;
	class StatusUI
	{
	public:
		StatusUI(CHARACTER chName);
		~StatusUI() = default;

	public:
		HRESULT Ready(string strChName, _float fX, _float fY);
		void Free();

	public:
		static StatusUI* Create(CHARACTER chName, string strChName, _float fX, _float fY);
		void	SetStatus(_float fHpPer, _float fMpPer);

	private:
		UIImage*  m_pFrameImg = nullptr;
		UIImage*  m_pHpBar = nullptr;
		UIImage*  m_pMpBar = nullptr;
	
		CHARACTER	m_eChName;
	};

}