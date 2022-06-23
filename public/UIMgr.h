#pragma once
#include "Client_Define.h"
#include "Base.h"

namespace Client
{
	class UIMgr :public Base
	{
		DECLARE_SINGLETON(UIMgr)
	public:
		explicit UIMgr();
		virtual ~UIMgr() = default;

	public:
		virtual void Free() override;

	private:
//		class PersonaUI* m_pPersonaUI;
	};

}