#pragma once

#include "../Core/Ref.h"

BIGGEST_BEGIN

class BIGGEST_DLL CBaseResource :
	public CRef
{
protected:
	RESOURCE_FLAG	m_RF;
	string			m_strPathKey;

public:
	void SetResourceFlag(const RESOURCE_FLAG& flag)
	{
		m_RF = flag;
	}

public:
	RESOURCE_FLAG GetResourceFlag()
	{
		return m_RF;
	}

public:
	virtual bool CreateResource(TCHAR* pFileName = NULL, const string& strPathKey = TEXTURE_PATH_KEY) = 0;
	virtual void Render() = 0;
	virtual void Render(const DWORD& dwSubset) = 0;
protected:
	CBaseResource();
	virtual ~CBaseResource();
};


BIGGEST_END