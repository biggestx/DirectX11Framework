#pragma once
#include "Component.h"

BIGGEST_BEGIN


class CShader;
class CVertexShader;
class CPixelShader;
class CEffectFX;
class CBaseResource;
class CMaterial;

class BIGGEST_DLL CRenderer :
	public CComponent
{
	CREATE_INSTANCE_TAG(CRenderer)
private:
	CBaseResource*	m_pRes;
	list<CMaterial*>	m_materialList;



	Matrix m_matWorld;
	Matrix m_matView;
	Matrix m_matProj;
	Matrix m_matWVP;


public:
	void SetWVP(Matrix& matWVP)
	{
		m_matWVP = matWVP;
	}

	void SetResource(CBaseResource* pRes)
	{
		m_pRes = pRes;
	}
	void AddMaterial(CMaterial* pMaterial);

public:
	CMaterial* GetMaterial(int idx)
	{
		if (idx >= m_materialList.size())
			return NULL;
		int i = 0;
		for (list<CMaterial*>::iterator iter = m_materialList.begin(); iter != m_materialList.end(); ++iter, ++i)
		{
			if (i == idx)
				return (*iter);
		}


		return NULL;
	}

public:
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CComponent* Clone();
	virtual void ConstantTable();

public:
	CRenderer();
	CRenderer(const CRenderer& pRenderer);
	virtual ~CRenderer();
};

BIGGEST_END