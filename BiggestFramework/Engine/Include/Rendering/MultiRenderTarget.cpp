#include "MultiRenderTarget.h"
#include "RenderTarget.h"
#include "RenderTargetDepth.h"

#include "../Core/Device.h"

USING_BIGGEST

CMultiRenderTarget::CMultiRenderTarget() : 
m_pDepthTarget(NULL),
m_pDepthStencilState(NULL)
{
}


CMultiRenderTarget::~CMultiRenderTarget()
{
	Safe_Release_VecList(m_vecRenderTarget);
	SAFE_RELEASE(m_pDepthTarget);
	SAFE_RELEASE(m_pDepthStencilState);
}

bool CMultiRenderTarget::Init()
{
	return true;
}

void CMultiRenderTarget::AddRenderTarget(const string& strKey,int iWidth, int iHeight, DXGI_FORMAT fmt, XMFLOAT4 fClearColor)
{
	CRenderTarget* pRenderTarget = FindRenderTarget(strKey);

	if (pRenderTarget)
		return;

	pRenderTarget = CRenderTarget::Create(strKey);
	pRenderTarget->SetTargetName(strKey);
	pRenderTarget->CreateTarget(iWidth, iHeight, fmt, fClearColor);

	m_vecRenderTarget.push_back(pRenderTarget);
}
void CMultiRenderTarget::CreateDepthTarget(const string& strKey, int iWidth, int iHeight, DXGI_FORMAT fmt)
{
	if (m_pDepthTarget)
		return;

	m_pDepthTarget = CRenderTargetDepth::Create(strKey);
	m_pDepthTarget->CreateDepth(iWidth, iHeight, fmt);
}

void CMultiRenderTarget::ChangeRenderTargetAndDepth()
{
	if (m_pDepthStencilState)
		DEVICECONTEXT->OMSetDepthStencilState(m_pDepthStencilState,0);

	vector<ID3D11RenderTargetView*> vecRenderTarget;
	for (int i = 0; i < m_vecRenderTarget.size(); ++i)
		vecRenderTarget.push_back(m_vecRenderTarget[i]->GetRenderTargetView());

	DEVICECONTEXT->OMSetRenderTargets(m_vecRenderTarget.size(), &vecRenderTarget[0], m_pDepthTarget->GetDepthStencilView());
	
}

void CMultiRenderTarget::ClearRenderTargetAndDpeth()
{
	for (int i = 0; i < m_vecRenderTarget.size(); ++i)
		m_vecRenderTarget[i]->ClearTarget();

	m_pDepthTarget->ClearDepth();
}

CRenderTarget* CMultiRenderTarget::FindRenderTarget(const string& strKey)
{
	for (int i = 0; i < m_vecRenderTarget.size(); ++i)
	{
		if (strKey == m_vecRenderTarget[i]->GetTargetName())
			return m_vecRenderTarget[i];
	}

	return NULL;
}

void CMultiRenderTarget::Render()
{
#ifdef _DEBUG
	for (int i = 0; i < m_vecRenderTarget.size(); ++i)
		m_vecRenderTarget[i]->Render();
#endif
}