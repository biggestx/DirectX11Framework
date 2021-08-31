#pragma once
#include "Component.h"

BIGGEST_BEGIN

class BIGGEST_DLL CLight :
	public CComponent
{
protected:
	class CShader*	m_pShader;
	LIGHT_TYPE		m_eLightType;
	char			m_szTech[64];

	XMFLOAT3		m_vLightColor;

public:
	void SetShader(CShader* pShader)
	{
		m_pShader = pShader;
	}

	void SetLightColor(XMFLOAT3 vLightColor)
	{
		m_vLightColor = vLightColor;
	}

public:
	CShader* GetShader()
	{
		return m_pShader;
	}

	LIGHT_TYPE GetLightType()
	{
		return m_eLightType;
	}

	XMFLOAT3 GetLightColor()
	{
		return m_vLightColor;
	}

public:
	virtual bool Init();
	virtual void Update(float fTime) = 0;
	virtual void LateUpdate(float fTime) = 0;
	virtual void Render(float fTime) = 0;
	virtual CComponent* Clone() = 0;
	virtual void ConstantTable() = 0;

public:
	CLight();
	~CLight();
};

BIGGEST_END