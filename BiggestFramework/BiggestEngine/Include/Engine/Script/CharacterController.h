#pragma once
#include "Script.h"

BIGGEST_BEGIN

class BIGGEST_DLL CCharacterController :
	public CScript
{
	CREATE_INSTANCE_TAG(CCharacterController)

public:
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual CScript* Clone();

public:
	CCharacterController();
	~CCharacterController();
};

BIGGEST_END