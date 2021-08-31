#include "RenderTargetManager.h"
#include "RenderTarget.h"
#include "RenderTargetDepth.h"
#include "MultiRenderTarget.h"

USING_BIGGEST

DEFINITION_SINGLE(CRenderTargetManager)

CRenderTargetManager::CRenderTargetManager()
{
}


CRenderTargetManager::~CRenderTargetManager()
{
	Safe_Release_Map(m_mapDepth);
	Safe_Release_Map(m_mapTarget);

	map<string, CMultiRenderTarget*>::iterator iter;

	for (iter = m_mapMRT.begin(); iter != m_mapMRT.end(); ++iter)
	{
		iter->second->Release();
	}

	Safe_Delete_Map(m_mapMRT);

}


CRenderTarget* CRenderTargetManager::CreateTarget(const string& strKey, int iWidth, int iHeight, DXGI_FORMAT fmt, XMFLOAT4 fClearColor)
{
	CRenderTarget* pTarget = FindRenderTarget(strKey);

	if (pTarget)
		return pTarget;

	pTarget = CRenderTarget::Create(strKey);

	if (!pTarget->CreateTarget(iWidth, iHeight, fmt,fClearColor))
	{
		SAFE_RELEASE(pTarget);
		return NULL;
	}
	
	m_mapTarget.insert(make_pair(strKey, pTarget));

	return pTarget;
}


CRenderTargetDepth* CRenderTargetManager::CreateDepth(const string& strKey, int iWidth, int iHeight, DXGI_FORMAT fmt)
{
	CRenderTargetDepth* pDepth = FindDepthTarget(strKey);

	if (pDepth)
		return pDepth;


	pDepth = CRenderTargetDepth::Create(strKey);
	
	if (!pDepth->CreateDepth(iWidth, iHeight, fmt))
	{
		SAFE_RELEASE(pDepth);
		return NULL;
	}
	
	m_mapDepth.insert(make_pair(strKey, pDepth));

	return pDepth;
}

CRenderTarget* CRenderTargetManager::FindRenderTarget(const string& strKey)
{
	map<string, CRenderTarget*>::iterator iter = m_mapTarget.find(strKey);
	if (iter == m_mapTarget.end())
		return NULL;

	return iter->second;
}

CRenderTargetDepth* CRenderTargetManager::FindDepthTarget(const string& strKey)
{
	map<string, CRenderTargetDepth*>::iterator iter = m_mapDepth.find(strKey);
	if (iter == m_mapDepth.end())
		return NULL;

	return iter->second;
}



bool CRenderTargetManager::AddMRT(const string& strMRTKey, const string& strTargetKey)
{
	CMultiRenderTarget* pMRT = FindMRT(strMRTKey);

	if (!pMRT)
	{
		pMRT = new CMultiRenderTarget;

		m_mapMRT.insert(make_pair(strMRTKey, pMRT));
	}

	// Dx11에서는 RenderTarget을 8개까지 사용 가능
	if (pMRT->GetRenderTargetCount() == 8)
		return false;

	CRenderTarget* pTarget = FindRenderTarget(strTargetKey);

	//pMRT->vecTarget.push_back(pTarget);
	

	return true;
}
bool CRenderTargetManager::AddMRTDepth(const string& strMRTKey, const string& strDepthKey)
{
	CMultiRenderTarget* pMRT = FindMRT(strMRTKey);

	/*if (!pMRT)
	{
		pMRT = new CMultiRenderTarget;
		pMRT->pDepth = NULL;

		m_mapMRT.insert(make_pair(strMRTKey, pMRT));
	}

	CRenderTargetDepth* pDepth = FindDepthTarget(strDepthKey);

	pMRT->pDepth = pDepth;*/

	return true;
}
CMultiRenderTarget* CRenderTargetManager::FindMRT(const string&strKey)
{
	map<string, CMultiRenderTarget*>::iterator iter = m_mapMRT.find(strKey);
	if (iter == m_mapMRT.end())
		return NULL;

	return iter->second;
}

