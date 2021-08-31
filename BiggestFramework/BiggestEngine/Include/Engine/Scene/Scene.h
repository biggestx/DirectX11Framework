#pragma once

#include "../Core/Ref.h"

BIGGEST_BEGIN

class BIGGEST_DLL CScene :
	public CRef
{
	CREATE_INSTANCE_TAG(CScene)

private:
	friend class CSceneManager;

private:
	vector<class CLayer*>	m_vecLayer;
	vector<class CScript*>	m_vecScript;
	class CGameObject*		m_pCamera;
	class CGameObject*		m_pUICamera;
	map<string, CGameObject*>	m_mapCamera;
	class CScheduler*		m_pScheduler;
	class CGameObjectPool*	m_pObjectPool;
	class CResourcesManager*	m_pResourcesManager;

private:
	list<CGameObject*>		m_LightList;

public:
	class CResourcesManager* GetResourceManager()
	{
		return m_pResourcesManager;
	}

	class CGameObjectPool* GetObjectPool()
	{
		return m_pObjectPool;
	}

public:
	const list<CGameObject*>* GetLightList();
	void AddLight(CGameObject* pLight);
	CGameObject* CreateLight(LIGHT_TYPE eType, const string& strTag = "Light");

public:
	CScheduler* GetScheduler()
	{
		return m_pScheduler;
	}

	CGameObject* GetMainCamera()
	{
		return m_pCamera;
	}

	CGameObject* GetUICamera()
	{
		return m_pUICamera;
	}

public:
	bool Init();
	void Update(float fTime);
	void LateUpdate(float fTime);
	void AddCollider();
	CLayer* CreateLayer(const string& strTag = "Default");
	CLayer* FindLayer(const string& strTag = "Default");
	void AddRendering();
	CGameObject* CreateCamera(const string& strKey, CAMERA_TYPE eType,
		const string& strTag = "MainCamera",
		const Vec3& vPos = Vec3(0.f, 0.f, 0.f), const Vec3& vRot = Vec3(0.f, 0.f, 0.f));
	CGameObject* FindCamera(const string& strKey);

	template <typename T>
	CScript* CreateScript(const string& strTag = "Default")
	{
		CScript*	pScript = new T;

		pScript->SetTag(strTag);
		pScript->SetScene(this);

		if (!pScript->Init())
		{
			SAFE_DELETE(pScript);
			return NULL;
		}

		m_vecScript.push_back(pScript);

		return pScript;
	}

private:
	CScene();
	~CScene();
};

BIGGEST_END