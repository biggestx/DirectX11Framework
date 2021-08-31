#pragma once
#include "Shader.h"

BIGGEST_BEGIN

class BIGGEST_DLL CVertexShader :
	public CShader
{
private:
	ID3D11VertexShader*		m_pVertexShader;

public:
	virtual ID3D11Buffer* GetShader()
	{
		return (ID3D11Buffer*)m_pVertexShader;
	}

public:
	virtual bool Create(TCHAR* pFileName = NULL, const string& strPathKey = TEXTURE_PATH_KEY);
	bool Create(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel);
	virtual void Render();
	virtual void Render(const DWORD& dwSubset);

public:
	CVertexShader();
	~CVertexShader();
};


BIGGEST_END