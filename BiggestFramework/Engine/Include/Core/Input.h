#pragma once
#include "Ref.h"
#include <stdarg.h>

BIGGEST_BEGIN

class BIGGEST_DLL CInput :
	public CRef
{
private:
	map<DWORD, INPUTINFO>	m_mapKey;
	POINT		m_ptMousePos;
	POINT		m_ptMovePos;
	HWND		m_hWnd;
	MOUSE_MODE	m_eMouseMode;
	short		m_sWheel;
	RAY			m_tViewRay;
	RAY			m_tWorldRay;
	class CGameObject*	m_pMouse;
	class CColliderRay*	m_pRay;

public:
	short GetWheel()
	{
		return m_sWheel;
	}

	POINT GetMosuePos()
	{
		return m_ptMousePos;
	}

	POINT GetMouseMove()
	{
		return m_ptMovePos;
	}

	MOUSE_MODE GetMouseMode()
	{
		return m_eMouseMode;
	}

	RAY GetViewRay()
	{
		return m_tViewRay;
	}

	RAY GetWorldRay()
	{
		return m_tWorldRay;
	}

public:
	bool Init(HWND hWnd);
	void Update(float fTime);
	void CalculateMouse();
	void SetMouseMode(MOUSE_MODE eMode);
	void SetCenterMouse();
	void SetWheel(short sWheel);
	void Clear();
	void ComputeRay();
	RAY ConvertSpaceRay(const RAY& ray, Matrix& matSpace);

public:
	bool GetKeyPress(const DWORD& key);
	bool GetKeyDown(const DWORD& key);
	bool GetKeyUp(const DWORD& key);
public:

	bool CreateKey(DWORD key)
	{
		if (FindKey(key) != NULL)
			return false;

		INPUTINFO info;
		info.bUp = false;
		info.bDown = false;
		info.bPress = false;
		m_mapKey.insert(make_pair(key, info));

		return true;
	}

	PINPUTINFO FindKey(const DWORD& strKey);

	DECLARE_SINGLE(CInput)
};

BIGGEST_END