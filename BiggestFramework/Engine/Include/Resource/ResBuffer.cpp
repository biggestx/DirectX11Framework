#include "ResBuffer.h"
#include "../Core/Device.h"


USING_BIGGEST

CResBuffer::CResBuffer() :
m_pInputLayout(NULL),
m_pVertexBuffer(NULL),
m_pIndexBuffer(NULL),
m_iVertexCount(0),
m_iIndexCount(0),
m_iVertexSize(0)
{
}


CResBuffer::~CResBuffer()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pInputLayout);
}

bool CResBuffer::CreateResource(TCHAR* pFileName, const string& strPathKey)
{
	return true;
}

bool CResBuffer::Init()
{
	m_RF = RF_RESBUFFER;

	return true;
}

bool CResBuffer::CreateLayout(D3D11_INPUT_ELEMENT_DESC* pInputElement, DWORD iNumElement, const void* pShaderByteCode, SIZE_T iBytecodeSize)
{
	HRESULT hr = S_OK;
	hr = DEVICE->CreateInputLayout(pInputElement, iNumElement, pShaderByteCode, iBytecodeSize, &m_pInputLayout);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Layout积己 角菩", L"", MB_OK);
		return false;
	}


	return true;
}

bool CResBuffer::CreateBuffer_SimpleVertex(PSimpleVertex pVertex, int iVertexSize)
{
	m_iVertexCount = iVertexSize;

	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * iVertexSize;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = pVertex;
	hr = DEVICE->CreateBuffer(&bd, &InitData, &m_pVertexBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Buffer_SimpleVertex 积己 角菩", L"", MB_OK);
		return false;
	}

}

bool CResBuffer::CreateInddex(WORD* pIndices, int iIndexCount)
{
	m_iIndexCount = iIndexCount;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * iIndexCount;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = pIndices;

	if (FAILED(DEVICE->CreateBuffer(&bd, &initData, &m_pIndexBuffer)))
	{
		MessageBox(NULL, L"CreateBufferFailed", L"", 0);
		return false;
	}



	return true;
}

void CResBuffer::Render()
{
	UINT stride = m_iVertexSize;
	UINT offset = 0;
	DEVICECONTEXT->IASetInputLayout(m_pInputLayout);
	DEVICECONTEXT->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	DEVICECONTEXT->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	DEVICECONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DEVICECONTEXT->DrawIndexed(m_iIndexCount, 0, 0);
}

void CResBuffer::Render(const DWORD& dwSubset)
{

}

