#pragma once
#include "Script.h"

BIGGEST_BEGIN

class BIGGEST_DLL CThirdPersonCam :
	public CScript
{
	CREATE_INSTANCE_TAG(CThirdPersonCam)
private:
	class CCamera* m_pCameraCom;
	class CGameObject* m_pLookAtObj;

	float			m_fDistanceFromObj;
	XMFLOAT3		m_vRot;
	XMFLOAT3		m_vLookAtObjOffsetPos;
public:
	void SetCameraComponent(CCamera* pCamera)
	{
		m_pCameraCom = pCamera;
	}

	void SetLookAtObject(CGameObject* pGameObject)
	{
		m_pLookAtObj = pGameObject;
	}

	void SetLookAtObjOffsetPos(XMFLOAT3 vOffset)
	{
		m_vLookAtObjOffsetPos = vOffset;
	}
public:
	CCamera* GetCameraComponent()
	{
		return m_pCameraCom;
	}

public:
	void CalculateInput();

public:
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual CScript* Clone();
public:
	CThirdPersonCam();
	~CThirdPersonCam();
};


BIGGEST_END