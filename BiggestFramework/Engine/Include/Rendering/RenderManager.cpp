#include "RenderManager.h"
#include "../Core/Device.h"
#include "Shader/Shader.h"
#include "../GameObject/GameObject.h"
#include "../Component/Camera.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneManager.h"
#include "RenderTarget.h"
#include "RenderTargetDepth.h"
#include "RenderTargetManager.h"
#include "MultiRenderTarget.h"
#include "../Component/Light.h"
#include "../Resource/ResBuffer.h"
#include "../Component/DirectionalLight.h"
#include "../Component/PointLight.h"
#include "../Component/Transform.h"

USING_BIGGEST

DEFINITION_SINGLE(CRenderManager)

CRenderManager::CRenderManager() :
m_pFullScreenQuad(NULL),
m_pPointLightSphere(NULL)
{
}

CRenderManager::~CRenderManager()
{/*
 SAFE_RELEASE(m_pVB);
 SAFE_RELEASE(m_pIB);

 SAFE_RELEASE(m_pDeferredDir);
 SAFE_RELEASE(m_pDeferredPoint);*/

	//DESTROY(CRenderTargetManager);


	SAFE_RELEASE(m_pFullScreenQuad);
	SAFE_RELEASE(m_pPointLightSphere);

	Safe_Release_Map(m_mapShader);
	Safe_Release_Map(m_mapMRT);
}

bool CRenderManager::Init(RESOLUTION_TYPE eType)
{
	//m_eRSType = eType;
	m_eRenderMode = RM_DEFERRED;
	//m_eRenderMode = RM_FORWARD;



	if (!InitShader())
		return false;

	if (!InitRenderTarget())
		return false;

	if (!InitLightDirBuffer())
		return false;

	if (!InitPointLightMesh())
		return false;



	return true;
}

bool CRenderManager::InitLightDirBuffer()
{
	MeshData<VertexPT> meshData;

	VertexPT v0;
	v0.vPosition = XMFLOAT3(0.f, 0.f, 0.f);
	v0.vTex = XMFLOAT2(0.f, 0.f);
	meshData.Vertices.push_back(v0);

	VertexPT v1;
	v1.vPosition = XMFLOAT3(1.f, 0.f, 0.f);
	v1.vTex = XMFLOAT2(1.f, 0.f);
	meshData.Vertices.push_back(v1);

	VertexPT v2;
	v2.vPosition = XMFLOAT3(0.f, 1.f, 0.f);
	v2.vTex = XMFLOAT2(0.f, 1.f);
	meshData.Vertices.push_back(v2);

	VertexPT v3;
	v3.vPosition = XMFLOAT3(1.f, 1.f, 0.f);
	v3.vTex = XMFLOAT2(1.f, 1.f);
	meshData.Vertices.push_back(v3);

	meshData.Indices.push_back(0);
	meshData.Indices.push_back(1);
	meshData.Indices.push_back(3);

	meshData.Indices.push_back(0);
	meshData.Indices.push_back(3);
	meshData.Indices.push_back(2);

	CShader* pShader = FindShader(SHADER_FXKEY_DEFERRED);
	ID3DX11EffectTechnique* pTech = pShader->GetShader()->GetTechniqueByName(SHADER_TECH_DEFERREDFX_LIGHTACC_DIRECTIONAL);
	D3DX11_PASS_DESC passDesc;
	pTech->GetPassByIndex(0)->GetDesc(&passDesc);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,0,12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};


	m_pFullScreenQuad = CResBuffer::Create("DirectionalLightQuad");
	m_pFullScreenQuad->CreateLayout(layout, ARRAYSIZE(layout), passDesc.pIAInputSignature, passDesc.IAInputSignatureSize);
	m_pFullScreenQuad->CreateBuffer(meshData);


	return true;
}

