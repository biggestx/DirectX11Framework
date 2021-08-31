#pragma once
#include "../Core/Ref.h"


BIGGEST_BEGIN

class BIGGEST_DLL CGameObject :
	public CRef
{
	CREATE_INSTANCE_TAG(CGameObject)

protected:
	class CScene*	m_pScene;
	class CLayer*	m_pLayer;
	friend class CScene;

public:
	void SetScene(CScene* pScene);
	void SetLayer(CLayer* pLayer);

public:
	CScene* GetScene();
	CLayer* GetLayer();

private:
	list<class CComponent*>		m_ComponentList;
	class CTransform*	m_pTransform;
	CGameObject*		m_pLookAtObject;
	list<CComponent*>			m_FindList;

public:
	CTransform*	GetTransform();
	CGameObject* GetLookAtObject();

public:
	void SetLookAtObject(CGameObject* pLookAtObject);
	void AddComponent(CComponent* pComponent);
	CComponent* FindComponent(const string& strTag);
	CComponent* FindComponent(COMPONENT_TYPE eType);
	CComponent* FindComponentFromTypeName(const string& strTypeName);
	list<CComponent*>* FindComponents(const string& strTag);
	list<CComponent*>* FindComponents(COMPONENT_TYPE eType);
	list<CComponent*>* FindComponentsFromTypeName(const string& strTypeName);

private:
	list<CGameObject*>		m_ChildList;
	CGameObject*			m_pParent;

public:
	CGameObject* GetParent();

public:
	void AddChild(CGameObject* pChild);
	void SetParent(CGameObject* pParent);
	CGameObject* FindChild(const string& strTag);

public:
	void AddRenderingObject();

public:
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CGameObject* Clone();

private:
	CGameObject();
	CGameObject(const CGameObject& obj);
	virtual ~CGameObject();

};


BIGGEST_END