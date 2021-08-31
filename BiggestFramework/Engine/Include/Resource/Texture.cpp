#include "Texture.h"
#include "../Core/Device.h"
#include "../Core/FilePathManager.h"

USING_BIGGEST

CTexture::CTexture() :
m_pResourceView(NULL),
m_pTexture(NULL)
{
}


CTexture::~CTexture()
{
	SAFE_RELEASE(m_pResourceView);
	SAFE_RELEASE(m_pTexture);
}

bool CTexture::Init()
{
	m_RF = RF_TEXTURE;

	return true;
}
bool CTexture::CreateResource(TCHAR* pFileName, const string& strPathKey)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	wstring strFilePath = SINGLE(CFilePathManager)->FindPath(strPathKey);

	strFilePath += pFileName;

	D3DX11CreateShaderResourceViewFromFile(DEVICE, strFilePath.c_str(), 0, 0, &m_pResourceView, 0);
	if (!m_pResourceView)
		return false;


	return true;
}

bool CTexture::CreateWriteableTexture(int iWidth, int iHeight)
{
	HRESULT hr;

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = iWidth;
	desc.Height = iHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DYNAMIC; // CPU에서 쓰기 가능
	desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	hr = DEVICE->CreateTexture2D(&desc, NULL, &m_pTexture);
	if (FAILED(hr))
		return false;

	hr = DEVICE->CreateShaderResourceView(m_pTexture, NULL, &m_pResourceView);
	if (FAILED(hr))
	{
		SAFE_RELEASE(m_pTexture);

		return false;
	}


	return true;
}

bool CTexture::CreateResourceFullPath(TCHAR* szFileFullPath)
{
	D3DX11CreateShaderResourceViewFromFile(DEVICE, szFileFullPath, 0, 0, &m_pResourceView, 0);
	if (!m_pResourceView)
		return false;

	return true;
}


bool CTexture::CreateResourceFullPath(char* szFileFullPath)
{
	TCHAR szPath[MAX_PATH] = { 0, };

	MultiByteToWideChar(CP_ACP, 0, szFileFullPath, -1, szPath, MAX_PATH);

	D3DX11CreateShaderResourceViewFromFile(DEVICE, szPath, 0, 0, &m_pResourceView, 0);
	if (!m_pResourceView)
		return false;

	return true;

}

void CTexture::Render()
{

}
void CTexture::Render(const DWORD& dwSubset)
{

}