bool CRenderManager::InitPointLightMesh()
{
	MeshData<VertexP> meshData;

	int iStackCount = 10;
	int iSliceCount = 10;

	float fPhiStep = XM_PI / iStackCount;
	float fThetaStep = 2.0f * XM_PI / iSliceCount;

	float fRadius = 0.5f;

	VertexP vTop;
	vTop.vPosition = XMFLOAT3(0.f, +fRadius, 0.f);

	VertexP vBottom;
	vBottom.vPosition = XMFLOAT3(0.f, -fRadius, 0.f);

	meshData.Vertices.push_back(vTop);

	for (int i = 1; i <= iStackCount -1; ++i)
	{
		float fPhi = i* fPhiStep;

		for (int j = 0; j <= iSliceCount; ++j)
		{
			float fTheta = j* fThetaStep;

			VertexP v;
			v.vPosition.x = fRadius * sinf(fPhi) * cosf(fTheta);
			v.vPosition.y = fRadius * cosf(fPhi);
			v.vPosition.z = fRadius * sinf(fPhi) * sinf(fTheta);

			meshData.Vertices.push_back(v);
		}

	}
	
	meshData.Vertices.push_back(vBottom);

	for (int i = 1; i <= iSliceCount; ++i)
	{
		meshData.Indices.push_back(0);
		meshData.Indices.push_back(i + 1);
		meshData.Indices.push_back(i);
	}

	int iBaseIdx = 1;
	int iRingVertexCount = iSliceCount + 1;

	for (int i = 0; i < iStackCount - 2; ++i)
	{
		for (int j = 0; j < iSliceCount; ++j)
		{
			meshData.Indices.push_back(iBaseIdx + i * iRingVertexCount + j);
			meshData.Indices.push_back(iBaseIdx + i * iRingVertexCount + j + 1);
			meshData.Indices.push_back(iBaseIdx + (i + 1) * iRingVertexCount + j);

			meshData.Indices.push_back(iBaseIdx + (i + 1) * iRingVertexCount + j);
			meshData.Indices.push_back(iBaseIdx + i * iRingVertexCount + j + 1);
			meshData.Indices.push_back(iBaseIdx + (i + 1)* iRingVertexCount + j + 1);
		}
	}

	int iBottomIdx = meshData.Indices.size() - 1;

	iBaseIdx = iBottomIdx - iRingVertexCount;

	for (int i = 0; i < iSliceCount; ++i)
	{
		meshData.Indices.push_back(iBottomIdx);
		meshData.Indices.push_back(iBaseIdx + i);
		meshData.Indices.push_back(iBaseIdx + i + 1);
	}

	CShader* pShader = FindShader(SHADER_FXKEY_DEFERRED);
	ID3DX11EffectTechnique* pTech = pShader->GetShader()->GetTechniqueByName(SHADER_TECH_DEFERREDFX_LIGHTACC_POINT);
	D3DX11_PASS_DESC passDesc;
	pTech->GetPassByIndex(0)->GetDesc(&passDesc);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};


	m_pPointLightSphere = CResBuffer::Create("PointLightSphere");
	m_pPointLightSphere->CreateLayout(layout, ARRAYSIZE(layout), passDesc.pIAInputSignature, passDesc.IAInputSignatureSize);
	m_pPointLightSphere->CreateBuffer(meshData);


	return true;
}

