#pragma once
#include "BaseResource.h"
#include "../Core/Device.h"

BIGGEST_BEGIN

class BIGGEST_DLL CResBuffer :
	public CBaseResource
{
	CREATE_INSTANCE_TAG(CResBuffer)
private:
	ID3D11InputLayout*	m_pInputLayout;

private:
	ID3D11Buffer* m_pVertexBuffer;
	int m_iVertexCount;
	int m_iVertexSize;

private:
	ID3D11Buffer* m_pIndexBuffer;
	int m_iIndexCount;

private:
	vector<Bone> m_boneList;

private:


public:
	bool Init();
	virtual bool CreateResource(TCHAR* pFileName = NULL, const string& strPathKey = TEXTURE_PATH_KEY);

	template <typename T>
	bool CreateBuffer(MeshData<T>& meshData, D3D11_USAGE eVertexUsage = D3D11_USAGE_DEFAULT, D3D11_USAGE eIndexUsage = D3D11_USAGE_DEFAULT)
	{
		m_iVertexCount = meshData.Vertices.size();
		m_iVertexSize = sizeof(T);

		HRESULT hr = S_OK;

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = eVertexUsage;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.ByteWidth = sizeof(T) * m_iVertexCount;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA vInitData;
		ZeroMemory(&vInitData, sizeof(vInitData));
		vInitData.pSysMem = &meshData.Vertices[0];
		hr = DEVICE->CreateBuffer(&bd, &vInitData, &m_pVertexBuffer);
		if (FAILED(hr))
		{
			MessageBox(NULL, L"Buffer 생성 실패", L"", MB_OK);
			return false;
		}

		m_iIndexCount = meshData.Indices.size();

		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = eIndexUsage;
		bd.ByteWidth = sizeof(UINT) * m_iIndexCount;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA iInitData;
		iInitData.pSysMem = &meshData.Indices[0];

		if (FAILED(DEVICE->CreateBuffer(&bd, &iInitData, &m_pIndexBuffer)))
		{
			MessageBox(NULL, L"Failed CreateBuffer", L"", 0);
			return false;
		}

		return true;
	}


	//bool Create(D3D11_INPUT_ELEMENT_DESC* pInputElement, DWORD iNumElement, const void* pShaderByteCode, SIZE_T iBytecodeSize);
	virtual void Render();
	virtual void Render(const DWORD& dwSubset);

public:
	bool CreateLayout(D3D11_INPUT_ELEMENT_DESC* pInputElement, DWORD iNumElement, const void* pShaderByteCode, SIZE_T iBytecodeSize);
	bool CreateBuffer_SimpleVertex(PSimpleVertex pVertex, int iVertexSize);
	bool CreateInddex(WORD* pIndices, int iIndexCount);

public:
	CResBuffer();
	virtual ~CResBuffer();
};

BIGGEST_END