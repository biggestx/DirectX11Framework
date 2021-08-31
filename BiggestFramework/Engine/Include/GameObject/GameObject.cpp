#include "GameObject.h"
#include "../Component/Transform.h"
#include "../Scene/Scene.h"
#include "../Component/Renderer.h"
#include "../Rendering/RenderManager.h"

USING_BIGGEST

CGameObject::CGameObject() :
m_pLayer(NULL),
m_pScene(NULL),
m_pTransform(NULL),
m_pParent(NULL),
m_pLookAtObject(NULL)
{
}

CGameObject::CGameObject(const CGameObject& obj)
{
	*this = obj;

	m_pTransform = obj.m_pTransform->Clone();
	m_pTransform->SetGameObject(this);

	m_ComponentList.clear();

	for (list<CComponent*>::const_iterator iter = obj.m_ComponentList.begin();
		iter != obj.m_ComponentList.end(); ++iter)
	{
		CComponent*	pCom = (*iter)->Clone();
		AddComponent(pCom);
		SAFE_RELEASE(pCom);
	}

	m_ChildList.clear();
	for (list<CGameObject*>::const_iterator iter = obj.m_ChildList.begin();
		iter != obj.m_ChildList.end(); ++iter)
	{
		CGameObject*	pChild = (*iter)->Clone();
		AddChild(pChild);
		SAFE_RELEASE(pChild);
	}

	if (m_pLookAtObject)
		m_pLookAtObject->AddRef();
}

CGameObject::~CGameObject()
{
	SAFE_RELEASE(m_pLookAtObject);
	Safe_Release_VecList(m_ChildList);
	Safe_Release_VecList(m_ComponentList);
	SAFE_RELEASE(m_pTransform);
}

bool CGameObject::Init()
{
	//m_pScene = pScene;
	m_pTransform = new CTransform;
	if (!m_pTransform->Init())
		return false;

	m_pTransform->SetGameObject(this);

	return true;
}

void CGameObject::Update(float fTime)
{
	m_pTransform->Update(fTime);

	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_ComponentList.end();

	for (iter = m_ComponentList.begin(); iter != iterEnd;)
	{
		if (!(*iter)->GetAlive())
		{
			SAFE_RELEASE((*iter));
			iter = m_ComponentList.erase(iter);
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

	list<CGameObject*>::iterator	iterChild;
	list<CGameObject*>::iterator	iterChildEnd = m_ChildList.end();

	for (iterChild = m_ChildList.begin(); iterChild != iterChildEnd;)
	{
		if (!(*iterChild)->GetAlive())
		{
			/*m_pScene->GetObjectPool()->EraseObject(
			(*iterChild));
			SAFE_RELEASE((*iterChild));
			iterChild = m_ChildList.erase(iterChild);*/
		}

		else
		{
			if (!(*iterChild)->GetEnable())
			{
				++iterChild;
				continue;
			}

			(*iterChild)->Update(fTime);
			++iterChild;
		}
	}
}

void CGameObject::LateUpdate(float fTime)
{
	m_pTransform->LateUpdate(fTime);

	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_ComponentList.end();

	for (iter = m_ComponentList.begin(); iter != iterEnd;)
	{
		if (!(*iter)->GetAlive())
		{
			SAFE_RELEASE((*iter));
			iter = m_ComponentList.erase(iter);
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

	list<CGameObject*>::iterator	iterChild;
	list<CGameObject*>::iterator	iterChildEnd = m_ChildList.end();

	for (iterChild = m_ChildList.begin(); iterChild != iterChildEnd;)
	{
		if (!(*iterChild)->GetAlive())
		{/*
		 m_pScene->GetObjectPool()->EraseObject(
		 (*iterChild));
		 SAFE_RELEASE((*iterChild));
		 iterChild = m_ChildList.erase(iterChild);*/
		}

		else
		{
			if (!(*iterChild)->GetEnable())
			{
				++iterChild;
				continue;
			}

			CTransform*	pChildTransform = (*iterChild)->GetTransform();



			(*iterChild)->LateUpdate(fTime);
			++iterChild;
		}
	}
}

void CGameObject::Render(float fTime)
{
	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_ComponentList.end();

	CRenderer*	pRenderer = (CRenderer*)FindComponent(CT_RENDERER);


	for (iter = m_ComponentList.begin(); iter != iterEnd; ++iter)
	{
		if (!(*iter)->GetEnable())
			continue;

		else if ((*iter)->GetComponentType() == CT_RENDERER)
			continue;

		(*iter)->ConstantTable();
	}

	for (iter = m_ComponentList.begin(); iter != iterEnd; ++iter)
	{
		if (!(*iter)->GetEnable())
			continue;

		(*iter)->Render(fTime);
	}
}

void CGameObject::SetScene(CScene* pScene)
{
	m_pScene = pScene;

	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_ComponentList.end();

	for (iter = m_ComponentList.begin(); iter != iterEnd; ++iter)
	{
		(*iter)->SetScene(pScene);
	}
}

void CGameObject::SetLayer(CLayer* pLayer)
{
	m_pLayer = pLayer;

	list<CComponent*>::iterator	iter;
	list<CComponent*>::iterator	iterEnd = m_ComponentList.end();

	for (iter = m_ComponentList.begin(); iter != iterEnd; ++iter)
	{
		(*iter)->SetLayer(pLayer);
	}
}

CGameObject* CGameObject::Clone()
{
	return new CGameObject(*this);
}

CScene* CGameObject::GetScene()
{
	return m_pScene;
}

CLayer* CGameObject::GetLayer()
{
	return m_pLayer;
}

void CGameObject::AddComponent(CComponent* pComponent)
{
	pComponent->SetScene(m_pScene);
	pComponent->SetLayer(m_pLayer);
	pComponent->SetGameObject(this);
	pComponent->AddRef();
	m_ComponentList.push_back(pComponent);
}

CTransform*	CGameObject::GetTransform()
{
	return m_pTransform;
}

CComponent* CGameObject::FindComponent(const string& strTag)
{
	list<CComponent*>::iterator	iter = m_ComponentList.begin();
	list<CComponent*>::iterator	iterEnd = m_ComponentList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->GetTag() == strTag)
			return *iter;
	}

	return NULL;
}

CComponent* CGameObject::FindComponent(COMPONENT_TYPE eType)
{
	list<CComponent*>::iterator	iter = m_ComponentList.begin();
	list<CComponent*>::iterator	iterEnd = m_ComponentList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->GetComponentType() == eType)
			return *iter;
	}

	return NULL;
}

