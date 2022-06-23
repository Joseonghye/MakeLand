#pragma once
#include "Client_Define.h"
#include "Base.h"

namespace Client
{
	class UIImage;
	class UIButton;
	class RePlayPanel :	public Base
	{
	public:
		RePlayPanel();
		~RePlayPanel() = default;

	public:
		HRESULT	Ready();

		void RePlayActive();
		void Replay(_int isRe);
	public:
		static RePlayPanel*	Create();
		void	Free();

	private:
		UIImage*	m_pPanel;
		UIButton*	m_pSelectImg[2];
		UIImage*	m_pTargetImg;
		UIImage*	m_pDieImg;

	};

}