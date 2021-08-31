#pragma once
#include "../Component/Component.h"

BIGGEST_BEGIN

class BIGGEST_DLL CScript :
	public CComponent
{
public:
		virtual bool Init();
		virtual void Update(float fTime);
		virtual void LateUpdate(float fTime);
		virtual CScript* Clone();

protected:
	CScript();
	CScript(const CScript& script);

public:
	virtual ~CScript();
};

BIGGEST_END