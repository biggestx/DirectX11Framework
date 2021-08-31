#include "Script.h"

USING_BIGGEST

CScript::CScript()
{
}

CScript::CScript(const CScript& script) :
CComponent(script)
{
	*this = script;
}

CScript::~CScript()
{
}

bool CScript::Init()
{
	m_eComType = CT_SCRIPT;

	return true;
}

void CScript::Update(float fTime)
{
}

void CScript::LateUpdate(float fTime)
{
}

CScript* CScript::Clone()
{
	return new CScript(*this);
}
