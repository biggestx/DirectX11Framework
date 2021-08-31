#pragma once
#include "Light.h"

BIGGEST_BEGIN

class BIGGEST_DLL CDirectionalLight :
	public CLight
{
	CREATE_INSTANCE_TAG(CDirectionalLight)
private:
	XMFLOAT3 m_vDir;

public:
	void SetDirection(XMFLOAT3 vDir)
	{
		m_vDir = vDir;
	}

public:
	XMFLOAT3 GetDirection()
	{
		return m_vDir;
	}

public:
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CComponent* Clone() ;
	virtual void ConstantTable();

public:
	CDirectionalLight();
	~CDirectionalLight();
};

BIGGEST_END