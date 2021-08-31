#include "Layer.h"
#include "Scene.h"
#include "../GameObject/GameObject.h"

USING_BIGGEST

CLayer::CLayer() :
m_pScene(NULL)
{
}

CLayer::~CLayer()
{
	Safe_Release_VecList(m_ObjList);
}

bool CLayer::Init()
{
	return true;
}

void CLayer::Update(float fTime)
{
	list<CGameObject*>::iterator	iterEnd = m_ObjList.end();

	for (list<CGameObject*>::iterator iter = m_ObjList.begin(); iter != iterEnd;)
	{
		if (!(*iter)->GetAlive())
		{
			//m_pScene->GetObjectPool()->EraseObject(
			//	*iter);
			SAFE_RELEASE((*iter));
			iter = m_ObjList.erase(iter);
		}

		else
		{
			if (!(*iter)->GetEnable())
			{
				++iter;
				continue;
			}

			(*iter)->Update(fTime);
			++iter;
		}
	}
}

void CLayer::LateUpdate(float fTime)
{
	list<CGameObject*>::iterator	iterEnd = m_ObjList.end();

	for (list<CGameObject*>::iterator iter = m_ObjList.begin(); iter != iterEnd;)
	{
		if (!(*iter)->GetAlive())
		{
			/*m_pScene->GetObjectPool()->EraseObject(
			*iter);*/
			SAFE_RELEASE((*iter));
			iter = m_ObjList.erase(iter);
		}

		else
		{
			if (!(*iter)->GetEnable())
			{
				++iter;
				continue;
			}

			(*iter)->LateUpdate(fTime);
			++iter;
		}
	}
}

void CLayer::AddCollider()
{
	list<CGameObject*>::iterator	iterEnd = m_ObjList.end();

	for (list<CGameObject*>::iterator iter = m_ObjList.begin(); iter != iterEnd; ++iter)
	{
		if (!(*iter)->GetEnable())
			continue;

	}
}

void CLayer::AddObject(CGameObject* pGameObject)
{
	pGameObject->SetScene(m_pScene);
	pGameObject->SetLayer(this);
	pGameObject->AddRef();

	//m_pScene->GetObjectPool()->AddObject(pGameObject);

	m_ObjList.push_back(pGameObject);
}

void CLayer::SetScene(CScene* pScene)
{
	m_pScene = pScene;
}

void CLayer::AddRendering()
{
	list<CGameObject*>::iterator	iterEnd = m_ObjList.end();

	for (list<CGameObject*>::iterator iter = m_ObjList.begin(); iter != iterEnd; ++iter)
	{
		if (!(*iter)->GetEnable())
			continue;

		(*iter)->AddRenderingObject();
	}
}
