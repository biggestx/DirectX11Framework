#include "Material.h"
#include "Texture.h"
#include "../Rendering/Shader/Shader.h"

USING_BIGGEST

CMaterial::CMaterial() :
m_vAmbient(0.f, 0.f, 0.f),
m_vDiffuse(0.f, 0.f, 0.f),
m_vEmissive(0.f, 0.f, 0.f),
m_pShader(NULL)
{
	memset(m_szTech, 0, 64);
}


CMaterial::~CMaterial()
{
	Safe_Release_Map(m_mapTexture);

}

bool CMaterial::Init()
{
	m_RF = RF_MATERIAL;

	return true;
}

bool CMaterial::CreateResource(TCHAR* pFileName, const string& strPathKey)
{

	return true;
}

void CMaterial::AddTexture(char* szName, class CTexture* pTexture)
{
	m_mapTexture.insert(make_pair(szName, pTexture));
}

void CMaterial::AddPass(int iPass)
{
	m_passList.push_back(iPass);
}

void CMaterial::Render()
{

}
void CMaterial::Render(const DWORD& dwSubset)
{

}

void CMaterial::ConstantTable()
{
	for (map<string, CTexture*>::iterator iter = m_mapTexture.begin();
		iter != m_mapTexture.end(); ++iter)
	{
		m_pShader->SetData(iter->first, iter->second->GetResourceView());
	}
}