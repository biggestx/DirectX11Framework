#include "DirectionalLight.h"
#include "../Rendering/RenderManager.h"
#include "../Rendering/Shader/Shader.h"
#include "../Rendering/RenderManager.h"

USING_BIGGEST

CDirectionalLight::CDirectionalLight()
{

}


CDirectionalLight::~CDirectionalLight()
{
}

bool CDirectionalLight::Init()
{
	if (!CLight::Init())
		return false;

	m_eLightType = LT_DIR;

	if (SINGLE(CRenderManager)->GetRenderMode() == RM_FORWARD)
	{

	}
	else if (SINGLE(CRenderManager)->GetRenderMode() == RM_DEFERRED)
	{
		m_pShader = SINGLE(CRenderManager)->FindShader(SHADER_FXKEY_DEFERRED);
		strcpy(m_szTech, SHADER_TECH_DEFERREDFX_LIGHTACC_DIRECTIONAL);
	}


	return true;
}

void CDirectionalLight::Update(float fTime)
{
	SINGLE(CRenderManager)->AddLight(this);
}

void CDirectionalLight::LateUpdate(float fTime)
{

}

void CDirectionalLight::Render(float fTime)
{

}

void CDirectionalLight::ConstantTable()
{
	m_pShader->SetData(SHADER_HANDLE_SHARE_FLOAT3_LIGHTCOLOR, &m_vLightColor);
	m_pShader->SetData(SHADER_HANDLE_SHARE_FLOAT3_LIGHTDIR, &m_vDir);
}

CComponent* CDirectionalLight::Clone()
{
	return new CDirectionalLight(*this);
}

