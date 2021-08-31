#pragma once

#include "../Core/Ref.h"

BIGGEST_BEGIN

class BIGGEST_DLL CSceneManager :
	public CRef
{
private:
	class CScene*	m_pScene;
	class CScene*	m_pNextScene;
	class CScene*	m_pPrevScene;

public:
	CScene* GetCurrentScene();
	class CGameObjectPool* GetCurrentObjectPool();
	class CGameObjectPool* GetPrevObjectPool();
	class CGameObjectPool* GetNextObjectPool();

	class CResourcesManager* GetCurrentResourceManager();
	class CResourcesManager* GetPrevResourceManager();
	class CResourcesManager* GetNextResourceManager();

public:
	bool Init();
	SCENE_CHANGE Update(float fTime);
	SCENE_CHANGE LateUpdate(float fTime);
	void AddCollider();
	void ChangeScene(CScene* pNext, SCENE_CHANGE_TYPE type);
	void ChangeReturn();
	void AddRenderingObject();
	CScene* CreateScene();

private:
	SCENE_CHANGE Change();

	DECLARE_SINGLE(CSceneManager)
};

BIGGEST_END