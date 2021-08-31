#include "MainSceneScript.h"
#include "Engine/Rendering/Shader/Shader.h"
#include "Engine/Rendering/RenderManager.h"
#include "Engine/Resource/Buffer.h"
#include "Engine/Resource/ResourcesManager.h"
#include "Engine/GameObject/GameObject.h"
#include "Engine/Component/Renderer.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Scene/Layer.h"
#include "Engine/Component/Transform.h"
#include "Engine/Resource/ResBuffer.h"
#include "Engine/Component/Animation3D.h"
#include "Engine/Component/Camera.h"
#include "Engine/Script/ThirdPersonCam.h"
#include "Engine/Component/DirectionalLight.h"
#include "Engine/Component/PointLight.h"
#include "Engine/Resource/Material.h"
#include "Engine/Resource/Texture.h"
#include "Engine/Script/CharacterController.h"

#include <crtdbg.h>

#include <fstream>

CMainSceneScript::CMainSceneScript()
{
}


CMainSceneScript::~CMainSceneScript()
{
}


bool CMainSceneScript::Init()
{
	if (!CScript::Init())
		return false;

	return true;
}
void CMainSceneScript::Update(float fTime)
{

}
void CMainSceneScript::LateUpdate(float fTime)
{

}



void CMainSceneScript::Load()
{
	CreateCharacter();
	CreateTerrain();
	CreateLight();
}

void CMainSceneScript::CreateCharacter()
{
	list<CMaterial*> materialList;
	CAnimation3D* pAnimation = NULL;

	const char* PATH_FBX_ZOMBIE = "PhychoZombie\\Phychozomb.fbx";
	const char* PATH_FBX_BASEMESH = "BaseMesh_Anim.fbx";

	CGameObject* pObj = CGameObject::Create("TestObject");
	pObj->GetTransform()->SetWorldPos(0.f, 0.f, 0.f);
	pObj->GetTransform()->SetScale(0.01f, 0.01f, 0.01f);
	//pObj->GetTransform()->SetLocalRotation(90.f, 0.f, 0.f);

	CScene* pScene = SINGLE(CSceneManager)->GetCurrentScene();
	CLayer* pLayer = pScene->FindLayer();

	if (pAnimation)
		pObj->AddComponent(pAnimation);

	CBuffer* pBuffer = SINGLE(CResourcesManager)->CreateBufferFromFBX(PATH_FBX_ZOMBIE, &materialList, &pAnimation);
	CRenderer* pRenderer = new CRenderer;
	pRenderer->Init();
	pRenderer->SetResource(pBuffer);
	for (list<CMaterial*>::iterator iter = materialList.begin(); iter != materialList.end(); ++iter)
		pRenderer->AddMaterial((*iter));
	pObj->AddComponent(pRenderer);

	pLayer->AddObject(pObj);

	// Set Third Person Camera
	CGameObject* pCamObj = SINGLE(CSceneManager)->GetCurrentScene()->GetMainCamera();
	CCamera* pCamComponent = (CCamera*)pCamObj->FindComponent(CT_CAMERA);
	pCamComponent->SetViewType(CVT_LOOKAT);

	CThirdPersonCam* pThirdPersonCam = CThirdPersonCam::Create("MainThirdPersonCam");
	pThirdPersonCam->SetCameraComponent(pCamComponent);
	pThirdPersonCam->SetLookAtObject(pObj);
	pCamObj->AddComponent(pThirdPersonCam);

	// Set Character Controller;
	CCharacterController* pCharacterController = CCharacterController::Create("CharacterController");
	pObj->AddComponent(pCharacterController);


	SAFE_RELEASE(pThirdPersonCam);
	SAFE_RELEASE(pAnimation);
	SAFE_RELEASE(pObj);
	//SAFE_RELEASE(pRenderer);
}

void CMainSceneScript::CreateTerrain()
{
	CGameObject* pObj = SINGLE(CResourcesManager)->CreateTerrainObject(100, 100);
	if (!pObj)
		return;

	pObj->GetTransform()->SetWorldPos(0.f, 0.f, 0.f);
	pObj->GetTransform()->SetScale(50.f, 50.f, 50.f);

	CRenderer* pRenderer = (CRenderer*)pObj->FindComponent(CT_RENDERER);
	CMaterial* pMaterial = pRenderer->GetMaterial(0);
	
	CTexture* pTexture = CTexture::Create("GrassTex");
	pTexture->CreateResource(L"Grass.png", TEXTURE_PATH_KEY);
	pMaterial->AddTexture(SHADER_HANDLE_TERRAINFX_DIFFUSETEX, pTexture);

	pTexture = CTexture::Create("HeightTex");
	pTexture->CreateResource(L"Height.png", TEXTURE_PATH_KEY);
	pMaterial->AddTexture(SHADER_HANDLE_TERRAINFX_HEIGHTTEX, pTexture);
	
	CScene* pScene = SINGLE(CSceneManager)->GetCurrentScene();
	CLayer* pLayer = pScene->FindLayer();

	pLayer->AddObject(pObj);
	SAFE_RELEASE(pObj);
}


void CMainSceneScript::CreateLight()
{
	CScene* pScene = SINGLE(CSceneManager)->GetCurrentScene();
	CLayer* pLayer = pScene->FindLayer();

	// directional light
	CGameObject* pObj = CGameObject::Create("DirectionalLightObject");
	CDirectionalLight* pLight = CDirectionalLight::Create("");
	pLight->SetLightColor(XMFLOAT3(1.f, 1.f, 1.f));
	pLight->SetDirection(XMFLOAT3(0.f, -1.f, 0.f));
	pObj->AddComponent(pLight);
	pLayer->AddObject(pObj);

	SAFE_RELEASE(pLight);
	SAFE_RELEASE(pObj);



	// point light
	CreatePointLight(Vec3(10.f, 0.f, 0.f), 10.f);
	CreatePointLight(Vec3(-10.f, 0.f, 0.f), 10.f);
}


void CMainSceneScript::CreatePointLight(const Vec3& vPos, float fRadius)
{
	CScene* pScene = SINGLE(CSceneManager)->GetCurrentScene();
	CLayer* pLayer = pScene->FindLayer();

	auto pObj = CGameObject::Create("PointLight");
	CPointLight* pPointLight = CPointLight::Create("");
	pObj->GetTransform()->SetWorldPos(vPos.x, vPos.y, vPos.z);

	pPointLight->SetRadius(fRadius);
	pObj->AddComponent(pPointLight);
	pLayer->AddObject(pObj);

	SAFE_RELEASE(pPointLight);
	SAFE_RELEASE(pObj);

}
