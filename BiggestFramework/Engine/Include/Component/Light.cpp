#include "Light.h"

USING_BIGGEST


CLight::CLight() :
m_pShader(NULL),
m_vLightColor(1.f,1.f,1.f)
{
	memset(m_szTech, 0, 64);
}


CLight::~CLight()
{
}

bool CLight::Init()
{
	m_eComType = CT_LIGHT;

	return true;
}