bool CRenderManager::InitRenderTarget()
{
	RESOLUTION rs = SINGLE(CDevice)->GetResolution();

	CMultiRenderTarget* pMRT = CMultiRenderTarget::Create("GBuffer");
	pMRT->AddRenderTarget("Albedo", rs.fWidth, rs.fHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, XMFLOAT4(0, 0, 0, 1));
	pMRT->AddRenderTarget("Normal", rs.fWidth, rs.fHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, XMFLOAT4(0, 0, 0, 1));
	pMRT->AddRenderTarget("Depth", rs.fWidth, rs.fHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, XMFLOAT4(0, 0, 0, 1));
	pMRT->AddRenderTarget("Specular", rs.fWidth, rs.fHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, XMFLOAT4(0, 0, 0, 1));
	pMRT->CreateDepthTarget("GBufferDepth", rs.fWidth, rs.fHeight, DXGI_FORMAT_D24_UNORM_S8_UINT);

#ifdef _DEBUG
	CRenderTarget* pTarget = pMRT->FindRenderTarget("Albedo");
	if (pTarget)
	{
		pTarget->SetDebugInfo(XMFLOAT2(0.f, 0.f), XMFLOAT2(100.f, 100.f));
		pTarget = NULL;
	}

	pTarget = pMRT->FindRenderTarget("Normal");
	if (pTarget)
	{
		pTarget->SetDebugInfo(XMFLOAT2(0.f, 100.f), XMFLOAT2(100.f, 100.f));
		pTarget = NULL;
	}

	pTarget = pMRT->FindRenderTarget("Depth");
	if (pTarget)
	{
		pTarget->SetDebugInfo(XMFLOAT2(0.f, 200.f), XMFLOAT2(100.f, 100.f));
		pTarget = NULL;
	}

	pTarget = pMRT->FindRenderTarget("Specular");
	if (pTarget)
	{
		pTarget->SetDebugInfo(XMFLOAT2(0.f, 300.f), XMFLOAT2(100.f, 100.f));
		pTarget = NULL;
	}

#endif
	ID3D11DepthStencilState* pDepthStencilState = NULL;

	D3D11_DEPTH_STENCIL_DESC depthDesc;
	ZeroMemory(&depthDesc, sizeof(depthDesc));
	depthDesc.DepthEnable = TRUE;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDesc.StencilEnable = FALSE;

	HRESULT hr = DEVICE->CreateDepthStencilState(&depthDesc, &pDepthStencilState);
	pMRT->SetDepthStencilState(pDepthStencilState);
	
	m_mapMRT.insert(make_pair("GBuffer", pMRT));

	pDepthStencilState = NULL;
	pMRT = NULL;
	pMRT = CMultiRenderTarget::Create("LightAcc");
	pMRT->AddRenderTarget("LightAccDif", rs.fWidth, rs.fHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, XMFLOAT4(0, 0, 0, 1));
	pMRT->AddRenderTarget("LightAccSpc", rs.fWidth, rs.fHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, XMFLOAT4(0, 0, 0, 1));
	pMRT->CreateDepthTarget("LightAccDepth", rs.fWidth, rs.fHeight, DXGI_FORMAT_D24_UNORM_S8_UINT);

#ifdef _DEBUG
	pTarget = pMRT->FindRenderTarget("LightAccDif");
	if (pTarget)
	{
		pTarget->SetDebugInfo(XMFLOAT2(100.f, 0.f), XMFLOAT2(100.f, 100.f));
		pTarget = NULL;
	}

	pTarget = pMRT->FindRenderTarget("LightAccSpc");
	if (pTarget)
	{
		pTarget->SetDebugInfo(XMFLOAT2(100.f, 100.f), XMFLOAT2(100.f, 100.f));
		pTarget = NULL;
	}
#endif

	ZeroMemory(&depthDesc, sizeof(depthDesc));
	depthDesc.DepthEnable = FALSE;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depthDesc.StencilEnable = FALSE;

	DEVICE->CreateDepthStencilState(&depthDesc, &pDepthStencilState);

	pMRT->SetDepthStencilState(pDepthStencilState);


	m_mapMRT.insert(make_pair("LightAcc", pMRT));


	return true;
}

void CRenderManager::AddRenderObject(CGameObject* pObject, RENDERING_FLAG eFlag)
{
	m_RenderList[eFlag].push_back(pObject);
}

void CRenderManager::Render(float fTime)
{
	CCamera* pCamera = (CCamera*)SINGLE(CSceneManager)->GetCurrentScene()->GetMainCamera()->FindComponent(CT_CAMERA);
	//CShader* pShader = FindShader(SHADER_FXKEY_SHARE);
	if (pCamera)
		pCamera->ConstantTable();

	switch (m_eRenderMode)
	{
	case RM_FORWARD:
		RenderForward(fTime);
		break;
	case RM_DEFERRED:
		RenderDeferred(fTime);
		break;
	}
}

