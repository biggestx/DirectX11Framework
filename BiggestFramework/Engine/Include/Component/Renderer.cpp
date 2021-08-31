#include "Renderer.h"
#include "../Rendering/Shader/Shader.h"
#include "../Resource/BaseResource.h"
#include "../Resource/Material.h"
#include "../Component/Camera.h"
#include "../GameObject/GameObject.h"
#include "../Component/Transform.h"
#include "../Scene/SceneManager.h"
#include "../Scene/Scene.h"
#include "../Core/Device.h"
#include "../Rendering/RenderManager.h"

USING_BIGGEST

CRenderer::CRenderer() :
m_pRes(NULL)
{
}


CRenderer::~CRenderer()
{
	SAFE_RELEASE(m_pRes);
	Safe_Release_VecList(m_materialList);
}

CRenderer::CRenderer(const CRenderer& pRenderer)
{
	m_eComType = CT_RENDERER;


	m_iRefCount = 0;




	m_pRes = pRenderer.m_pRes;
	m_pRes->AddRef();



	//m_matWVP = pRenderer.m_matWVP;

}

CComponent* CRenderer::Clone()
{
	return new CRenderer(*this);
}

bool CRenderer::Init()
{
	m_eComType = CT_RENDERER;

	return true;
}

void CRenderer::AddMaterial(CMaterial* pMaterial)
{
	m_materialList.push_back(pMaterial);
}

void  CRenderer::Update(float fTime)
{
	/*CCamera* pCamera = (CCamera*)SINGLE(CSceneManager)->GetCurrentScene()->GetMainCamera()->FindComponent(CT_CAMERA);

	XMMATRIX matWorld = _MATRIX(&m_pGameObject->GetTransform()->GetWorldMatrix());
	XMMATRIX matView = _MATRIX(&pCamera->GetViewMatrix());
	XMMATRIX matProj = _MATRIX(&pCamera->GetProjectionMatrix());
	XMMATRIX matWVP = matWorld * matView * matProj;

	XMStoreFloat4x4(&m_matWVP, matWVP);
	XMStoreFloat4x4(&m_matWorld, matWorld);
	XMStoreFloat4x4(&m_matView, matView);
	XMStoreFloat4x4(&m_matProj, matProj);*/

}

void  CRenderer::LateUpdate(float fTime)
{
}

void CRenderer::Render(float fTime)
{
	if (m_pRes)
	{
		if (m_materialList.size() > 0)
		{

			CTransform* pTransform = m_pGameObject->GetTransform();

			int iRenderMonde = SINGLE(CRenderManager)->GetRenderMode();

			CCamera* pCamera = (CCamera*)SINGLE(CSceneManager)->GetCurrentScene()->GetMainCamera()->FindComponent(CT_CAMERA);

			XMMATRIX matWorld = _MATRIX(&m_pGameObject->GetTransform()->GetWorldMatrix());
			XMMATRIX matView = _MATRIX(&pCamera->GetViewMatrix());
			XMMATRIX matProj = _MATRIX(&pCamera->GetProjectionMatrix());
			XMMATRIX matWV = XMMatrixMultiply(matWorld,matView);
			XMMATRIX matWVP = XMMatrixMultiply(matWV, matProj);

			for (list<CMaterial*>::iterator iter = m_materialList.begin(); iter != m_materialList.end(); ++iter)
			{
				CShader* pShader = (*iter)->GetShader();

				list<int>* pPassList = (*iter)->GetPassList();
				ID3DX11EffectTechnique* pTech = pShader->GetShader()->GetTechniqueByName((*iter)->GetTech());
				(*iter)->ConstantTable();

				pShader->SetData(SHADER_HANDLE_SHARE_MATRIX_WORLD, &matWorld);
				pShader->SetData(SHADER_HANDLE_SHARE_MATRIX_VIEW, &matView);
				pShader->SetData(SHADER_HANDLE_SHARE_MATRIX_PROJ, &matProj);
				pShader->SetData(SHADER_HANDLE_SHARE_MATRIX_WV, &matWV);
				pShader->SetData(SHADER_HANDLE_SHARE_MATRIX_WVP, &matWVP);

				for (list<int>::iterator iter2 = pPassList->begin(); iter2 != pPassList->end(); ++iter2)
				{
					pTech->GetPassByIndex((*iter2))->Apply(0, DEVICECONTEXT);
					m_pRes->Render();
				}

			}
			//for (int i = 0; i < m_tTechDesc.Passes; ++i)
			//{
			//	m_pTech->GetPassByIndex(i)->Apply(0, DEVICECONTEXT);
			//	m_pRes->Render();
			//}

		}

	}

}


void CRenderer::ConstantTable()
{
	/*CTransform* pTransform = m_pGameObject->GetTransform();

	int iRenderMonde = SINGLE(CRenderManager)->GetRenderMode();


	CCamera* pCamera = (CCamera*)SINGLE(CSceneManager)->GetCurrentScene()->GetMainCamera()->FindComponent(CT_CAMERA);
	if (pCamera)
	pCamera->ConstantTable(m_pShader);

	XMMATRIX matWorld = _MATRIX(&m_pGameObject->GetTransform()->GetWorldMatrix());
	XMMATRIX matView = _MATRIX(&pCamera->GetViewMatrix());
	XMMATRIX matProj = _MATRIX(&pCamera->GetProjectionMatrix());
	XMMATRIX matWVP = matWorld * matView * matProj;


	Matrix fMatWorld;
	Matrix fMatView;
	Matrix fMatProj;
	Matrix fMatWVP;

	XMStoreFloat4x4(&fMatWorld, matWorld);
	XMStoreFloat4x4(&fMatView, matView);
	XMStoreFloat4x4(&fMatProj, matProj);
	XMStoreFloat4x4(&fMatWVP, matWVP);


	m_pShader->SetData(SHADER_DEFAULT_MATRIX_WORLD, &fMatWorld);
	m_pShader->SetData(SHADER_DEFAULT_MATRIX_VIEW, &fMatView);
	m_pShader->SetData(SHADER_DEFAULT_MATRIX_PROJ, &fMatProj);
	m_pShader->SetData(SHADER_DEFAULT_MATRIX_WVP, &fMatWVP);*/

}



