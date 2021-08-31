#pragma once
#include "Shader.h"

BIGGEST_BEGIN

class BIGGEST_DLL CEffectFX :
	public CShader
{
private:
	ID3DX11Effect*	m_pEffectFX;
	ID3DBlob*		m_pBlob;

public:
	ID3DX11Effect* GetEffect()
	{
		return m_pEffectFX;
	}

public:
	virtual bool Create(TCHAR* pFileName = NULL, const string& strPathKey = TEXTURE_PATH_KEY);
	bool Create(TCHAR* szFileName, char* szShaderVersion, UINT flag1);
	virtual void Render();
	virtual void Render(const DWORD& dwSubset);

public:
	CEffectFX();
	virtual ~CEffectFX();
};

BIGGEST_END