void CRenderManager::RenderForward(float fTime)
{

	SINGLE(CDevice)->ClearRenderTargetView();


	for (int i = 0; i < RF_MAX; ++i)
	{
		/*switch (i)
		{
		case RF_NORMAL_OBJECT:
		m_RenderList[i].sort(SortNormalObject);
		break;
		case RF_ALPHA_OBJECT:
		m_RenderList[i].sort(SortAlphaObject);
		break;
		case RF_UI:
		m_RenderList[i].sort(SortUI);
		break;
		}*/

		list<CGameObject*>::iterator	iter = m_RenderList[i].begin();
		list<CGameObject*>::iterator	iterEnd = m_RenderList[i].end();

		for (; iter != iterEnd; ++iter)
		{
			(*iter)->Render(fTime);
		}

		m_RenderList[i].clear();
	}

	//CCamera* pCamera = (CCamera*)SINGLE(CSceneManager)->GetCurrentScene()->GetMainCamera()->FindComponent(CT_CAMERA);
	//CShader* pShader = FindShader(SHADER_FXKEY_UI);

	//XMMATRIX matProj = _MATRIX(&pCamera->GetProjectionMatrix());
	//XMMATRIX matView = _MATRIX(&pCamera->GetViewMatrix());

	//pShader->SetData(SHADER_HANDLE_SHARE_MATRIX_VIEW, &matView);
	//pShader->SetData(SHADER_HANDLE_SHARE_MATRIX_PROJ, &matProj);
	//

	map<string, CMultiRenderTarget*>::iterator iter = m_mapMRT.find("GBuffer");
	if (iter != m_mapMRT.end())
		iter->second->Render();


	SINGLE(CDevice)->GetSwapChain()->Present(0, 0);

}

void CRenderManager::RenderDeferred(float fTime)
{
	// Render GBuffer
	RenderGBuffer(fTime);

	// Render Light Accumulation Buffer
	RenderLightAcc(fTime);

	RenderCombine();

	// DEBUG
	map<string, CMultiRenderTarget*>::iterator iter = m_mapMRT.find("GBuffer");
	if (iter != m_mapMRT.end())
		iter->second->Render();

	iter = m_mapMRT.find("LightAcc");
	if (iter != m_mapMRT.end())
		iter->second->Render();
	//

	SINGLE(CDevice)->GetSwapChain()->Present(0, 0);
	
}

void CRenderManager::RenderGBuffer(float fTime)
{
	
	CMultiRenderTarget* pMrt = m_mapMRT.find("GBuffer")->second;

	pMrt->ClearRenderTargetAndDpeth();
	pMrt->ChangeRenderTargetAndDepth();
	

	list<CGameObject*>::iterator iter = m_RenderList[RF_NORMAL_OBJECT].begin();
	list<CGameObject*>::iterator iterEnd = m_RenderList[RF_NORMAL_OBJECT].end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter)->Render(fTime);
	}

	m_RenderList[RF_NORMAL_OBJECT].clear();


}

