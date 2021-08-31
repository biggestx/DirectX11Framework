#include "BaseWindow.h"
#include "../Core.h"

USING_BIGGEST

CBaseWindow::CBaseWindow()
{
}

CBaseWindow::~CBaseWindow()
{
}

bool CBaseWindow::Create(RESOLUTION_TYPE jrt, TCHAR* pTitle, TCHAR* pClass,
	HINSTANCE hInst, int iShow,
	int iIconID)
{
	m_eRSType = jrt;
	MyRegisterClass(hInst, pClass, iIconID);

	InitInstance(pTitle, pClass, hInst, iShow);

	return true;
}

ATOM CBaseWindow::MyRegisterClass(HINSTANCE hInstance, TCHAR* pClass, int iIconID)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = CCore::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(iIconID));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = pClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(iIconID));

	return RegisterClassEx(&wcex);
}

BOOL CBaseWindow::InitInstance(TCHAR* pTitle, TCHAR* pClass, HINSTANCE hInstance,
	int iShow)
{
	m_hInst = hInstance; 

	m_hWnd = CreateWindow(pClass, pTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!m_hWnd)
	{
		return FALSE;
	}

	RECT	rc = { 0, 0, g_tResolution[m_eRSType].fWidth, g_tResolution[m_eRSType].fHeight };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, rc.right - rc.left, rc.bottom - rc.top,
		SWP_NOMOVE | SWP_NOZORDER);

	m_iShow = iShow;

	return TRUE;
}

void CBaseWindow::Show()
{
	ShowWindow(m_hWnd, m_iShow);
	UpdateWindow(m_hWnd);
}

HWND CBaseWindow::GetWindowHandle()
{
	return m_hWnd;
}
