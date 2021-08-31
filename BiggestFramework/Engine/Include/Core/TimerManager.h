#pragma once
#include "Ref.h"

BIGGEST_BEGIN

class BIGGEST_DLL CTimerManager :
	public CRef
{
private:
	map<string, class CTimer*>	m_mapTimer;

public:
	bool Init();
	CTimer* CreateTimer(const string& strKey, const string& strTag = "Default");
	CTimer* FindTimer(const string& strKey);

	DECLARE_SINGLE(CTimerManager)
};

BIGGEST_END