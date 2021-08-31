#pragma once

#include "../../Core/Ref.h"

BIGGEST_BEGIN
//
//typedef struct _tagConstantHandle
//{
//	ID3DX11Effect	hConstant;
//	SHADER_CONSTANT_DATA_TYPE	eType;
//}CONSTANTHANDLE, *PCONSTANTHANDLE;
//

typedef struct _tagConstantHandle
{
	ID3DX11EffectVariable* pConstant;
	SHADER_CONSTANT_DATA_TYPE eType;
}ConstantHandle, *PConstantHandle;


class BIGGEST_DLL CShader :
	public CRef
{
protected:
	ID3DX11Effect* m_pEffect;

	wstring		m_strFileName;
	string		m_strEntryPoint;
	string		m_strShaderModel;
	ID3DBlob*				m_pBlob;

	map<string, ConstantHandle>	m_mapHandle;


public:
	ID3DX11Effect* GetShader()
	{
		return m_pEffect;
	}

	ID3DBlob* GetBlob()
	{
		return m_pBlob;
	}

protected:
	virtual bool CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

public:
	bool CreateHandle(const char* pName, SHADER_CONSTANT_DATA_TYPE eType);
	ID3DX11EffectVariable* FindHandle(const string& strKey);
	PConstantHandle FindConstantHandle(const string& strKey);


	void SetData(const string& strKety, void* pData, int iSize = 1,int iArrayCount = 0);

public:
	virtual bool Create(TCHAR* pFileName = NULL, const string& strPathKey = TEXTURE_PATH_KEY);
	virtual void Render();
	virtual void Render(const DWORD& dwSubset);
	virtual void SetConstant();

public:
	CShader();
	virtual ~CShader();
};

BIGGEST_END
