#pragma once
#include "Component.h"

BIGGEST_BEGIN

class BIGGEST_DLL CCamera :
	public CComponent
{
	FRIEND_CLASS(CGameObject)
		FRIEND_CLASS(CScene)
		CREATE_INSTANCE_TAG(CCamera)

private:
	Matrix		m_matProj;
	Matrix		m_matView;
	float		m_fAngle;
	float		m_fNear;
	float		m_fFar;
	CAMERA_TYPE	m_eCameraType;
	CAMERA_VIEW_TYPE	m_eViewType;
	class CShader* m_pShader;

	XMFLOAT3	m_vLookAtPos;
	XMFLOAT3	m_vUp;

public:
	Matrix GetViewMatrix()
	{
		return m_matView;
	}

	Matrix GetProjectionMatrix()
	{
		return m_matProj;
	}

	XMFLOAT3 GetCameraUp()
	{
		return m_vUp;
	}

public:
	void SetShader(CShader* pShader)
	{
		m_pShader = pShader;
	}

	void SetLookAtPos(XMFLOAT3 vLookAtPos)
	{
		m_vLookAtPos = vLookAtPos;
	}

	void SetCameraUp(XMFLOAT3 vUp)
	{
		m_vUp = vUp;
	}

	void SetViewType(CAMERA_VIEW_TYPE eType)
	{
		m_eViewType = eType;
	}

public:
	void SetCameraType(CAMERA_TYPE eType);
	void SetProjection(float fAngle, float fNear, float fFar);
	void SetOrthoProjection(float fNear, float fFar);
	void SetDevice();

	//void SetShader(class CShader* pShader);

public:
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual CCamera* Clone();
	virtual void ConstantTable();


protected:
	CCamera();
	CCamera(const CCamera& camera);

public:
	~CCamera();
};


BIGGEST_END