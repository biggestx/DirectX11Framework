#include "Timer.h"

USING_BIGGEST

CTimer::CTimer()
{
}

CTimer::~CTimer()
{
}

bool CTimer::Init()
{
	QueryPerformanceFrequency(&m_tSecond);
	QueryPerformanceCounter(&m_tTime);
	m_fDeltaTime = 0.f;

	m_fAccTime = 0.f;
	m_iTick = 0;
	m_iFrameLimit = 0;

	return true;
}

void CTimer::Update()
{
	CalculateTime();
	CalculateFPS();
}

void CTimer::CalculateTime()
{
	LARGE_INTEGER	tTime;
	QueryPerformanceCounter(&tTime);

	m_fDeltaTime = (tTime.QuadPart - m_tTime.QuadPart) /
		(float)m_tSecond.QuadPart;

	m_tTime = tTime;
}

void CTimer::CalculateFPS()
{
	++m_iTick;
	m_fAccTime += m_fDeltaTime;

	if (m_fAccTime >= 1.f)
	{
		m_fFPS = m_iTick / m_fAccTime;
		m_iTick = 0;
		m_fAccTime = 0.f;
		char	strFPS[128] = {};
		sprintf_s(strFPS, "FPS : %.5f\n", m_fFPS);
		_cprintf(strFPS);
		sprintf_s(strFPS, "DeltaTime : %.5f\n", m_fDeltaTime);
		_cprintf(strFPS);
	}
}
