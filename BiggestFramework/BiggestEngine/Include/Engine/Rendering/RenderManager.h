#pragma once

#include "../Core/Ref.h"

BIGGEST_BEGIN

class BIGGEST_DLL CRenderManager :
	public CRef
{
private:
	RESOLUTION_TYPE	m_eRSType;
	list<class CGameObject*>	m_RenderList[RF_MAX];
	list<class CLight*>			m_lightList;
	map<string, class CShader*>	m_mapShader;
	RENDER_MODE					m_eRenderMode;

	map<string, class CMultiRenderTarget*> m_mapMRT;

	ID3D11DepthStencilState* m_pLightAccDepthState;

private:

	class CResBuffer* m_pFullScreenQuad;
	class CResBuffer* m_pPointLightSphere;

public:
	RENDER_MODE GetRenderMode()
	{
		return m_eRenderMode;
	}

	void SetRenderMode(RENDER_MODE eMode)
	{
		m_eRenderMode = eMode;
	}

public:
	static bool __cdecl SortNormalObject(CGameObject* pObj1, CGameObject* pObj2);
	static bool __cdecl SortAlphaObject(CGameObject* pObj1, CGameObject* pObj2);
	static bool __cdecl SortUI(CGameObject* pObj1, CGameObject* pObj2);

public:

	void AddLight(CLight* pLight)
	{
		if(m_eRenderMode == RM_DEFERRED)
			m_lightList.push_back(pLight);
	}

public:
	bool Init(RESOLUTION_TYPE eType);
	void Render(float fTime);
	void AddRenderObject(CGameObject* pObject, RENDERING_FLAG eFlag);
	CShader* LoadShader(const string& strKey, wchar_t* pFileName);
	CShader* FindShader(const string& strKey);

private:
	void RenderForward(float fTime);
	void RenderDeferred(float fTime);
	void RenderGBuffer(float fTime);
	void RenderLightAcc(float fTime);
	void RenderCombine();

private:
	bool InitRenderTarget();
	bool InitLightDirBuffer();
	bool InitPointLightMesh();

private:
	bool InitShader();
	void CreateShareHandle(CShader* pShader);
	void CreateTerrainHandle(CShader* pShader);
	void CreateEffectHandle(CShader* pShader);
	void CreateUIHandle(CShader* pShader);
	void CreateLightHandle(CShader* pShader);

	void CreateDefaultHandle(CShader* pShader);
	void CreateDeferredRenderingHandle(CShader* pShader);

	DECLARE_SINGLE(CRenderManager)
};

BIGGEST_END