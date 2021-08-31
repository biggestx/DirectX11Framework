#include "HierarchicalBuffer.h"

USING_BIGGEST

CHierarchicalBuffer::CHierarchicalBuffer()
{
}


CHierarchicalBuffer::~CHierarchicalBuffer()
{
	Safe_Release_Map(m_mapResources);
}

bool CHierarchicalBuffer::Init()
{


	return true;
}

bool CHierarchicalBuffer::CreateResource(TCHAR* pFileName, const string& strPathKey)
{

	return true;
}

void CHierarchicalBuffer::AddResource(string strKey, CBaseResource* pBuffer)
{
	m_mapResources.insert(make_pair(strKey, pBuffer));
}

void CHierarchicalBuffer::Render()
{
	map<string, CBaseResource*>::iterator iter;
	for (iter = m_mapResources.begin(); iter != m_mapResources.end(); ++iter)
	{
		iter->second->Render();
	}
}
void CHierarchicalBuffer::Render(const DWORD& dwSubset)
{

}