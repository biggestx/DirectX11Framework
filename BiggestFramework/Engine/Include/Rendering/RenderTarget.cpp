#include "RenderTarget.h"
#include "../Core/Device.h"
#include "../Resource/ResBuffer.h"
#include "../Resource/ResourcesManager.h"
#include "Shader/Shader.h"
#include "RenderManager.h"
#include "../GameObject/GameObject.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneManager.h"
#include "../Component/Camera.h"

USING_BIGGEST

CRenderTarget::CRenderTarget() :
m_pTexture(NULL),
m_pResourceView(NULL),
m_pRenderTargetView(NULL)
{
#ifdef _DEBUG
	m_pResBuffer = NULL;
#endif

}


CRenderTarget::~CRenderTarget()
{

#ifdef _DEBUG
	SAFE_RELEASE(m_pResBuffer);
#endif
	SAFE_RELEASE(m_pResourceView);
	SAFE_RELEASE(m_pRenderTargetView);
	SAFE_RELEASE(m_pTexture);

}

bool CRenderTarget::Init()
{
	return true;
}

bool CRenderTarget::CreateTarget(int iWidth, int iHeight, DXGI_FORMAT fmt, XMFLOAT4 fClearColor)
{

	m_fClearColor = fClearColor;

	HRESULT hr = S_OK;


	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = iWidth;
	textureDesc.Height = iHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = fmt;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	hr = DEVICE->CreateTexture2D(&textureDesc, NULL, &m_pTexture);
	if (FAILED(hr))
		return false;

	D3D11_RENDER_TARGET_VIEW_DESC rendertargetDesc;
	rendertargetDesc.Format = textureDesc.Format;
	rendertargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rendertargetDesc.Texture2D.MipSlice = 0;

	hr = DEVICE->CreateRenderTargetView(m_pTexture, &rendertargetDesc, &m_pRenderTargetView);
	if (FAILED(hr))
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderresourceDesc;
	shaderresourceDesc.Format = textureDesc.Format;
	shaderresourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderresourceDesc.Texture2D.MostDetailedMip = 0;
	shaderresourceDesc.Texture2D.MipLevels = 1;

	hr = DEVICE->CreateShaderResourceView(m_pTexture, &shaderresourceDesc, &m_pResourceView);
	if (FAILED(hr))
		return false;
	

	return true;
}

void CRenderTarget::ChangeTarget(int idx)
{
	if (m_iTargetIdx != -1)
		return;
	m_iTargetIdx = idx;

	DEVICECONTEXT->OMSetRenderTargets(idx, &m_pRenderTargetView,NULL);

}
void CRenderTarget::ResetTarget()
{
	if (m_iTargetIdx == -1)
		return;

	DEVICECONTEXT->OMSetRenderTargets(m_iTargetIdx, &m_pRenderTargetView,NULL);

	m_iTargetIdx = -1;
}

void CRenderTarget::ClearTarget()
{
	float fClearColor[4] = { m_fClearColor.x, m_fClearColor.y, m_fClearColor.y, m_fClearColor.w };
	DEVICECONTEXT->ClearRenderTargetView(m_pRenderTargetView, fClearColor);
}

void CRenderTarget::SetDebugInfo(const XMFLOAT2& vPos, const XMFLOAT2& vSize)
{
#ifdef _DEBUG

	m_vPos = XMFLOAT2(vPos.x,vPos.y);
	m_vSize = XMFLOAT2(vSize.x,vSize.y);

	MeshData<VertexUI> meshData;

	VertexUI v0;
	v0.vPosition.x = 0.f;
	v0.vPosition.y = 0.f;
	v0.vPosition.z = 0.f;
	v0.vTex = XMFLOAT2(0.f, 0.f);

	meshData.Vertices.push_back(v0);

	VertexUI v1;
	v1.vPosition.x = 1.f;
	v1.vPosition.y = 0.f;
	v1.vPosition.z = 0.f;
	v1.vTex = XMFLOAT2(1.f, 0.f);

	meshData.Vertices.push_back(v1);

	VertexUI v2;
	v2.vPosition.x = 0.f;
	v2.vPosition.y = 1.f;
	v2.vPosition.z = 0.f;
	v2.vTex = XMFLOAT2(0.f, 1.f);

	meshData.Vertices.push_back(v2);


	VertexUI v3;
	v3.vPosition.x = 1.f;
	v3.vPosition.y = 1.f;
	v3.vPosition.z = 0.f;
	v3.vTex = XMFLOAT2(1.f, 1.f);

	meshData.Vertices.push_back(v3);


	meshData.Indices.push_back(0);
	meshData.Indices.push_back(1);
	meshData.Indices.push_back(3);

	meshData.Indices.push_back(0);
	meshData.Indices.push_back(3);
	meshData.Indices.push_back(2);



	ID3DX11EffectTechnique* pTech = SINGLE(CRenderManager)->FindShader(SHADER_FXKEY_UI)->GetShader()->GetTechniqueByName(SHADER_TECH_UIFX_DEFAULT);
	D3DX11_PASS_DESC passDesc;
	pTech->GetPassByIndex(0)->GetDesc(&passDesc);

	m_pResBuffer = CResBuffer::Create("DebugRenderTarget");

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	//D3D11_INPUT_ELEMENT_DESC layout[] =
	//{
	//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//};


	m_pResBuffer->CreateLayout(layout, ARRAYSIZE(layout), passDesc.pIAInputSignature, passDesc.IAInputSignatureSize);
	m_pResBuffer->CreateBuffer(meshData);

#endif
}


void CRenderTarget::Render()
{
#ifdef _DEBUG
	RESOLUTION rs = SINGLE(CDevice)->GetResolution();
	if (!m_pResBuffer)
		return;

	CShader* pShader = SINGLE(CRenderManager)->FindShader(SHADER_FXKEY_UI);
	
	XMMATRIX matWorld, matTrans, matScale;

	matTrans = XMMatrixTranslation(m_vPos.x, m_vPos.y,0.f);
	matScale = XMMatrixScaling(m_vSize.x,m_vSize.y, 1.f);
	
	matWorld = matScale * matTrans;

	CCamera* pCamera = (CCamera*)SINGLE(CSceneManager)->GetCurrentScene()->GetMainCamera()->FindComponent(CT_CAMERA);
	XMMATRIX matView = XMMatrixIdentity();//_MATRIX(&pCamera->GetViewMatrix());
	//XMMATRIX matProj = _MATRIX(&pCamera->GetProjectionMatrix());
	XMMATRIX matProj = XMMatrixOrthographicOffCenterLH(0.f, rs.fWidth, rs.fHeight,0.f, 0.f, 1000.f);

	XMMATRIX matWV, matWVP;

	matWV = matWorld * matView;
	matWVP = matWorld * matView * matProj;

	pShader->SetData(SHADER_HANDLE_SHARE_TEXTURE_DIFFUSE, m_pResourceView);
	pShader->SetData(SHADER_HANDLE_SHARE_MATRIX_WORLD, &matWorld);
	pShader->SetData(SHADER_HANDLE_SHARE_MATRIX_VIEW, &matView);
	pShader->SetData(SHADER_HANDLE_SHARE_MATRIX_PROJ, &matProj);
	pShader->SetData(SHADER_HANDLE_SHARE_MATRIX_WVP, &matWVP);
	

	ID3DX11EffectTechnique* pTech = pShader->GetShader()->GetTechniqueByName(SHADER_TECH_UIFX_DEFAULT);
	pTech->GetPassByIndex(0)->Apply(0, DEVICECONTEXT);
	m_pResBuffer->Render();


#endif
}