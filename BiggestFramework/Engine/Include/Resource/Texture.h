#pragma once
#include "BaseResource.h"

BIGGEST_BEGIN

class BIGGEST_DLL CTexture :
	public CBaseResource
{
	CREATE_INSTANCE_TAG(CTexture)
private:

	ID3D11Texture2D* m_pTexture;
	ID3D11ShaderResourceView* m_pResourceView;



public:
	ID3D11ShaderResourceView* GetResourceView()
	{
		return m_pResourceView;
	}
	ID3D11Texture2D* GetTexture2D()
	{
		return m_pTexture;
	}

public:
	bool CreateResourceFullPath(TCHAR* szFileFullPath);
	bool CreateResourceFullPath(char* szFileFullPath);

	bool CreateWriteableTexture(int iWidth, int iHeight);

public:
	bool Init();
	virtual bool CreateResource(TCHAR* pFileName = NULL, const string& strPathKey = TEXTURE_PATH_KEY);
	virtual void Render();
	virtual void Render(const DWORD& dwSubset);

public:
	CTexture();
	~CTexture();
};

BIGGEST_END