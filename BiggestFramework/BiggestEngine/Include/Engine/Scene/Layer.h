#pragma once

#include "../Core/Ref.h"

BIGGEST_BEGIN

class BIGGEST_DLL CLayer :
	public CRef
{
	CREATE_INSTANCE_TAG(CLayer)

private:
	friend class CScene;

private:
	CScene*		m_pScene;
	list<class CGameObject*>	m_ObjList;

public:
	void SetScene(CScene* pScene);

public:
	bool Init();
	void Update(float fTime);
	void LateUpdate(float fTime);
	void AddObject(CGameObject* pGameObject);
	void AddRendering();
	void AddCollider();

private:
	CLayer();
	~CLayer();
};

BIGGEST_END