void CRenderManager::RenderLightAcc(float fTime)
{
	CMultiRenderTarget* pMrt = m_mapMRT.find("LightAcc")->second;

	pMrt->ClearRenderTargetAndDpeth();
	pMrt->ChangeRenderTargetAndDepth();

	CShader* pShader = FindShader(SHADER_FXKEY_DEFERRED);

	CRenderTarget* pTarget = NULL;
	

	CMultiRenderTarget* pGBuffer = m_mapMRT.find("GBuffer")->second;
	pTarget = pGBuffer->FindRenderTarget("Albedo");
	pShader->SetData(SHADER_HANDLE_DEFERREDFX_TEXTURE_GBUFFER_ALBEDO, pTarget->GetResourceView());

	pTarget = pGBuffer->FindRenderTarget("Normal");
	pShader->SetData(SHADER_HANDLE_DEFERREDFX_TEXTURE_GBUFFER_NORMAL, pTarget->GetResourceView());

	pTarget = pGBuffer->FindRenderTarget("Depth");
	pShader->SetData(SHADER_HANDLE_DEFERREDFX_TEXTURE_GBUFFER_DEPTH, pTarget->GetResourceView());

	pTarget = pGBuffer->FindRenderTarget("Specular");
	pShader->SetData(SHADER_HANDLE_DEFERREDFX_TEXTURE_GBUFFER_SPECULAR, pTarget->GetResourceView());

	RESOLUTION rs = SINGLE(CDevice)->GetResolution();

	CCamera* pCamera = (CCamera*)SINGLE(CSceneManager)->GetCurrentScene()->GetMainCamera()->FindComponent(CT_CAMERA);

	for (list<CLight*>::const_iterator iter = m_lightList.begin(); iter != m_lightList.end(); ++iter)
	{
		CLight* pLight = (*iter);

		pLight->SetShader(pShader);
		pLight->ConstantTable();

		switch (pLight->GetLightType())
		{
		case LT_DIR:
		{
			CDirectionalLight* pDirectionalLight = (CDirectionalLight*)pLight;

			XMFLOAT3 vDir = pDirectionalLight->GetDirection();
			XMFLOAT3 vLightColor = pDirectionalLight->GetLightColor();

			XMMATRIX matWorld = XMMatrixScaling(rs.fWidth, rs.fHeight, 1.f);
			XMMATRIX matProj = XMMatrixOrthographicOffCenterLH(0.f, rs.fWidth, rs.fHeight, 0.f, 0.f, 100.f);

			XMMATRIX matWVP = matWorld * matProj;

			pShader->SetData(SHADER_HANDLE_SHARE_MATRIX_WVP, &matWVP);

			ID3DX11EffectTechnique* pTech = pShader->GetShader()->GetTechniqueByName(SHADER_TECH_DEFERREDFX_LIGHTACC_DIRECTIONAL);
			pTech->GetPassByIndex(0)->Apply(0, DEVICECONTEXT);
			m_pFullScreenQuad->Render();
		}
			break;

		case LT_POINT:
		{
			CPointLight* pPointLight = (CPointLight*)pLight;
			XMFLOAT3 vLightPos = pPointLight->GetGameObject()->GetTransform()->GetWorldPos();
			XMFLOAT3 vLightColor = pPointLight->GetLightColor();
			float fRadius = pPointLight->GetRadius();

			XMMATRIX matView = _MATRIX(&pCamera->GetViewMatrix());
			XMMATRIX matInvView = XMMatrixInverse(&XMMatrixDeterminant(matView), matView);
			XMMATRIX matProj = _MATRIX(&pCamera->GetProjectionMatrix());

			XMMATRIX matWorld, matTrans, matScale;
			matTrans = XMMatrixTranslation(vLightPos.x, vLightPos.y, vLightPos.z);
			matScale = XMMatrixScaling(fRadius, fRadius, fRadius);
			
			matWorld = matScale * matTrans;

			XMMATRIX matWV,matWVP;
			matWV = matWorld * matView;
			matWVP = matWorld * matView * matProj;

			pShader->SetData(SHADER_HANDLE_SHARE_MATRIX_WV, &matWV);
			pShader->SetData(SHADER_HANDLE_SHARE_MATRIX_WVP, &matWVP);
			pShader->SetData(SHADER_HANDLE_SHARE_MATRIX_VIEW, &matView);
			pShader->SetData(SHADER_HANDLE_SHARE_MATRIX_INVERSEVIEW, &matInvView);

			ID3DX11EffectTechnique* pTech = pShader->GetShader()->GetTechniqueByName(SHADER_TECH_DEFERREDFX_LIGHTACC_POINT);
			pTech->GetPassByIndex(0)->Apply(0, DEVICECONTEXT);
			m_pPointLightSphere->Render();

		}
			break;
		}
	}

	m_lightList.clear();

}

