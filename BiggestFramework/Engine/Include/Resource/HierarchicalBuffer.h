#pragma once
#include "BaseResource.h"

BIGGEST_BEGIN

class BIGGEST_DLL CHierarchicalBuffer :
	public CBaseResource
{
	CREATE_INSTANCE_TAG(CHierarchicalBuffer)
private:
	map<string, CBaseResource*>		m_mapResources;

public:
	void AddResource(string strKey, CBaseResource* pBuffer);

public:
	bool Init();
	virtual bool CreateResource(TCHAR* pFileName = NULL, const string& strPathKey = TEXTURE_PATH_KEY);
	virtual void Render();
	virtual void Render(const DWORD& dwSubset);

public:
	CHierarchicalBuffer();
	~CHierarchicalBuffer();
};

BIGGEST_END