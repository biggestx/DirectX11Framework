#include "Camera.h"
#include "../Core/Device.h"
#include "Transform.h"
#include "../GameObject/GameObject.h"
#include "../Rendering/Shader/Shader.h"

USING_BIGGEST

CCamera::CCamera() :
m_pShader(NULL),
m_vLookAtPos(0.f,0.f,0.f),
m_vUp(0.f,1.f,0.f)
{
}

CCamera::CCamera(const CCamera& camera) :
CComponent(camera)
{
	*this = camera;
}

CCamera::~CCamera()
{
}

bool CCamera::Init()
{
	//m_pScene = pScene;
	m_eComType = CT_CAMERA;
	m_eCameraType = CT_PERSPECTIVE;
	m_eViewType = CVT_AXIS;

	XMMATRIX matProj = XMMatrixIdentity();
	XMMATRIX matView = XMMatrixIdentity();
	XMStoreFloat4x4(&m_matProj, matProj);
	XMStoreFloat4x4(&m_matView, matView);


	return true;
}

void CCamera::Update(float fTime)
{
	
}

void CCamera::LateUpdate(float fTime)
{
	if (m_eCameraType == CT_PERSPECTIVE)
	{
		if (m_eViewType == CVT_AXIS)
		{
			// Transform을 얻어온다.
			CTransform*	pTransform = m_pGameObject->GetTransform();

			// 위치 정보를 얻어온다.
			Vec3	vPos = pTransform->GetWorldPos();

			// 축 정보를 얻어온다.
			Vec3	vAxis[WA_MAX] = {};
			for (int i = 0; i < WA_MAX; ++i)
				vAxis[i] = pTransform->GetWorldAxis(i);

			XMMATRIX	matView = XMMatrixIdentity();
			//matView = _MATRIX(&m_matView);


			for (int i = 0; i < WA_MAX; ++i)
			{
				memcpy(&matView(i, 0), &vAxis[i], sizeof(Vec3));
			}

			matView = XMMatrixTranspose(matView);
			for (int i = 0; i < WA_MAX; ++i)
			{
				matView(3, i) = XMVectorGetX(
					-XMVector3Dot(XMLoadFloat3(&vPos),
						XMLoadFloat3(&vAxis[i])));
			}


			XMStoreFloat4x4(&m_matView, matView);
		}
		else if (m_eViewType == CVT_LOOKAT)
		{
			CTransform*	pTransform = m_pGameObject->GetTransform();

			// 위치 정보를 얻어온다.
			Vec3	vPos = pTransform->GetWorldPos();

			XMMATRIX	matView = XMMatrixIdentity();

			matView = XMMatrixLookAtLH(_VEC3(&vPos), _VEC3(&m_vLookAtPos), _VEC3(&m_vUp));

			XMStoreFloat4x4(&m_matView, matView);
		}


	}
}

CCamera* CCamera::Clone()
{
	return new CCamera(*this);
}

void CCamera::SetProjection(float fAngle, float fNear, float fFar)
{
	m_fAngle = fAngle;
	m_fNear = fNear;
	m_fFar = fFar;

	RESOLUTION	rs = SINGLE(CDevice)->GetResolution();
	float	fAspect = rs.fWidth / rs.fHeight;

	XMMATRIX matProj = XMMatrixPerspectiveFovLH(fAngle, fAspect, fNear, fFar);

	XMStoreFloat4x4(&m_matProj, matProj);
}

void CCamera::SetOrthoProjection(float fNear, float fFar)
{
	RESOLUTION	rs = SINGLE(CDevice)->GetResolution();

	m_fNear = fNear;
	m_fFar = fFar;

	XMMATRIX matProj = XMMatrixOrthographicLH(rs.fWidth, rs.fHeight, fNear, fFar);
	XMStoreFloat4x4(&m_matProj, matProj);
}

void CCamera::SetDevice()
{
}

void CCamera::ConstantTable()
{
	if (!m_pShader)
		return;

	m_pShader->SetData(SHADER_HANDLE_SHARE_MATRIX_VIEW, &m_matView);
	m_pShader->SetData(SHADER_HANDLE_SHARE_MATRIX_PROJ, &m_matProj);

	// 다시 확인 필요
	/*XMMATRIX matInvView;
	XMMatrixInverse((XMVECTOR*)&matInvView,_MATRIX(&m_matView));
	*/
}

void CCamera::SetCameraType(CAMERA_TYPE eType)
{
	m_eCameraType = eType;
}
