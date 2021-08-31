#pragma once
#include "../Core/Ref.h"


BIGGEST_BEGIN

class BIGGEST_DLL CRenderTarget :
	public CRef
{
	CREATE_INSTANCE_TAG(CRenderTarget)

private:
	string m_strTargetName;


	ID3D11Texture2D*	m_pTexture;
	ID3D11RenderTargetView* m_pRenderTargetView;
	ID3D11ShaderResourceView*	m_pResourceView;

	int m_iTargetIdx;
	XMFLOAT4 m_fClearColor;

#ifdef _DEBUG
	class CResBuffer* m_pResBuffer;
	XMFLOAT2 m_vPos;
	XMFLOAT2 m_vSize;

#endif // _DEBUG

public:
	string GetTargetName()
	{
		return m_strTargetName;
	}

	ID3D11Texture2D* GetTexture()
	{
		return m_pTexture;
	}

	ID3D11ShaderResourceView* GetResourceView()
	{
		return m_pResourceView;
	}
	
	ID3D11RenderTargetView* GetRenderTargetView()
	{
		return m_pRenderTargetView;
	}

public:
	void SetTargetName(string strName)
	{
		m_strTargetName = strName;
	}

public:
	bool Init();
	bool CreateTarget(int iWidth, int iHeight, DXGI_FORMAT fmt, XMFLOAT4 fClearColor);
	void ChangeTarget(int idx);
	void ResetTarget();
	void ClearTarget();

public:
	void SetDebugInfo(const XMFLOAT2& vPos, const XMFLOAT2& vSize);
	void Render();
public:
	CRenderTarget();
	~CRenderTarget();
};


BIGGEST_END