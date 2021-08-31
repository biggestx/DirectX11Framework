#include "Scene.h"
#include "../GameObject/GameObject.h"
#include "Layer.h"
#include "../Script/Script.h"
#include "../Component/Transform.h"
#include "../Component/Camera.h"
#include "../Rendering/RenderManager.h"

USING_BIGGEST

CScene::CScene() :
m_pCamera(NULL),
m_pUICamera(NULL),
m_pScheduler(NULL),
m_pObjectPool(NULL),
m_pResourcesManager(NULL)
{
}

CScene::~CScene()
{
	Safe_Release_VecList(m_LightList);
	//SAFE_RELEASE(m_pScheduler);

	//SAFE_RELEASE(m_pCamera);
	//SAFE_RELEASE(m_pUICamera);

	Safe_Release_Map(m_mapCamera);
	Safe_Release_VecList(m_vecLayer);
	Safe_Release_VecList(m_vecScript);
	//SAFE_RELEASE(m_pResourcesManager);
	//SAFE_RELEASE(m_pObjectPool);
}

bool CScene::Init()
{
	//// 오브젝트 풀 생성
	//m_pObjectPool = new CGameObjectPool;

	//if (!m_pObjectPool->Init())
	//	return false;

	//m_pObjectPool->SetScene(this);

	//// 리소스 관리자 생성
	//m_pResourcesManager = new CResourcesManager;

	//m_pResourcesManager->SetScene(this);

	// 기본 레이어 생성
	CreateLayer();

	//// Scheduler 생성
	//m_pScheduler = new CScheduler;
	//if (!m_pScheduler->Init())
	//	return false;

	// 기본 장면 카메라 생성
	m_pCamera = CreateCamera("MainCamera", CT_PERSPECTIVE,
		"MainCamera", Vec3(0.f, 0.f, -5.f));


	// UI 카메라 생성
	m_pUICamera = CreateCamera("UICamera", CT_ORTHOGONAL, "UICamera");

	// 전역조명 생성
	//CGameObject*	pLight = CreateLight(LT_DIR, "GlobalLight");

	//CTransform*	pLightTr = pLight->GetTransform();
	//pLightTr->SetWorldRotation(D3DX_PI / 4.f, D3DX_PI / 2.f, 0.f);

	//SAFE_RELEASE(pLight);

	return true;
}

void CScene::Update(float fTime)
{
	//m_pScheduler->Update(fTime);


	vector<CScript*>::iterator	iterSc = m_vecScript.begin();
	vector<CScript*>::iterator	iterScEnd = m_vecScript.end();

	for (; iterSc != iterScEnd; ++iterSc)
	{
		(*iterSc)->Update(fTime);
	}

	vector<CLayer*>::iterator	iterEnd = m_vecLayer.end();
	for (vector<CLayer*>::iterator iter = m_vecLayer.begin(); iter != iterEnd; ++iter)
	{
		(*iter)->Update(fTime);
	}

	m_pCamera->Update(fTime);

	//// 조명연산
	//list<CGameObject*>::iterator	iterLight = m_LightList.begin();
	//list<CGameObject*>::iterator	iterLightEnd = m_LightList.end();

	//for (; iterLight != iterLightEnd; ++iterLight)
	//{
	//	(*iterLight)->Update(fTime);
	//}
}

void CScene::LateUpdate(float fTime)
{
	m_pCamera->LateUpdate(fTime);

	vector<CScript*>::iterator	iterSc = m_vecScript.begin();
	vector<CScript*>::iterator	iterScEnd = m_vecScript.end();

	for (; iterSc != iterScEnd; ++iterSc)
	{
		(*iterSc)->LateUpdate(fTime);
	}

	vector<CLayer*>::iterator	iterEnd = m_vecLayer.end();
	for (vector<CLayer*>::iterator iter = m_vecLayer.begin(); iter != iterEnd; ++iter)
	{
		(*iter)->LateUpdate(fTime);
	}

	// 조명연산
	/*list<CGameObject*>::iterator	iterLight = m_LightList.begin();
	list<CGameObject*>::iterator	iterLightEnd = m_LightList.end();

	for (; iterLight != iterLightEnd; ++iterLight)
	{
	(*iterLight)->LateUpdate(fTime);
	}*/
	//CCamera*	pCamera = (CCamera*)m_pCamera->FindComponent(CT_CAMERA);
	//pCamera->SetDevice();
}

void CScene::AddCollider()
{
	vector<CLayer*>::iterator	iterEnd = m_vecLayer.end();
	for (vector<CLayer*>::iterator iter = m_vecLayer.begin(); iter != iterEnd; ++iter)
	{
		(*iter)->AddCollider();
	}
}

CLayer* CScene::CreateLayer(const string& strTag)
{
	CLayer*	pLayer = CLayer::Create(strTag);

	pLayer->SetScene(this);
	m_vecLayer.push_back(pLayer);

	return pLayer;
}

CLayer* CScene::FindLayer(const string& strTag)
{
	vector<CLayer*>::iterator	iterEnd = m_vecLayer.end();
	for (vector<CLayer*>::iterator iter = m_vecLayer.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter)->GetTag() == strTag)
			return *iter;
	}

	return NULL;
}

void CScene::AddRendering()
{
	vector<CLayer*>::iterator	iterEnd = m_vecLayer.end();
	for (vector<CLayer*>::iterator iter = m_vecLayer.begin(); iter != iterEnd; ++iter)
	{
		(*iter)->AddRendering();
	}
}

CGameObject* CScene::CreateCamera(const string& strKey, CAMERA_TYPE eType,
	const string& strTag,
	const Vec3& vPos, const Vec3& vRot)
{
	CGameObject*	pCamera = FindCamera(strKey);

	if (pCamera)
	{
		pCamera->AddRef();
		return pCamera;
	}

	pCamera = CGameObject::Create(strTag);

	CTransform*	pTransform = pCamera->GetTransform();
	pTransform->SetWorldPos(vPos);
	pTransform->SetWorldRotation(vRot);

	CCamera*	pCameraCom = new CCamera;
	pCameraCom->Init();

	switch (eType)
	{
	case CT_PERSPECTIVE:
		pCameraCom->SetProjection(XM_PI / 3.f, 0.3f, 1000.f);
		break;
	case CT_ORTHOGONAL:
		pCameraCom->SetOrthoProjection(0.f, 1000.f);
		break;
	}

	pCameraCom->SetCameraType(eType);

	//pCameraCom->SetShader(SINGLE(CRenderManager)->FindShader(SHADER_FXKEY_DEFAULT));
	pCameraCom->SetShader(SINGLE(CRenderManager)->FindShader(SHADER_FXKEY_SHARE));
	pCamera->AddComponent(pCameraCom);

	SAFE_RELEASE(pCameraCom);

	m_mapCamera.insert(make_pair(strKey, pCamera));

	return pCamera;
}

CGameObject* CScene::FindCamera(const string& strKey)
{
	map<string, CGameObject*>::iterator	iter = m_mapCamera.find(strKey);

	if (iter == m_mapCamera.end())
		return NULL;

	return iter->second;
}

void CScene::AddLight(CGameObject* pLight)
{
	pLight->AddRef();
	m_LightList.push_back(pLight);
}

CGameObject* CScene::CreateLight(LIGHT_TYPE eType,
	const string& strTag)
{
	CGameObject* pTest;
	return pTest;
}

const list<CGameObject*>* CScene::GetLightList()
{
	return &m_LightList;
}
