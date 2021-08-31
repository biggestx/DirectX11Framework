#pragma once

#include "Ref.h"


BIGGEST_BEGIN

class BIGGEST_DLL CCore :
	public CRef
{
private:
	class CBaseWindow*	m_pWindow;
	//class CJJiSplashWindow*	m_pSplashWindow;
	bool	m_bMsgLoop;


private:
	class CTimer*		m_pTimer;

public:
	bool CreateSplashWindow(HINSTANCE hInst, TCHAR* pClass, TCHAR* pBackGroundName,
		const int r = -1, const int g = -1, const int b = -1);
	bool Init(RESOLUTION_TYPE rt, TCHAR* pTitle, TCHAR* pClass, HINSTANCE hInst, int iShow,
		int iIconID, bool bWindowMode);
	int Run();
	int SplashRun();
	void EndSplashLoop();
	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	void EndLoop();
	void DestroySplash();
	void ShowMainWindow();

private:
	void GameModule();
	SCENE_CHANGE Update(float fTime);
	SCENE_CHANGE LateUpdate(float fTime);
	void Render(float fTime);

	DECLARE_SINGLE(CCore)
};

BIGGEST_END 