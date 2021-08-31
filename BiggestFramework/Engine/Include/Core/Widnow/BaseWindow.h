#pragma once

#include "../Ref.h"

BIGGEST_BEGIN

class BIGGEST_DLL CBaseWindow :
	public CRef
{
private:
	friend class CCore;

private:
	HWND		m_hWnd;
	HINSTANCE	m_hInst;
	RESOLUTION_TYPE	m_eRSType;
	int			m_iShow;

public:
	HWND GetWindowHandle();

public:
	bool Create(RESOLUTION_TYPE jrt, TCHAR* pTitle, TCHAR* pClass,
		HINSTANCE hInst, int iShow,
		int iIconID);
	void Show();

private:
	ATOM MyRegisterClass(HINSTANCE hInstance, TCHAR* pClass, int iIconID);
	BOOL InitInstance(TCHAR*, TCHAR*, HINSTANCE, int);

private:
	CBaseWindow();
	~CBaseWindow();
};

BIGGEST_END