#pragma once

#include "Ref.h"

BIGGEST_BEGIN

class BIGGEST_DLL CFilePathManager :
	public CRef
{
private:
	map<string, wstring>	m_mapPath;
	char					m_strMultiByte[MAX_PATH];
	bool					m_bInitialize;

public:
	bool Init();
	bool CreatePath(const string& strKey, wchar_t* pPath,
		const string& strBasePathKey = ROOT_PATH_KEY);
	const wchar_t* FindPath(const string& strKey);
	const char* FindPathToMultiByte(const string& strKey);

	DECLARE_SINGLE(CFilePathManager)
};

BIGGEST_END