void CRenderManager::RenderCombine()
{
	SINGLE(CDevice)->ClearRenderTargetView();
	SINGLE(CDevice)->SetMainRenderTarget();

	CShader* pShader = FindShader(SHADER_FXKEY_DEFERRED);

	map<string, CMultiRenderTarget*>::iterator iter = m_mapMRT.find("GBuffer");
	if (iter == m_mapMRT.end())
		return;

	CMultiRenderTarget* pGBufferMRT = iter->second;

	iter = m_mapMRT.find("LightAcc");
	if (iter == m_mapMRT.end())
		return;

	CMultiRenderTarget* pLightAccMRT = iter->second;

	pShader->SetData(SHADER_HANDLE_DEFERREDFX_TEXTURE_GBUFFER_ALBEDO, pGBufferMRT->FindRenderTarget("Albedo")->GetResourceView());
	pShader->SetData(SHADER_HANDLE_DEFERREDFX_TEXTURE_LIGHTACC_DIFFUSE, pLightAccMRT->FindRenderTarget("LightAccDif")->GetResourceView());
	pShader->SetData(SHADER_HANDLE_DEFERREDFX_TEXTURE_LIGHTACC_SPECULAR, pLightAccMRT->FindRenderTarget("LightAccSpc")->GetResourceView());


	RESOLUTION rs = SINGLE(CDevice)->GetResolution();

	XMMATRIX matWorld, matTrans, matScale;
	XMMATRIX matProj, matWVP;

	matTrans = XMMatrixTranslation(0.f, 0.f, 0.f);
	matScale = XMMatrixScaling(rs.fWidth, rs.fHeight, 1);
	matWorld = matScale * matTrans;

	matProj = XMMatrixOrthographicOffCenterLH(0.f, rs.fWidth, rs.fHeight, 0.f, 0.f, 1000.f);
	matWVP = matWorld * matProj;

	pShader->SetData(SHADER_HANDLE_SHARE_MATRIX_WVP, &matWVP);


	pShader->GetShader()->GetTechniqueByName(SHADER_TECH_DEFERREDFX_COMBINE)->GetPassByIndex(0)->Apply(0, DEVICECONTEXT);
	m_pFullScreenQuad->Render();

}

bool __cdecl CRenderManager::SortNormalObject(CGameObject* pObj1, CGameObject* pObj2)
{
	//// 트렌스폼을 얻어온다.
	//CTransform*	pTr1 = pObj1->GetTransform();
	//CTransform*	pTr2 = pObj2->GetTransform();

	//Vec3	vPos1 = pTr1->GetWorldPos();
	//Vec3	vPos2 = pTr2->GetWorldPos();

	//CRenderer*	pRd1 = (CRenderer*)pObj1->FindComponent(CT_RENDERER);
	//CRenderer*	pRd2 = (CRenderer*)pObj2->FindComponent(CT_RENDERER);

	//// 카메라를 얻어온다.
	//CCamera*	pCam1 = (CCamera*)pRd1->GetCameraObj()->FindComponent(CT_CAMERA);
	//CCamera*	pCam2 = (CCamera*)pRd2->GetCameraObj()->FindComponent(CT_CAMERA);

	//// 위치를 뷰 공간으로 변환한다.
	//Matrix	matView1 = pCam1->GetViewMatrix();
	//Matrix	matView2 = pCam2->GetViewMatrix();

	//D3DXVec3TransformCoord(&vPos1, &vPos1, &matView1);
	//D3DXVec3TransformCoord(&vPos2, &vPos2, &matView2);

	//if (vPos1.z >= vPos2.z)
	//	return false;

	return true;
}

