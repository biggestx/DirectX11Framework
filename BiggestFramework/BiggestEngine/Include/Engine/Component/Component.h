#pragma once
#include "../Core/Ref.h"

BIGGEST_BEGIN

class BIGGEST_DLL CComponent :
	public CRef
{
	CREATE_INSTANCE(CComponent)
protected:
	class CGameObject*		m_pGameObject;
	class CScene*			m_pScene;
	class CLayer*			m_pLayer;

public:
	void SetGameObject(CGameObject* pGameObject);
	void SetScene(CScene* pScene);
	void SetLayer(CLayer* pLayer);

public:
	CGameObject* GetGameObject()
	{
		return m_pGameObject;
	}

protected:
	COMPONENT_TYPE		m_eComType;

public:
	COMPONENT_TYPE GetComponentType();

public:
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CComponent* Clone();
	virtual void ConstantTable();

public:
	CComponent();
	CComponent(const CComponent& com);
	virtual ~CComponent();
};

BIGGEST_END