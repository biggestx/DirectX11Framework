#pragma once
#include "Component.h"

BIGGEST_BEGIN

class BIGGEST_DLL CTerrain :
	public CComponent
{
	CREATE_INSTANCE_TAG(CTerrain)

private:
	int m_iVertexCountX;
	int m_iVertexCountY;
	class CResBuffer* m_pResBuffer;
	MeshData<VertexPNT> m_MeshData;

	class CShader* m_pShader;

public:
	CResBuffer* GetTerrainBuffer()
	{
		return m_pResBuffer;
	}

public:
	void CreateTerrain(int iVertexX, int iVertexY);

public:
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CComponent* Clone();
	virtual void ConstantTable();


public:
	CTerrain();
	~CTerrain();
};

BIGGEST_END