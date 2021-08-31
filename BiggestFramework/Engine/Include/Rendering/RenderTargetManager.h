#pragma once
#include "../Core/Ref.h"

BIGGEST_BEGIN



class BIGGEST_DLL CRenderTargetManager :
	public CRef
{
private:
	map<string, class CRenderTarget*>		m_mapTarget;
	map<string, class CRenderTargetDepth*>	m_mapDepth;
	map<string, class CMultiRenderTarget*>	m_mapMRT;

public:

	CRenderTarget* CreateTarget(const string& strKey, int iWidth, int iHeight, DXGI_FORMAT fmt, XMFLOAT4 fClearColor);
	CRenderTargetDepth* CreateDepth(const string& strKey, int iWidth, int iHeight, DXGI_FORMAT fmt);

	CRenderTarget* FindRenderTarget(const string& strKey);
	CRenderTargetDepth* FindDepthTarget(const string& strKey);

	bool AddMRT(const string& strMRTKey, const string& strTargetKey);
	bool AddMRTDepth(const string& strMRTKey, const string& strDepthKey);
	CMultiRenderTarget* FindMRT(const string&strKey);

private:
	DECLARE_SINGLE(CRenderTargetManager)
};

BIGGEST_END