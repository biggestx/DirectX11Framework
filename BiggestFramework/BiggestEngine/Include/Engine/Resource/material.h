#pragma once
#include "BaseResource.h"

BIGGEST_BEGIN

class BIGGEST_DLL CMaterial :
	public CBaseResource
{
	CREATE_INSTANCE_TAG(CMaterial)
		FRIEND_CLASS(CShader)
private:
	XMFLOAT3	m_vAmbient;
	XMFLOAT3	m_vDiffuse;
	XMFLOAT3	m_vEmissive;

	CShader*	m_pShader;
	char		m_szTech[64];

	map<string, class CTexture*>	m_mapTexture;
	list<int>	m_passList;

public:
	void SetAmbient(XMFLOAT3 vAmb)
	{
		m_vAmbient = vAmb;
	}

	void SetDiffuse(XMFLOAT3 vDif)
	{
		m_vDiffuse = vDif;
	}

	void SetEmissive(XMFLOAT3 vEmv)
	{
		m_vEmissive = vEmv;
	}

	void SetShader(CShader* pShader)
	{
		m_pShader = pShader;
	}

	void SetTech(char* pTech)
	{
		strcpy(m_szTech, pTech);
	}

public:
	CShader* GetShader()
	{
		return m_pShader;
	}

	XMFLOAT3 GetAmbient()
	{
		return m_vAmbient;
	}

	XMFLOAT3 GetDiffuse()
	{
		return m_vDiffuse;
	}

	XMFLOAT3 GetEmissive()
	{
		return m_vEmissive;
	}

	const char* GetTech()
	{
		return m_szTech;
	}

	list<int>* GetPassList()
	{
		return &m_passList;
	}

public:
	void AddTexture(char* szName, class CTexture* pTexture);
	void AddPass(int iPass);
	void ConstantTable();

public:
	bool Init();
	virtual bool CreateResource(TCHAR* pFileName = NULL, const string& strPathKey = TEXTURE_PATH_KEY);
	virtual void Render();
	virtual void Render(const DWORD& dwSubset);

public:
	CMaterial();
	~CMaterial();
};

BIGGEST_END