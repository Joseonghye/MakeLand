#include "..\public\Timer.h"

Engine::Timer::Timer(void)
	: m_fTimeDelta(0.0)
{

}

HRESULT Engine::Timer::Ready(void)
{
	QueryPerformanceCounter(&m_FrameTime);			// 1077
	QueryPerformanceCounter(&m_LastTime);			// 1085
	QueryPerformanceCounter(&m_FixTime);			// 1090

	QueryPerformanceFrequency(&m_CpuTick);

	return S_OK;
}

void Engine::Timer::Update(void)
{
	QueryPerformanceCounter(&m_FrameTime);			// 1500

	if (m_FrameTime.QuadPart - m_FixTime.QuadPart >= m_CpuTick.QuadPart)
	{
		QueryPerformanceFrequency(&m_CpuTick);
		m_FixTime = m_FrameTime;
	}

	m_fTimeDelta = (m_FrameTime.QuadPart - m_LastTime.QuadPart) / (_double)m_CpuTick.QuadPart;
	m_LastTime = m_FrameTime;
}

Engine::Timer* Engine::Timer::Create(void)
{
	Timer*		pInstance = new Timer;

	if (FAILED(pInstance->Ready()))
		Safe_Release(pInstance);

	return pInstance;
}

void Engine::Timer::Free(void)
{

}

