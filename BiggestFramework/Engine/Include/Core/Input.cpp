#include "Input.h"
#include "Device.h"
#include "Ref.h"

USING_BIGGEST

DEFINITION_SINGLE(CInput)

CInput::CInput() :
m_sWheel(0),
m_pMouse(NULL)
{
}

CInput::~CInput()
{/*
	SAFE_RELEASE(m_pRay);
	SAFE_RELEASE(m_pMouse);*/
}

bool CInput::Init(HWND hWnd)
{
	m_ptMousePos.x = m_ptMousePos.y = 0;
	m_ptMovePos.x = m_ptMovePos.y = 0;
	m_hWnd = hWnd;
	m_eMouseMode = MM_FREE;

	CreateKey(VK_RBUTTON);
	CreateKey(VK_LBUTTON);

	CreateKey('W');
	CreateKey('A');
	CreateKey('S');
	CreateKey('D');

	/*m_pMouse = CGameObject::Create(NULL, "Mouse");

	m_pRay = CColliderRay::Create(NULL, "MouseRay");*/

	//m_pMouse->AddComponent(m_pRay);

	return true;
}

void CInput::ComputeRay()
{
	// 마우스 위치를 뷰공간으로 변환한다.
	/*D3DVIEWPORT9	tVP;

	DEVICE->GetViewport(&tVP);

	m_tViewRay.vPos = Vec3(0.f, 0.f, 0.f);
	Vec3	vDir;

	CGameObject*	pCameraObj = SINGLE(CSceneManager)->GetCurrentScene()->GetMainCamera();
	CCamera*	pCamera = (CCamera*)pCameraObj->FindComponent(CT_CAMERA);

	Matrix	matProj = pCamera->GetProjectionMatrix();

	vDir.x = (((2.f * m_ptMousePos.x) / tVP.Width) - 1) / matProj._11;
	vDir.y = (((-2.f * m_ptMousePos.y) / tVP.Height) + 1) / matProj._22;
	vDir.z = 1.f;

	D3DXVec3Normalize(&m_tViewRay.vDir, &vDir);

	Matrix	matView = pCamera->GetViewMatrix();

	m_tWorldRay = ConvertSpaceRay(m_tViewRay, matView);*/
}

RAY CInput::ConvertSpaceRay(const RAY& ray, Matrix& matSpace)
{
	/*RAY	tRay;
	memset(&tRay, 0, sizeof(RAY));

	D3DXMatrixInverse(&matSpace, 0, &matSpace);

	D3DXVec3TransformCoord(&tRay.vPos, &ray.vPos, &matSpace);
	D3DXVec3TransformNormal(&tRay.vDir, &ray.vDir, &matSpace);
	D3DXVec3Normalize(&tRay.vDir, &tRay.vDir);

	return tRay;
	*/
	RAY tRay;
	return tRay;
}

void CInput::Update(float fTime)
{
	CalculateMouse();

	/*CTransform*	pTransform = m_pMouse->GetTransform();

	m_pRay->SetInfo(m_tWorldRay.vPos, m_tWorldRay.vDir);*/

	//pTransform->SetWorldPos(m_tWorldRay.vPos);
	//pTransform->SetWorldAxisZ(m_tWorldRay.vDir);

	//m_pMouse->Update(fTime);
	//m_pMouse->LateUpdate(fTime);

	//SINGLE(CColliderManager)->AddCollider(m_pMouse);

	map<DWORD, INPUTINFO>::iterator	iter;

	for (iter = m_mapKey.begin(); iter != m_mapKey.end(); ++iter)
	{
		PINPUTINFO	pInfo = &iter->second;
		
		// 0x0000 : 이전에 누른 적이 없고 호출 시점에도 눌러 있지 않은 상태
		// 0x0001 : 이전에 누른 적이 있고 호출 시점에는 눌러 있지 않은 상태
		// 0x8000 : 이전에 누른 적이 없고 호출 시점에는 눌러 있는 상태
		// 0x8001 : 이전에 누른 적이 있고 호출 시점에도 눌러 있는 상태 
		auto keyState = GetAsyncKeyState(iter->first);
		if (keyState & 0x0000)
		{
			pInfo->bDown = false;
			pInfo->bPress = false;
			pInfo->bUp = false;
		}
		else if (keyState & 0x0001)
		{
			pInfo->bDown = false;
			pInfo->bPress = false;
			pInfo->bUp = true;
		}
		else if (keyState & 0x8000)
		{
			pInfo->bDown = true;
			pInfo->bPress = false;
			pInfo->bUp = false;
		}
		else if (keyState & 0x8001)
		{
			pInfo->bDown = false;
			pInfo->bPress = true;
			pInfo->bUp = false;
		}
		else
		{
			pInfo->bDown = false;
			pInfo->bPress = false;
			pInfo->bUp = false;
		}
		


		
	}
}

PINPUTINFO CInput::FindKey(const DWORD& key)
{
	map<DWORD, INPUTINFO>::iterator	iter = m_mapKey.find(key);

	if (iter == m_mapKey.end())
		return NULL;

	return &iter->second;
}

bool CInput::GetKeyPress(const DWORD& key)
{
	auto retval = FindKey(key);
	if (retval == NULL)
		return false;
	
	return retval->bPress;
}

bool CInput::GetKeyDown(const DWORD& key)
{
	auto retval = FindKey(key);
	if (retval == NULL)
		return false;

	return retval->bDown;
}

bool CInput::GetKeyUp(const DWORD& key)
{
	auto retval = FindKey(key);
	if (retval == NULL)
		return false;

	return retval->bUp;
}

void CInput::CalculateMouse()
{
	POINT	ptPos = { 0 };
	GetCursorPos(&ptPos);
	ScreenToClient(m_hWnd, &ptPos);

	RESOLUTION rs = SINGLE(CDevice)->GetResolution();

	RECT	rc;
	GetWindowRect(m_hWnd, &rc);

	float	x, y;
	x = rs.fWidth / (rc.right - rc.left);
	y = rs.fHeight / (rc.bottom - rc.top);

	ptPos.x *= x;
	ptPos.y *= y;

	// 마우스 이동값을 구한다.
	m_ptMovePos.x = ptPos.x - m_ptMousePos.x;
	m_ptMovePos.y = ptPos.y - m_ptMousePos.y;

	switch (m_eMouseMode)
	{
	case MM_FREE:
		m_ptMousePos = ptPos;
		break;
	case MM_CENTER:
		SetCenterMouse();
		break;
	}

	ComputeRay();
}

void CInput::SetMouseMode(MOUSE_MODE eMode)
{
	m_eMouseMode = eMode;

	if (eMode == MM_FREE)
	{
		SetCenterMouse();
	}
}

void CInput::SetCenterMouse()
{
	RECT	rc;
	GetWindowRect(m_hWnd, &rc);

	POINT	ptPos;
	ptPos.x = (rc.right - rc.left) / 2;
	ptPos.y = (rc.bottom - rc.top) / 2;

	RESOLUTION	rs = SINGLE(CDevice)->GetResolution();

	float	x, y;
	x = rs.fWidth / (rc.right - rc.left);
	y = rs.fHeight / (rc.bottom - rc.top);

	m_ptMousePos.x = ptPos.x * x;
	m_ptMousePos.y = ptPos.y * y;

	ClientToScreen(m_hWnd, &ptPos);

	SetCursorPos(ptPos.x, ptPos.y);
}

void CInput::SetWheel(short sWheel)
{
	m_sWheel = sWheel / 120;
}

void CInput::Clear()
{
	m_sWheel = 0;
}
