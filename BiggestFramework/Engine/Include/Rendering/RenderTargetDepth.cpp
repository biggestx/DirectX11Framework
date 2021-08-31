#include "RenderTargetDepth.h"
#include "../Core/Device.h"

USING_BIGGEST

CRenderTargetDepth::CRenderTargetDepth():
m_pTexture(NULL),
m_pDepthStencilView(NULL)
{
}


CRenderTargetDepth::~CRenderTargetDepth()
{
	SAFE_RELEASE(m_pDepthStencilView);
	SAFE_RELEASE(m_pDepthStencilView);
}

bool CRenderTargetDepth::Init()
{
	return true;
}

bool CRenderTargetDepth::CreateDepth(int iWidth, int iHeight, DXGI_FORMAT fmt)
{
	HRESULT hr = S_OK;

	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = iWidth;
	descDepth.Height = iHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = DEVICE->CreateTexture2D(&descDepth, NULL, &m_pTexture);
	if (FAILED(hr))
		return false;

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = DEVICE->CreateDepthStencilView(m_pTexture, &descDSV, &m_pDepthStencilView);
	if (FAILED(hr))
		return false;



	return true;
}


void CRenderTargetDepth::ChangeDepth()
{
	DEVICECONTEXT->OMSetRenderTargets(0, NULL, m_pDepthStencilView);
}

void CRenderTargetDepth::ResetDepth()
{

}

void CRenderTargetDepth::ClearDepth()
{
	DEVICECONTEXT->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}




