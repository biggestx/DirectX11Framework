#include "Device.h"

USING_BIGGEST

DEFINITION_SINGLE(CDevice)

CDevice::CDevice() :
m_hWnd(NULL),
m_pd3dDevice(NULL),
m_pImmediateContext(NULL),
m_driverType(D3D_DRIVER_TYPE_NULL),
m_featureLevel(D3D_FEATURE_LEVEL_11_0),
m_pSwapChain(NULL),
m_pRenderTargetView(NULL),
m_pBackBuffer(NULL)
{
#ifdef _DEBUG
	m_pDebug = NULL;
#endif

}


CDevice::~CDevice()
{
	
#ifdef _DEBUG
	SAFE_RELEASE(m_pDebug);
#endif


	// 역순으로 Release
	if(m_pImmediateContext !=  NULL)
		m_pImmediateContext->ClearState();


	SAFE_RELEASE(m_pDepthStencil);
	SAFE_RELEASE(m_pDepthStencilView);
	SAFE_RELEASE(m_pDepthStencil);
	
	SAFE_RELEASE(m_pRenderTargetView);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pBackBuffer);

	SAFE_RELEASE(m_pImmediateContext);
	SAFE_RELEASE(m_pd3dDevice);

}


bool CDevice::InitDevice(HWND hWnd, RESOLUTION rs)
{
	HRESULT hr = S_OK;

	m_hWnd = hWnd;
	m_tResolution = rs;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// 사용 가능한 드라이버 설정
	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	// Feature Level 설정. 3개만 등록해서 확인하기 때문에 Dx9보다 훨씬 빠르다.
	// Dx9은 처음부터 끝 버전 까지 순차적으로 확인
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_11_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels); // Array의 사이즈를 알 수 있는 매크로

	// Swap Chain 설정. 정보를 채운다.
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;					// 백버퍼 수
	sd.BufferDesc.Width = rs.fWidth;		// Width
	sd.BufferDesc.Height = rs.fHeight;		// Height
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// Format
	sd.BufferDesc.RefreshRate.Numerator = 60;			// 프레임 수(주사율) 
	sd.BufferDesc.RefreshRate.Denominator = 1;			// 1초에 60번
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;		// 표시 될 윈도우
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		m_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, m_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &m_pSwapChain, &m_pd3dDevice, &m_featureLevel, &m_pImmediateContext);
		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
		return false;

	// https://stackoverflow.com/questions/44334207/d3d11createdevice-0x887a002d-error

	// Create a render target view
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&m_pBackBuffer); //uuid = GUID 얻어온다.
	if (FAILED(hr))
		return false;

	hr = m_pd3dDevice->CreateRenderTargetView(m_pBackBuffer, NULL, &m_pRenderTargetView);
	if (FAILED(hr))
		return false;

	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = rs.fWidth;
	descDepth.Height = rs.fHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = m_pd3dDevice->CreateTexture2D(&descDepth, NULL, &m_pDepthStencil);
	if (FAILED(hr))
		return hr;

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = m_pd3dDevice->CreateDepthStencilView(m_pDepthStencil, &descDSV, &m_pDepthStencilView);
	if (FAILED(hr))
		return hr;

	m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)rs.fWidth;
	vp.Height = (FLOAT)rs.fHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_pImmediateContext->RSSetViewports(1, &vp);


	
#ifdef _DEBUG
	HRESULT result = DEVICE->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**> (&m_pDebug));
	result = m_pDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif


	return true;
}

void CDevice::SetMainRenderTarget()
{
	m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
}

void CDevice::ClearRenderTargetView()
{
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; 
	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, ClearColor);
	m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

}