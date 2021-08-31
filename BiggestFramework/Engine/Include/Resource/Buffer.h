#pragma once
#include "BaseResource.h"

BIGGEST_BEGIN

class BIGGEST_DLL CBuffer :
	public CBaseResource
{
protected:
	ID3D11Buffer*	m_pBuffer;

public:
	ID3D11Buffer* GetBuffer()
	{
		return m_pBuffer;
	}


public:
	virtual bool Create(TCHAR* pFileName = NULL, const string& strPathKey = TEXTURE_PATH_KEY) = 0;
	virtual void Render() = 0;
	virtual void Render(const DWORD& dwSubset) = 0;

public:
	CBuffer();
	virtual ~CBuffer();
};

BIGGEST_END