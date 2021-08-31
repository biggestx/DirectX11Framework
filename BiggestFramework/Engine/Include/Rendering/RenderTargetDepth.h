#pragma once
#include "../Core/Ref.h"

BIGGEST_BEGIN

class BIGGEST_DLL CRenderTargetDepth :
	public CRef
{
	CREATE_INSTANCE_TAG(CRenderTargetDepth)
private:
	ID3D11Texture2D* m_pTexture;
	ID3D11DepthStencilView* m_pDepthStencilView;


public:
	ID3D11Texture2D* GetTexture()
	{
		return m_pTexture;
	}

	ID3D11DepthStencilView* GetDepthStencilView()
	{
		return m_pDepthStencilView;
	}

public:
	bool Init();
	bool CreateDepth(int iWidth, int iHeight, DXGI_FORMAT fmt);
	void ChangeDepth();
	void ResetDepth();
	void ClearDepth();

public:
	CRenderTargetDepth();
	~CRenderTargetDepth();
};

BIGGEST_END