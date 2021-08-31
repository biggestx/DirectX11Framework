#include "Transform.h"
#include "../GameObject/GameObject.h"

USING_BIGGEST

CTransform::CTransform() :
m_bStatic(false)
//m_iParentInfluence(TI_POSITION),
//m_iLookAtInfluence(TI_NONE)
{
}

CTransform::CTransform(const CTransform& transform) :
CComponent(transform)
{
	*this = transform;
}

CTransform::~CTransform()
{
}

bool CTransform::Init()
{
	//m_pScene = pScene;

	m_vScale = Vec3(1.f, 1.f, 1.f);
	m_vOriginScale = Vec3(1.f, 1.f, 1.f);
	m_vLocalPos = Vec3(0.f, 0.f, 0.f);
	m_vWorldPos = Vec3(0.f, 0.f, 0.f);
	m_vLocalRotation = Vec3(0.f, 0.f, 0.f);
	m_vWorldRotation = Vec3(0.f, 0.f, 0.f);


	m_vLocalAxis[WA_X] = Vec3(1.f, 0.f, 0.f);
	m_vLocalAxis[WA_Y] = Vec3(0.f, 1.f, 0.f);
	m_vLocalAxis[WA_Z] = Vec3(0.f, 0.f, 1.f);

	m_vWorldAxis[WA_X] = Vec3(1.f, 0.f, 0.f);
	m_vWorldAxis[WA_Y] = Vec3(0.f, 1.f, 0.f);
	m_vWorldAxis[WA_Z] = Vec3(0.f, 0.f, 1.f);

	m_vOriginView = Vec3(0.f, 0.f, 1.f);

	XMStoreFloat4x4(&m_matScale, XMMatrixIdentity());
	XMStoreFloat4x4(&m_matRotation, XMMatrixIdentity());
	XMStoreFloat4x4(&m_matTrans, XMMatrixIdentity());
	XMStoreFloat4x4(&m_matParent, XMMatrixIdentity());
	XMStoreFloat4x4(&m_matWorld, XMMatrixIdentity());
	XMStoreFloat4x4(&m_matLocalRotation, XMMatrixIdentity());

	m_eComType = CT_TRANSFORM;

	XMStoreFloat4x4(&m_matLocalTrans, XMMatrixIdentity());
	XMStoreFloat4x4(&m_matLocalRot, XMMatrixIdentity());
	XMStoreFloat4x4(&m_matOriginScale, XMMatrixIdentity());


	m_eRotationType = TRT_XYZ;

	return true;
}

void CTransform::Update(float fTime)
{
	
}

void CTransform::LateUpdate(float fTime)
{
	CGameObject*	pParentObj = m_pGameObject->GetParent();


	Vec3	vRenderPos = m_vWorldPos;

	CalculateLocalMatrix();
	CalculateRotation();

	XMMATRIX matLocal = XMLoadFloat4x4(&m_matLocal);

	XMMATRIX matTrans = XMMatrixTranslation(m_vWorldPos.x, m_vWorldPos.y, m_vWorldPos.z);
	XMMATRIX matRot = _MATRIX(&m_matRotation);
	XMMATRIX matScale = XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z);


	XMStoreFloat4x4(&m_matTrans, matTrans);
	XMStoreFloat4x4(&m_matScale, matScale);

	XMMATRIX matWorld = matLocal * matScale * matRot * matTrans;


	XMStoreFloat4x4(&m_matWorld, matWorld);

	CalculateAxis();
}

CTransform* CTransform::Clone()
{
	return new CTransform(*this);
}

void CTransform::SetDevice()
{
	//DEVICE->SetTransform(D3DTS_WORLD, &m_matWorld);
}

void CTransform::SetShader(class CShader* pShader)
{
	//pShader->SetData(SHADER_SHARE_WORLDMATRIX, &m_matWorld);
}

void CTransform::CalculateRotation()
{



	CalculateWorldRotation();
}

void CTransform::CalculateAxis()
{

}

void CTransform::CalculateWorldRotation()
{
	switch (m_eRotationType)
	{
	case TRT_XYZ:
	{
		XMMATRIX matRotX = XMMatrixRotationX(m_vWorldRotation.x);
		XMMATRIX matRotY = XMMatrixRotationY(m_vWorldRotation.y);
		XMMATRIX matRotZ = XMMatrixRotationZ(m_vWorldRotation.z);

		XMMATRIX matRot = matRotX * matRotY * matRotZ;

		XMStoreFloat4x4(&m_matRotation, matRot);
	}
		break;
	case TRT_AXIS:
	{

	}
		break;

	}

}


//

void CTransform::Copy(CTransform* pTransform)
{
	CGameObject*	pGameObject = pTransform->GetGameObject();
	*pTransform = *this;
	pTransform->SetGameObject(pGameObject);
}



void CTransform::CalculateLocalMatrix()
{
	XMMATRIX matTrans = XMMatrixTranslation(m_vLocalPos.x, m_vLocalPos.y, m_vLocalPos.z);
	XMStoreFloat4x4(&m_matLocalTrans, matTrans);
		
	XMMATRIX matScale = XMMatrixScaling(m_vOriginScale.x, m_vOriginScale.y, m_vOriginScale.z);
	XMStoreFloat4x4(&m_matOriginScale, matScale);


	XMMATRIX matRotX = XMMatrixRotationX(m_vLocalRotation.x);
	XMMATRIX matRotY = XMMatrixRotationY(m_vLocalRotation.y);
	XMMATRIX matRotZ = XMMatrixRotationZ(m_vLocalRotation.z);

	XMMATRIX matRot = matRotX * matRotY * matRotZ;

	XMStoreFloat4x4(&m_matLocalRotation, matRot);

	XMMATRIX matLocal = matScale * matRot * matTrans;

	XMStoreFloat4x4(&m_matLocal, matLocal);

}







