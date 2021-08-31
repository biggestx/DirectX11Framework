#pragma once
#include "Light.h"

BIGGEST_BEGIN

class BIGGEST_DLL CPointLight :
	public CLight
{
	CREATE_INSTANCE_TAG(CPointLight)
private:
	float m_fRadius;
	
public:
	float GetRadius()
	{
		return m_fRadius;
	}

public:
	void SetRadius(const float& fRadius)
	{
		m_fRadius = fRadius;
	}

public:
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CComponent* Clone();
	virtual void ConstantTable();

public:
	CPointLight();
	~CPointLight();
};

BIGGEST_END