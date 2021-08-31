#include "CharacterController.h"
#include "../Core/Input.h"
#include "../GameObject/GameObject.h"
#include "../Component/Transform.h"

USING_BIGGEST

CCharacterController::CCharacterController()
{
}


CCharacterController::~CCharacterController()
{
}

CScript* CCharacterController::Clone()
{
	return new CCharacterController(*this);
}

bool CCharacterController::Init()
{

	return true;
}
void CCharacterController::Update(float fTime)
{
	if (SINGLE(CInput)->FindKey('W')->bDown)
	{
		auto pos = m_pGameObject->GetTransform()->GetWorldPos();
		pos.z += 0.1f;
		m_pGameObject->GetTransform()->SetWorldPos(pos);
	}
	
	if (SINGLE(CInput)->FindKey('A')->bDown)
	{
		auto pos = m_pGameObject->GetTransform()->GetWorldPos();
		pos.x -= 0.1f;
		m_pGameObject->GetTransform()->SetWorldPos(pos);
	}

	if (SINGLE(CInput)->FindKey('S')->bDown)
	{
		auto pos = m_pGameObject->GetTransform()->GetWorldPos();
		pos.z -= 0.1f;
		m_pGameObject->GetTransform()->SetWorldPos(pos);
	}

	if (SINGLE(CInput)->FindKey('D')->bDown)
	{
		auto pos = m_pGameObject->GetTransform()->GetWorldPos();
		pos.x += 0.1f;
		m_pGameObject->GetTransform()->SetWorldPos(pos);
	}

}
void CCharacterController::LateUpdate(float fTime)
{

}
