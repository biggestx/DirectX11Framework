#include "ThirdPersonCam.h"
#include "../GameObject/GameObject.h"
#include "../Component/Camera.h"
#include "../Component/Transform.h"
#include "../Core/Input.h"

USING_BIGGEST

CThirdPersonCam::CThirdPersonCam() : 
m_pCameraCom(NULL),
m_pLookAtObj(NULL),
m_fDistanceFromObj(10.f),
m_vRot(0.f,0.f,0.f),
m_vLookAtObjOffsetPos(0.f,3.f,0.f)
{
}


CThirdPersonCam::~CThirdPersonCam()
{
}

bool CThirdPersonCam::Init()
{
	CScript::Init();

	return true;
}

void CThirdPersonCam::Update(float fTime)
{
	CalculateInput();

	XMVECTOR vLookAtObjPos = XMLoadFloat3(&m_pLookAtObj->GetTransform()->GetWorldPos());
	XMVECTOR vCamPos = XMVectorSet(0.f, 0.f, -1.f,0.f);
	XMVECTOR vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	

	XMMATRIX matRot,matRotX, matRotY;

	matRotY = XMMatrixRotationY(m_vRot.x);


	vCamPos = XMVector3TransformNormal(vCamPos, matRotY);
	
	XMVECTOR vAxisX = XMVector3Cross(vCamPos, vUp);

	matRotX = XMMatrixRotationAxis(vAxisX, m_vRot.y);

	matRot = matRotX * matRotY;

	vCamPos = XMVector3TransformNormal(vCamPos, matRotX);


	vCamPos *= m_fDistanceFromObj;
	vCamPos += vLookAtObjPos;
	
	XMFLOAT3 vFinalValue;
	XMStoreFloat3(&vFinalValue, vCamPos);

	m_pGameObject->GetTransform()->SetWorldPos(vFinalValue.x,vFinalValue.y, vFinalValue.z);

	m_pCameraCom->SetLookAtPos(m_pLookAtObj->GetTransform()->GetWorldPos());
}

void CThirdPersonCam::LateUpdate(float fTime)
{

}

void CThirdPersonCam::CalculateInput()
{
	
	m_fDistanceFromObj -= SINGLE(CInput)->GetWheel();
	if (m_fDistanceFromObj < 1)
		m_fDistanceFromObj = 1;

	if (SINGLE(CInput)->GetKeyDown(VK_RBUTTON) == true)
	{	
		POINT ptMouseMove = SINGLE(CInput)->GetMouseMove();
		m_vRot.x += ptMouseMove.x / 10.f;
		m_vRot.y += ptMouseMove.y / 10.f;

		if (m_vRot.y >= XM_PI / 4)
			m_vRot.y = XM_PI / 4;
		else if (m_vRot.y <= 0)
			m_vRot.y = 0;
	}
}

CScript* CThirdPersonCam::Clone()
{
	return new CThirdPersonCam(*this);
}
 



