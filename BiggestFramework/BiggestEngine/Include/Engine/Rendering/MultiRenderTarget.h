#pragma once
#include "../Core/Ref.h"

BIGGEST_BEGIN

class BIGGEST_DLL CMultiRenderTarget :
	public CRef
{
	CREATE_INSTANCE_TAG(CMultiRenderTarget)

private:
	vector<class CRenderTarget*> m_vecRenderTarget;
	class CRenderTargetDepth*	m_pDepthTarget;

	ID3D11DepthStencilState*	m_pDepthStencilState;

public:
	void SetDepthStencilState(ID3D11DepthStencilState* pState)
	{
		m_pDepthStencilState = pState;
	}

public:
	ID3D11DepthStencilState* GetDepthStencilState()
	{
		return m_pDepthStencilState;
	}

public:
	int GetRenderTargetCount()
	{
		return m_vecRenderTarget.size();
	}

public:
	bool Init();

	void AddRenderTarget(const string& strKey,int iWidth, int iHeight, DXGI_FORMAT fmt, XMFLOAT4 fClearColor);
	void CreateDepthTarget(const string& strKey,int iWidth, int iHeight, DXGI_FORMAT fmt);

	void ChangeRenderTargetAndDepth();
	void ClearRenderTargetAndDpeth();

	CRenderTarget* FindRenderTarget(const string& strKey);

public:
	void Render();

public:
	CMultiRenderTarget();
	~CMultiRenderTarget();
};

BIGGEST_END