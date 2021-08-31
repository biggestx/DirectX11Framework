
#include "BiggestClient.h"
#include "Engine/Core/Core.h"
#include "Engine/Core/Timer.h"
#include "Engine/Core/TimerManager.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Scene/Scene.h"
#include "Script\MainSceneScript.h"

USING_BIGGEST

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
_In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{

	if (!SINGLE(CCore)->Init(RT_NORMAL, L"TestGame", L"TestGame", hInstance, nCmdShow, IDI_ICON1, true))
	{
		DESTROY(CCore);
		return 0;
	}

	CTimer* pTimer = SINGLE(CTimerManager)->CreateTimer("DefaultTimer");
	pTimer->Init();
	SAFE_RELEASE(pTimer);

	CScene* pCurScene = SINGLE(CSceneManager)->GetCurrentScene();
	CMainSceneScript* pScript = (CMainSceneScript*)pCurScene->CreateScript<CMainSceneScript>("MainSceneScript");
	
	pScript->Load();

	SINGLE(CCore)->ShowMainWindow();

	

	int iRet = SINGLE(CCore)->Run();

	DESTROY(CCore);

	return iRet;
}