bool __cdecl CRenderManager::SortAlphaObject(CGameObject* pObj1, CGameObject* pObj2)
{
	//// 트렌스폼을 얻어온다.
	//CTransform*	pTr1 = pObj1->GetTransform();
	//CTransform*	pTr2 = pObj2->GetTransform();

	//Vec3	vPos1 = pTr1->GetWorldPos();
	//Vec3	vPos2 = pTr2->GetWorldPos();

	//CRenderer*	pRd1 = (CRenderer*)pObj1->FindComponent(CT_RENDERER);
	//CRenderer*	pRd2 = (CRenderer*)pObj2->FindComponent(CT_RENDERER);

	//// 카메라를 얻어온다.
	//CCamera*	pCam1 = (CCamera*)pRd1->GetCameraObj()->FindComponent(CT_CAMERA);
	//CCamera*	pCam2 = (CCamera*)pRd2->GetCameraObj()->FindComponent(CT_CAMERA);

	//// 위치를 뷰 공간으로 변환한다.
	//Matrix	matView1 = pCam1->GetViewMatrix();
	//Matrix	matView2 = pCam2->GetViewMatrix();

	//D3DXVec3TransformCoord(&vPos1, &vPos1, &matView1);
	//D3DXVec3TransformCoord(&vPos2, &vPos2, &matView2);

	//if (vPos1.z <= vPos2.z)
	//	return false;

	return true;
}

bool __cdecl CRenderManager::SortUI(CGameObject* pObj1, CGameObject* pObj2)
{
	return true;
}

void CRenderManager::CreateShareHandle(CShader* pShader)
{
	pShader->CreateHandle(SHADER_HANDLE_SHARE_MATRIX_WORLD, SCDT_MATRIX);
	pShader->CreateHandle(SHADER_HANDLE_SHARE_MATRIX_VIEW, SCDT_MATRIX);
	pShader->CreateHandle(SHADER_HANDLE_SHARE_MATRIX_PROJ, SCDT_MATRIX);
	pShader->CreateHandle(SHADER_HANDLE_SHARE_MATRIX_WV, SCDT_MATRIX);
	pShader->CreateHandle(SHADER_HANDLE_SHARE_MATRIX_WVP, SCDT_MATRIX);
	pShader->CreateHandle(SHADER_HANDLE_SHARE_MATRIX_INVERSEVIEW, SCDT_MATRIX);
	pShader->CreateHandle(SHADER_HANDLE_SHARE_TEXTURE_DIFFUSE, SCDT_TEXTURE);
	pShader->CreateHandle(SHADER_HANDLE_SHARE_TEXTURE_NORMAL, SCDT_TEXTURE);
	pShader->CreateHandle(SHADER_HANDLE_SHARE_TEXTURE_EMISSIVE, SCDT_TEXTURE);
	pShader->CreateHandle(SHADER_HANDLE_SHARE_TEXTRUE_VTF, SCDT_TEXTURE);
	pShader->CreateHandle(SHADER_HANDLE_SHARE_INT_VTFTEXTUREWIDTH, SCDT_INT);
	pShader->CreateHandle(SHADER_HANDLE_SHARE_MATRIX_BONES, SCDT_MATRIX_ARRAY);
	pShader->CreateHandle(SHADER_HANDLE_SHARE_FLOAT3_LIGHTDIR, SCDT_FLOAT3);
	pShader->CreateHandle(SHADER_HANDLE_SHARE_FLOAT3_LIGHTPOS, SCDT_FLOAT3);
	pShader->CreateHandle(SHADER_HANDLE_SHARE_FLOAT3_LIGHTCOLOR, SCDT_FLOAT3);
	pShader->CreateHandle(SHADER_HANDLE_SHARE_FLOAT_LIGHTRADIUS, SCDT_FLOAT);
}

void CRenderManager::CreateTerrainHandle(CShader* pShader)
{
	CreateShareHandle(pShader);

	pShader->CreateHandle(SHADER_HANDLE_TERRAINFX_DIFFUSETEX, SCDT_TEXTURE);
	pShader->CreateHandle(SHADER_HANDLE_TERRAINFX_HEIGHTTEX, SCDT_TEXTURE);
}

void CRenderManager::CreateEffectHandle(CShader* pShader)
{/*
 CreateShareHandle(pShader);

 pShader->CreateHandle(SHADER_EFFECT_FRAMEX, SCDT_INT);
 pShader->CreateHandle(SHADER_EFFECT_FRAMEY, SCDT_INT);
 pShader->CreateHandle(SHADER_EFFECT_CELLSIZEX, SCDT_FLOAT);
 pShader->CreateHandle(SHADER_EFFECT_CELLSIZEY, SCDT_FLOAT);*/
}

