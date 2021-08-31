#include "Terrain.h"
#include "Renderer.h"
#include "../Resource/ResourcesManager.h"
#include "../Resource/ResBuffer.h"
#include "../Rendering/Shader/Shader.h"
#include "../Rendering/RenderManager.h"

USING_BIGGEST


CTerrain::CTerrain() :
m_pResBuffer(NULL)
{
}


CTerrain::~CTerrain()
{
}


bool CTerrain::Init()
{
	m_eComType = CT_TERRAIN;

	return true;
}

void CTerrain::CreateTerrain(int iVertexX, int iVertexY)
{
	m_MeshData.Vertices.clear();
	m_MeshData.Indices.clear();

	m_pResBuffer = (CResBuffer*)SINGLE(CResourcesManager)->CreateTerrainResource(iVertexX, iVertexY,m_MeshData);
}

void CTerrain::Update(float fTime)
{

}

void CTerrain::LateUpdate(float fTime)
{
	 
}

void CTerrain::Render(float fTime)
{

}

CComponent* CTerrain::Clone()
{
	return new CTerrain(*this);
}

void CTerrain::ConstantTable()
{

}