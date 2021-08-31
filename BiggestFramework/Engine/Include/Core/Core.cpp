#include "Core.h"
#include "Widnow\BaseWindow.h"
#include "Device.h"
#include "Timer.h"
#include "TimerManager.h"
#include "Input.h"
#include "../Scene/SceneManager.h"
#include "../Rendering/RenderManager.h"
#include "FilePathManager.h"
#include "../Resource/ResourcesManager.h"

USING_BIGGEST

DEFINITION_SINGLE(CCore)

CCore::CCore() :
m_pWindow(NULL),
m_pTimer(NULL),
//m_pSplashWindow(NULL),
m_bMsgLoop(true)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(893);
	AllocConsole();

#endif // _DEBUG
}


CCore::~CCore()
{
	EndLoop();
	SAFE_RELEASE(m_pTimer);
	DESTROY(CSceneManager);
	DESTROY(CRenderManager);
	DESTROY(CTimerManager);
	DESTROY(CFilePathManager);
	DESTROY(CResourcesManager);
	//DESTROY(CColliderManager);
	DESTROY(CInput);
	DESTROY(CDevice);
	//DESTROY(CThreadManager);
	//SAFE_RELEASE(m_pSplashWindow);
	SAFE_RELEASE(m_pWindow);
#ifdef _DEBUG
	FreeConsole();
#endif // _DEBUG

}

bool CCore::CreateSplashWindow(HINSTANCE hInst, TCHAR* pClass,
	TCHAR* pBackGroundName,
	const int r, const int g, const int b)
{
	return true;
}

void CCore::ShowMainWindow()
{
	m_pWindow->Show();
}

bool CCore::Init(RESOLUTION_TYPE rt, TCHAR* pTitle, TCHAR* pClass,
	HINSTANCE hInst, int iShow,
	int iIconID, bool bWindowMode)
{

	m_pWindow = new CBaseWindow;

	m_pWindow->Create(rt, pTitle, pClass, hInst, iShow, iIconID);

	if (!SINGLE(CDevice)->InitDevice(m_pWindow->GetWindowHandle(),
		g_tResolution[rt]))
		return false;

	if (!SINGLE(CTimerManager)->Init())
		return false;

	m_pTimer = SINGLE(CTimerManager)->CreateTimer("DefaultTimer");

	if (!SINGLE(CFilePathManager)->Init())
		return false;

	if (!SINGLE(CRenderManager)->Init(rt))
		return false;

	if (!SINGLE(CResourcesManager)->Init())
		return false;

	if (!SINGLE(CInput)->Init(m_pWindow->GetWindowHandle()))
		return false;

	if (!SINGLE(CSceneManager)->Init())
		return false;

	return true;
}

int CCore::Run()
{
	MSG	msg;

	if (m_pWindow)
	{
		while (m_bMsgLoop)
		{
			// PeekMessage 함수는 메세지가 있을 경우에는 안으로 들어가서 처리하고
			// 메세지가 없을 경우에는 바로 리턴되서 else로 들어간다.
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			// 메세지가 없을때는 여기로 들어온다.
			else
			{
				GameModule();
			}
		}

		return (int)msg.wParam;
	}

	else
		GameModule();

	return 0;
}

void CCore::DestroySplash()
{
}

void CCore::EndSplashLoop()
{
}

int CCore::SplashRun()
{
	return 0;
}

void CCore::GameModule()
{
	m_pTimer->Update();

	SINGLE(CInput)->Update(m_pTimer->GetDeltaTime());

	if (Update(m_pTimer->GetDeltaTime()) != SC_NONE)
	{
		SINGLE(CInput)->Clear();
		return;
	}
	if (LateUpdate(m_pTimer->GetDeltaTime()) != SC_NONE)
	{
		SINGLE(CInput)->Clear();
		return;
	}

	Render(m_pTimer->GetDeltaTime());

	SINGLE(CInput)->Clear();
}

SCENE_CHANGE CCore::Update(float fTime)
{
	return SINGLE(CSceneManager)->Update(fTime);
}

SCENE_CHANGE CCore::LateUpdate(float fTime)
{
	return SINGLE(CSceneManager)->LateUpdate(fTime);
}

void CCore::Render(float fTime)
{
	// 렌더링 오브젝트 추가
	SINGLE(CSceneManager)->AddRenderingObject();

	SINGLE(CRenderManager)->Render(fTime);
}

void CCore::EndLoop()
{
	m_bMsgLoop = false;
}

LRESULT CALLBACK CCore::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		}
		break;
	case WM_MOUSEWHEEL:
		SINGLE(CInput)->SetWheel(HIWORD(wParam));
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		SINGLE(CCore)->EndLoop();
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
