#pragma once
#include "Ref.h"

BIGGEST_BEGIN

class BIGGEST_DLL CTimer :
	public CRef
{
private:
	friend class CTimerManager;

private:
	LARGE_INTEGER	m_tTime;
	LARGE_INTEGER	m_tSecond;
	float			m_fDeltaTime;

public:
	float GetDeltaTime()
	{
		return m_fDeltaTime;
	}

private:	// FPS
	int		m_iTick;
	int		m_iFrameLimit;
	float	m_fAccTime;
	float	m_fFPS;

public:
	void LimitFrame(int iFrame)
	{
		m_iFrameLimit = iFrame;
	}

public:
	virtual bool Init();
	void Update();

private:
	void CalculateTime();
	void CalculateFPS();

protected:
	CTimer();

public:
	~CTimer();
};

BIGGEST_END