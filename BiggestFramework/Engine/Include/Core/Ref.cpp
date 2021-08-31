#include "Ref.h"


USING_BIGGEST

CRef::CRef() :
m_iRefCount(1),
m_bAlive(true),
m_bEnable(true)
{
}

CRef::~CRef()
{
}

void CRef::SetTag(const string& strTag)
{
	m_strTag = strTag;
}

string CRef::GetTag()
{
	return m_strTag;
}

void CRef::AddRef()
{
	++m_iRefCount;
}

void CRef::Release()
{
	--m_iRefCount;

	if (m_iRefCount <= 0)
		delete	this;
}

void CRef::Remove()
{
	m_iRefCount = 0;
	delete	this;
}

string CRef::GetTypeName()
{
	return m_strTypeName;
}

bool CRef::GetEnable()
{
	return m_bEnable;
}

bool CRef::GetAlive()
{
	return m_bAlive;
}

void CRef::Death()
{
	m_bAlive = false;
}

void CRef::Enable(bool bEnable)
{
	m_bEnable = bEnable;
}
