#include "PointLight.h"
#include "../Rendering/RenderManager.h"

#include "../GameObject/GameObject.h"
#include "../Component/Transform.h"
#include "../Rendering/Shader/Shader.h"

USING_BIGGEST
CPointLight::CPointLight() : 
m_fRadius(1.f)
{
}


CPointLight::~CPointLight()
{
}

bool CPointLight::Init()
{
	if (!CLight::Init())
		return false;

	m_eLightType = LT_POINT;

	return true;
}

void CPointLight::Update(float fTime)
{
	SINGLE(CRenderManager)->AddLight(this);
}

void CPointLight::LateUpdate(float fTime)
{

}
void CPointLight::Render(float fTime)
{

}

CComponent* CPointLight::Clone()
{
	return new CPointLight(*this);
}
void CPointLight::ConstantTable()
{
	auto vLightPos = GetGameObject()->GetTransform()->GetWorldPos();

	m_pShader->SetData(SHADER_HANDLE_SHARE_FLOAT3_LIGHTPOS, &vLightPos);
	m_pShader->SetData(SHADER_HANDLE_SHARE_FLOAT3_LIGHTCOLOR, &m_vLightColor);
	m_pShader->SetData(SHADER_HANDLE_SHARE_FLOAT_LIGHTRADIUS, &m_fRadius);
}