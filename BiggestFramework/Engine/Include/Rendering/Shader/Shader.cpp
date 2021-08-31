#include "Shader.h"
#include "../../Core/FilePathManager.h"
#include "../../Core/Device.h"

USING_BIGGEST


CShader::CShader() :
m_strFileName(L""),
m_strEntryPoint(""),
m_strShaderModel(""),
m_pBlob(NULL),
m_pEffect(NULL)
{

}


CShader::~CShader()
{
	SAFE_RELEASE(m_pBlob);
	SAFE_RELEASE(m_pEffect);
}

bool CShader::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	hr = D3DX11CompileFromFile(szFileName, NULL, NULL, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);

	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		if (pErrorBlob)
			pErrorBlob->Release();
		return false;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return true;
}

bool CShader::Create(TCHAR* pFileName, const string& strPathKey)
{
	HRESULT hr = S_OK;

	wstring strFullPath = SINGLE(CFilePathManager)->FindPath(SHADER_PATH_KEY);
	strFullPath += pFileName;

	ID3D10Blob* pErrorBlob;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	hr = D3DX11CompileFromFile(strFullPath.c_str(), 0, NULL, NULL, "fx_5_0", dwShaderFlags, 0, NULL, &m_pBlob, &pErrorBlob, 0);
	if (pErrorBlob != 0)
	{
		MessageBoxA(0, (char*)pErrorBlob->GetBufferPointer(), 0, 0);
		SAFE_RELEASE(pErrorBlob);
	}
	if (FAILED(hr))
	{
		MessageBox(0, L"Failed Compile FX", L"", 0);
		return false;
	}
	hr = D3DX11CreateEffectFromMemory(m_pBlob->GetBufferPointer(), m_pBlob->GetBufferSize(), 0, DEVICE, &m_pEffect);
	if (FAILED(hr))
	{
		MessageBox(0, L"Failed Create FX", L"", 0);
		return false;
	}

	
	return true;
}


bool CShader::CreateHandle(const char* pName, SHADER_CONSTANT_DATA_TYPE eType)
{
	if (!m_pEffect)
		return false;

	if (FindHandle(pName))
		return false;

	ConstantHandle tHandle;
	tHandle.pConstant = NULL;
	tHandle.pConstant = m_pEffect->GetVariableByName(pName);
	if (!tHandle.pConstant->IsValid())
		return false;


	tHandle.eType = eType;

	m_mapHandle.insert(make_pair(pName, tHandle));

	return true;
}

ID3DX11EffectVariable* CShader::FindHandle(const string& strKey)
{
	map<string, ConstantHandle>::iterator	iter = m_mapHandle.find(strKey);

	if (iter == m_mapHandle.end())
		return NULL;

	return iter->second.pConstant;
}

PConstantHandle CShader::FindConstantHandle(const string& strKey)
{
	map<string, ConstantHandle>::iterator	iter = m_mapHandle.find(strKey);

	if (iter == m_mapHandle.end())
		return NULL;

	return &iter->second;
}

void CShader::SetData(const string& strKey, void* pData, int iSize, int iArrayCount)
{
	PConstantHandle	pHandle = FindConstantHandle(strKey);

	if (!pHandle)
		return;

	// pHandle->pConstant->SetRawValue(pData, 0, sizeof(XMFLOAT4X4));
	// SetRawValue 사이즈를 지정해주어도 상수레지스터에 등록이 안돼서 변경

	switch (pHandle->eType)
	{
	case SCDT_BOOL:
		pHandle->pConstant->AsScalar()->SetBool(*(bool*)pData);
		break;

	case SCDT_FLOAT:
		pHandle->pConstant->AsScalar()->SetFloat(*(float*)pData);
		break;

	case SCDT_FLOAT_ARRAY:
		pHandle->pConstant->AsScalar()->SetFloatArray((float*)pData, 0, iArrayCount);
		break;

	case SCDT_FLOAT2:
	case SCDT_FLOAT3:
	case SCDT_FLOAT4:
		pHandle->pConstant->AsVector()->SetFloatVector((float*)pData);
		break;

	case SCDT_INT:
		pHandle->pConstant->AsScalar()->SetInt(*(int*)pData);
		break;
	case SCDT_MATRIX:
		pHandle->pConstant->AsMatrix()->SetMatrix((float*)pData);
		break;

	case SCDT_TEXTURE:
		pHandle->pConstant->AsShaderResource()->SetResource((ID3D11ShaderResourceView*)pData);
		break;
	case SCDT_MATRIX_ARRAY:
		pHandle->pConstant->AsMatrix()->SetMatrixArray((float*)pData, iSize, iArrayCount);
		break;
	}
}



void CShader::Render()
{
}
void CShader::Render(const DWORD& dwSubset)
{

}
void CShader::SetConstant()
{

}