#pragma once

#include "Engine/Script/Script.h"


USING_BIGGEST

class CMainSceneScript : 
	public CScript
{

public:
	void CreateCharacter();
	void CreateTerrain();
	void CreateLight();

private:
	void CreatePointLight(const Vec3& vPos, float fRadius);

public:
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);

public:
	void Load();

public:
	CMainSceneScript();
	~CMainSceneScript();
};


