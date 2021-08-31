#include "TimerManager.h"
#include "Timer.h"

USING_BIGGEST
DEFINITION_SINGLE(CTimerManager)

CTimerManager::CTimerManager()
{
}

CTimerManager::~CTimerManager()
{
	Safe_Release_Map(m_mapTimer);
}

bool CTimerManager::Init()
{
	CTimer*	pTimer = CreateTimer("DefaultTimer");
	SAFE_RELEASE(pTimer);

	return true;
}

CTimer* CTimerManager::CreateTimer(const string& strKey, const string& strTag)
{
	CTimer*	pTimer = FindTimer(strKey);

	if (pTimer)
	{
		pTimer->AddRef();
		return pTimer;
	}

	pTimer = new CTimer;

	pTimer->Init();
	pTimer->SetTag(strTag);

	m_mapTimer.insert(make_pair(strKey, pTimer));
	pTimer->AddRef();

	return pTimer;
}

CTimer* CTimerManager::FindTimer(const string& strKey)
{
	map<string, CTimer*>::iterator	iter = m_mapTimer.find(strKey);

	if (iter == m_mapTimer.end())
		return NULL;

	return iter->second;
}
