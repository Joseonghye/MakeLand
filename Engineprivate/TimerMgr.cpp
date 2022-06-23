#include "..\public\TimerMgr.h"
#include "Timer.h"

IMPLEMENT_SINGLETON(TimerMgr)

Engine::TimerMgr::TimerMgr(void)
{

}

_double Engine::TimerMgr::GetTimeDelta(const _tchar* pTimerTag)
{
	Timer*		pInstance = Find_Timer(pTimerTag);
	if (nullptr == pInstance)
		return 0.0;

	return pInstance->GetTimeDelta();
}

HRESULT Engine::TimerMgr::Ready_Timer(const _tchar* pTimerTag)
{
	Timer*		pInstance = Find_Timer(pTimerTag);

	if (nullptr != pInstance)
		return E_FAIL;

	pInstance = Timer::Create();
	if (nullptr == pInstance)
		return E_FAIL;

	m_mapTimer.emplace(pTimerTag, pInstance);

	return S_OK;
}

void TimerMgr::Update_Timer(const _tchar * pTimerTag)
{
	Timer*		pInstance = Find_Timer(pTimerTag);
	if (nullptr == pInstance)
		return;

	pInstance->Update();
}

Engine::Timer* Engine::TimerMgr::Find_Timer(const _tchar* pTimerTag)
{
	// auto	iter = m_mapTimer.find(pTimerTag);

	auto	iter = find_if(m_mapTimer.begin(), m_mapTimer.end(), Tag_Finder(pTimerTag));

	if (iter == m_mapTimer.end())
		return nullptr;

	return iter->second;
}

void Engine::TimerMgr::Free(void)
{
	for (auto& Pair : m_mapTimer)
		Safe_Release(Pair.second);

	m_mapTimer.clear();
}
