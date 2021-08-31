#include "FilePathManager.h"

USING_BIGGEST

DEFINITION_SINGLE(CFilePathManager)

CFilePathManager::CFilePathManager() :
m_bInitialize(false)
{
}

CFilePathManager::~CFilePathManager()
{
}

bool CFilePathManager::Init()
{
	if (m_bInitialize)
		return true;

	m_bInitialize = true;

	wchar_t	strPath[MAX_PATH] = {};

	GetCurrentDirectory(MAX_PATH - 1, strPath);

	for (int i = lstrlen(strPath) - 1; i >= 0; --i)
	{
		if (strPath[i] == '\\')
		{
			memset(strPath + (i + 1), 0, sizeof(wchar_t) * (MAX_PATH - (i + 1)));
			lstrcat(strPath, L"Bin\\");
			break;
		}
	}

	m_mapPath.insert(make_pair(ROOT_PATH_KEY, strPath));

	// Add Shader Path
	CreatePath(SHADER_PATH_KEY, L"Shader\\");
	CreatePath(MESH_PATH_KEY, L"Mesh\\");
	CreatePath(TEXTURE_PATH_KEY, L"Texture\\");

	// Add Texture Path
	//CreatePath(TEXTURE_PATH_KEY,)
 
	//// 리소스 패스 추가
	//CreatePath(RESOURCES_PATH_KEY, L"Resources\\");
	//// 메쉬 패스 추가
	//CreatePath(MESH_PATH_KEY, L"Mesh\\", RESOURCES_PATH_KEY);

	//// 텍스쳐 패스 추가
	//CreatePath(TEXTURE_PATH_KEY, L"Textures\\", RESOURCES_PATH_KEY);

	//// Shader 패스 추가
	//CreatePath(SHADER_PATH_KEY, L"Shader\\");

	return true;
}

bool CFilePathManager::CreatePath(const string& strKey, wchar_t* pPath,
	const string& strBasePathKey)
{
	if (FindPath(strKey))
		return false;

	wstring	strPath = FindPath(strBasePathKey);

	strPath += pPath;

	m_mapPath.insert(make_pair(strKey, strPath));

	return true;
}

const wchar_t* CFilePathManager::FindPath(const string& strKey)
{
	map<string, wstring>::iterator	iter = m_mapPath.find(strKey);

	if (iter == m_mapPath.end())
		return NULL;

	return iter->second.c_str();
}

const char* CFilePathManager::FindPathToMultiByte(const string& strKey)
{
	map<string, wstring>::iterator	iter = m_mapPath.find(strKey);

	if (iter == m_mapPath.end())
		return NULL;

	memset(m_strMultiByte, 0, sizeof(char) * MAX_PATH);
	WideCharToMultiByte(CP_ACP, 0, iter->second.c_str(), -1,
		m_strMultiByte, sizeof(wchar_t) * iter->second.length(), NULL, NULL);

	return m_strMultiByte;
}
