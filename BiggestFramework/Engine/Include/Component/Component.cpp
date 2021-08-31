#include "Component.h"

USING_BIGGEST

CComponent::CComponent() :
m_pGameObject(NULL),
m_pScene(NULL),
m_pLayer(NULL)
{
}

CComponent::CComponent(const CComponent& com)
{
	*this = com;
}

CComponent::~CComponent()
{
}

void CComponent::SetScene(CScene* pScene)
{
	m_pScene = pScene;
}

void CComponent::SetLayer(CLayer* pLayer)
{
	m_pLayer = pLayer;
}

void CComponent::SetGameObject(CGameObject* pGameObject)
{
	m_pGameObject = pGameObject;
}

void CComponent::Render(float fTime)
{
}

COMPONENT_TYPE CComponent::GetComponentType()
{
	return m_eComType;
}

void CComponent::ConstantTable()
{
}

bool CComponent::Init()
{
	return true;
}

void CComponent::Update(float fTime)
{

}
void CComponent::LateUpdate(float fTime)
{

}
CComponent* CComponent::Clone()
{
	return new CComponent(*this);
}