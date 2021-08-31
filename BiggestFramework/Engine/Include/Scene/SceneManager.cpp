#include "SceneManager.h"
#include "Scene.h"

USING_BIGGEST

DEFINITION_SINGLE(CSceneManager)

CSceneManager::CSceneManager() :
m_pScene(NULL),
m_pNextScene(NULL),
m_pPrevScene(NULL)
{
}

CSceneManager::~CSceneManager()
{
	SAFE_RELEASE(m_pScene);
	SAFE_RELEASE(m_pNextScene);
	SAFE_RELEASE(m_pPrevScene);
}

bool CSceneManager::Init()
{
	m_pScene = CScene::Create();

	return true;
}

SCENE_CHANGE CSceneManager::Update(float fTime)
{
	m_pScene->Update(fTime);

	return Change();
}

SCENE_CHANGE CSceneManager::LateUpdate(float fTime)
{
	m_pScene->LateUpdate(fTime);

	return Change();
}

void CSceneManager::AddCollider()
{
	m_pScene->AddCollider();
}

void CSceneManager::ChangeScene(CScene* pNext, SCENE_CHANGE_TYPE type)
{
	m_pNextScene = pNext;
	switch (type)
	{
	case SCT_PUSH:
		SAFE_RELEASE(m_pPrevScene);
		m_pPrevScene = m_pScene;
		break;
	}
}

SCENE_CHANGE CSceneManager::Change()
{
	SCENE_CHANGE	change = SC_NONE;

	if (m_pNextScene)
	{
		if (m_pPrevScene)
		{
			m_pScene = m_pNextScene;
			change = SC_NEXT;
		}

		else
		{
			SAFE_RELEASE(m_pScene);
			m_pScene = m_pNextScene;
			change = SC_NEXT;
		}

		m_pNextScene = NULL;
	}

	return change;
}

void CSceneManager::ChangeReturn()
{
	if (!m_pPrevScene)
		return;

	SAFE_RELEASE(m_pScene);
	m_pScene = m_pPrevScene;
	m_pPrevScene = NULL;
}

CScene* CSceneManager::GetCurrentScene()
{
	return m_pScene;
}

void CSceneManager::AddRenderingObject()
{
	m_pScene->AddRendering();
}

class CGameObjectPool* CSceneManager::GetCurrentObjectPool()
{
	return m_pScene->GetObjectPool();
}

class CGameObjectPool* CSceneManager::GetPrevObjectPool()
{
	return m_pPrevScene->GetObjectPool();
}

class CGameObjectPool* CSceneManager::GetNextObjectPool()
{
	return m_pNextScene->GetObjectPool();
}

class CResourcesManager* CSceneManager::GetCurrentResourceManager()
{
	return m_pScene->GetResourceManager();
}

class CResourcesManager* CSceneManager::GetPrevResourceManager()
{
	return m_pPrevScene->GetResourceManager();
}

class CResourcesManager* CSceneManager::GetNextResourceManager()
{
	return m_pNextScene->GetResourceManager();
}

CScene* CSceneManager::CreateScene()
{
	return CScene::Create();
}
