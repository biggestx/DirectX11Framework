#pragma once

#include "Ref.h"

BIGGEST_BEGIN

class BIGGEST_DLL CDevice :
	public CRef
{
#ifdef _DEBUG
	ID3D11Debug*			m_pDebug;
#endif	

private:
	HWND					m_hWnd;
	ID3D11Device*			m_pd3dDevice;
	ID3D11DeviceContext*	m_pImmediateContext;

	D3D_DRIVER_TYPE			m_driverType;
	D3D_FEATURE_LEVEL		m_featureLevel;

	ID3D11Texture2D*		m_pBackBuffer;	

	IDXGISwapChain*			m_pSwapChain;
	ID3D11RenderTargetView*	m_pRenderTargetView;

	ID3D11Texture2D*        m_pDepthStencil;
	ID3D11DepthStencilView* m_pDepthStencilView;

	RESOLUTION				m_tResolution;

public:
	RESOLUTION GetResolution()
	{
		return m_tResolution;
	}

public:
	ID3D11Device*	GetDevice()
	{
		return m_pd3dDevice;
	}
	ID3D11DeviceContext* GetDeviceContext()
	{
		return m_pImmediateContext;
	}

	IDXGISwapChain* GetSwapChain()
	{
		return m_pSwapChain;
	}

public:
	bool InitDevice(HWND hWnd, RESOLUTION rs);
	void SetMainRenderTarget();
	void ClearRenderTargetView();

	DECLARE_SINGLE(CDevice)

};

BIGGEST_END