void CRenderManager::CreateUIHandle(CShader* pShader)
{
	CreateShareHandle(pShader);
}


void CRenderManager::CreateLightHandle(CShader* pShader)
{
	/*
	CreateShareHandle(pShader);

	pShader->CreateHandle(SHADER_LIGHT_LIGHTWORLDMAT, SCDT_MATRIX);
	pShader->CreateHandle(SHADER_LIGHT_ALBEDOTEX, SCDT_TEXTURE);
	pShader->CreateHandle(SHADER_LIGHT_NORMALTEX, SCDT_TEXTURE);
	pShader->CreateHandle(SHADER_LIGHT_DEPTHTEX, SCDT_TEXTURE);
	pShader->CreateHandle(SHADER_LIGHT_DIFTEX, SCDT_TEXTURE);
	pShader->CreateHandle(SHADER_LIGHT_SPCTEX, SCDT_TEXTURE);
	*/

}


void CRenderManager::CreateDefaultHandle(CShader* pShader)
{
	CreateShareHandle(pShader);


}

void CRenderManager::CreateDeferredRenderingHandle(CShader* pShader)
{
	CreateShareHandle(pShader);

	pShader->CreateHandle(SHADER_HANDLE_DEFERREDFX_TEXTURE_GBUFFER_ALBEDO, SCDT_TEXTURE);
	pShader->CreateHandle(SHADER_HANDLE_DEFERREDFX_TEXTURE_GBUFFER_NORMAL, SCDT_TEXTURE);
	pShader->CreateHandle(SHADER_HANDLE_DEFERREDFX_TEXTURE_GBUFFER_DEPTH, SCDT_TEXTURE);
	pShader->CreateHandle(SHADER_HANDLE_DEFERREDFX_TEXTURE_GBUFFER_SPECULAR, SCDT_TEXTURE);

	pShader->CreateHandle(SHADER_HANDLE_DEFERREDFX_TEXTURE_LIGHTACC_DIFFUSE, SCDT_TEXTURE);
	pShader->CreateHandle(SHADER_HANDLE_DEFERREDFX_TEXTURE_LIGHTACC_SPECULAR, SCDT_TEXTURE);
}


bool CRenderManager::InitShader()
{
	// Load Share.fx
	CShader* pShader = LoadShader(SHADER_FXKEY_SHARE, L"Share.hlsl");
	if (!pShader)
		return false;
	CreateShareHandle(pShader);

	// Load Default.fx
	pShader = LoadShader(SHADER_FXKEY_DEFAULT, L"Default.hlsl");
	if (!pShader)
		return false;
	CreateDefaultHandle(pShader);

	pShader = LoadShader(SHADER_FXKEY_UI, L"UI.hlsl");
	if (!pShader)
		return false;
	CreateUIHandle(pShader);

	pShader = LoadShader(SHADER_FXKEY_DEFERRED, L"DeferredRendering.hlsl");
	if (!pShader)
		return false;
	CreateDeferredRenderingHandle(pShader);

	pShader = LoadShader(SHADER_FXKEY_TERRAIN, L"Terrain.hlsl");
	if (!pShader)
		return false;
	CreateTerrainHandle(pShader);

	return true;
}

CShader* CRenderManager::LoadShader(const string& strKey, wchar_t* pFileName)
{
	CShader*	pShader = FindShader(strKey);

	if (pShader)
		return pShader;

	pShader = new CShader;

	if (!pShader->Create(pFileName))
	{
		SAFE_RELEASE(pShader);
		return NULL;
	}

	//pShader->AddRef();

	m_mapShader.insert(make_pair(strKey, pShader));

	return pShader;
}

CShader* CRenderManager::FindShader(const string& strKey)
{
	map<string, CShader*>::iterator	iter = m_mapShader.find(strKey);

	if (iter == m_mapShader.end())
		return NULL;

	//iter->second->AddRef();

	return iter->second;
}