CComponent* CGameObject::FindComponentFromTypeName(const string& strTypeName)
{
	list<CComponent*>::iterator	iter = m_ComponentList.begin();
	list<CComponent*>::iterator	iterEnd = m_ComponentList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->GetTypeName() == strTypeName)
			return *iter;
	}

	return NULL;
}

list<CComponent*>* CGameObject::FindComponents(const string& strTag)
{
	m_FindList.clear();

	list<CComponent*>::iterator	iter = m_ComponentList.begin();
	list<CComponent*>::iterator	iterEnd = m_ComponentList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->GetTag() == strTag)
			m_FindList.push_back(*iter);
	}

	return &m_FindList;
}

list<CComponent*>* CGameObject::FindComponents(COMPONENT_TYPE eType)
{
	m_FindList.clear();

	list<CComponent*>::iterator	iter = m_ComponentList.begin();
	list<CComponent*>::iterator	iterEnd = m_ComponentList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->GetComponentType() == eType)
			m_FindList.push_back(*iter);
	}

	return &m_FindList;
}

list<CComponent*>* CGameObject::FindComponentsFromTypeName(const string& strTypeName)
{
	m_FindList.clear();

	list<CComponent*>::iterator	iter = m_ComponentList.begin();
	list<CComponent*>::iterator	iterEnd = m_ComponentList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->GetTypeName() == strTypeName)
			m_FindList.push_back(*iter);
	}

	return &m_FindList;
}

void CGameObject::AddChild(CGameObject* pChild)
{
	pChild->AddRef();
	pChild->SetScene(m_pScene);
	pChild->SetLayer(m_pLayer);
	pChild->SetParent(this);
	//m_pScene->GetObjectPool()->AddObject(pChild);
	m_ChildList.push_back(pChild);
}

void CGameObject::SetParent(CGameObject* pParent)
{
	m_pParent = pParent;
}

CGameObject* CGameObject::FindChild(const string& strTag)
{
	list<CGameObject*>::iterator	iterChild;
	list<CGameObject*>::iterator	iterChildEnd = m_ChildList.end();

	for (iterChild = m_ChildList.begin(); iterChild != iterChildEnd; ++iterChild)
	{
		if ((*iterChild)->GetTag() == strTag)
			return *iterChild;
	}

	return NULL;
}

CGameObject* CGameObject::GetParent()
{
	return m_pParent;
}

void CGameObject::AddRenderingObject()
{
	CRenderer*	pRenderer = (CRenderer*)FindComponent(CT_RENDERER);

	if (pRenderer)
		SINGLE(CRenderManager)->AddRenderObject(this, RF_NORMAL_OBJECT);

	list<CGameObject*>::iterator	iterChild;
	list<CGameObject*>::iterator	iterChildEnd = m_ChildList.end();

	for (iterChild = m_ChildList.begin(); iterChild != iterChildEnd; ++iterChild)
	{
		(*iterChild)->AddRenderingObject();
	}
}

CGameObject* CGameObject::GetLookAtObject()
{
	return m_pLookAtObject;
}

void CGameObject::SetLookAtObject(CGameObject* pLookAtObject)
{
	m_pLookAtObject = pLookAtObject;
	pLookAtObject->AddRef();
}

