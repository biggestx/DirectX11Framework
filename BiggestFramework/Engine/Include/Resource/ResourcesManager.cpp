#include "ResourcesManager.h"
#include "Buffer.h"
#include "ResBuffer.h"
#include "../Core/FilePathManager.h"
#include "../Rendering/RenderManager.h"
#include "../Rendering/Shader/Shader.h"
#include "../Core/CustomMath.h"
#include "Material.h"
#include "Texture.h"
#include "HierarchicalBuffer.h"
#include "../Component/Animation3D.h"
#include "../GameObject/GameObject.h"
#include "../Component/Terrain.h"
#include "../Component/Renderer.h"


USING_BIGGEST

DEFINITION_SINGLE(CResourcesManager)

CResourcesManager::CResourcesManager()
//m_bHasAnimation(true),
//m_iTriangleCount(0)
{
}


CResourcesManager::~CResourcesManager()
{

}

bool CResourcesManager::Init()
{

	if (!InitFbxManager())
		return false;

	return true;
}

bool CResourcesManager::InitFbxManager()
{



	return true;
}

void CResourcesManager::CreateCylinder(float bottomRadius, float topRadius, float height, UINT sliceCount,
	UINT stackCount, MeshData<Vertex>& meshData)
{
	meshData.Vertices.clear();
	meshData.Indices.clear();

	//
	// Build Stacks.
	// 

	float stackHeight = height / stackCount;

	// Amount to increment radius as we move up each stack level from bottom to top.
	float radiusStep = (topRadius - bottomRadius) / stackCount;

	UINT ringCount = stackCount + 1;

	// Compute vertices for each stack ring starting at the bottom and moving up.
	for (UINT i = 0; i < ringCount; ++i)
	{
		float y = -0.5f*height + i*stackHeight;
		float r = bottomRadius + i*radiusStep;

		// vertices of ring
		float dTheta = 2.0f*XM_PI / sliceCount;
		for (UINT j = 0; j <= sliceCount; ++j)
		{
			Vertex vertex;

			float c = cosf(j*dTheta);
			float s = sinf(j*dTheta);

			vertex.Position = XMFLOAT3(r*c, y, r*s);

			vertex.TexC.x = (float)j / sliceCount;
			vertex.TexC.y = 1.0f - (float)i / stackCount;

			// Cylinder can be parameterized as follows, where we introduce v
			// parameter that goes in the same direction as the v tex-coord
			// so that the bitangent goes in the same direction as the v tex-coord.
			//   Let r0 be the bottom radius and let r1 be the top radius.
			//   y(v) = h - hv for v in [0,1].
			//   r(v) = r1 + (r0-r1)v
			//
			//   x(t, v) = r(v)*cos(t)
			//   y(t, v) = h - hv
			//   z(t, v) = r(v)*sin(t)
			// 
			//  dx/dt = -r(v)*sin(t)
			//  dy/dt = 0
			//  dz/dt = +r(v)*cos(t)
			//
			//  dx/dv = (r0-r1)*cos(t)
			//  dy/dv = -h
			//  dz/dv = (r0-r1)*sin(t)

			// This is unit length.
			vertex.TangentU = XMFLOAT3(-s, 0.0f, c);

			float dr = bottomRadius - topRadius;
			XMFLOAT3 bitangent(dr*c, -height, dr*s);

			XMVECTOR T = XMLoadFloat3(&vertex.TangentU);
			XMVECTOR B = XMLoadFloat3(&bitangent);
			XMVECTOR N = XMVector3Normalize(XMVector3Cross(T, B));
			XMStoreFloat3(&vertex.Normal, N);

			meshData.Vertices.push_back(vertex);
		}
	}

	// Add one because we duplicate the first and last vertex per ring
	// since the texture coordinates are different.
	UINT ringVertexCount = sliceCount + 1;

	// Compute indices for each stack.
	for (UINT i = 0; i < stackCount; ++i)
	{
		for (UINT j = 0; j < sliceCount; ++j)
		{
			meshData.Indices.push_back(i*ringVertexCount + j);
			meshData.Indices.push_back((i + 1)*ringVertexCount + j);
			meshData.Indices.push_back((i + 1)*ringVertexCount + j + 1);

			meshData.Indices.push_back(i*ringVertexCount + j);
			meshData.Indices.push_back((i + 1)*ringVertexCount + j + 1);
			meshData.Indices.push_back(i*ringVertexCount + j + 1);
		}
	}

	BuildCylinderTopCap(bottomRadius, topRadius, height, sliceCount, stackCount, meshData);
	BuildCylinderBottomCap(bottomRadius, topRadius, height, sliceCount, stackCount, meshData);
}

void CResourcesManager::BuildCylinderTopCap(float bottomRadius, float topRadius, float height,
	UINT sliceCount, UINT stackCount, MeshData<Vertex>& meshData)
{
	UINT baseIndex = (UINT)meshData.Vertices.size();

	float y = 0.5f*height;
	float dTheta = 2.0f*XM_PI / sliceCount;

	// Duplicate cap ring vertices because the texture coordinates and normals differ.
	for (UINT i = 0; i <= sliceCount; ++i)
	{
		float x = topRadius*cosf(i*dTheta);
		float z = topRadius*sinf(i*dTheta);

		// Scale down by the height to try and make top cap texture coord area
		// proportional to base.
		float u = x / height + 0.5f;
		float v = z / height + 0.5f;

		meshData.Vertices.push_back(Vertex(x, y, z, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v));
	}

	// Cap center vertex.
	meshData.Vertices.push_back(Vertex(0.0f, y, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f));

	// Index of center vertex.
	UINT centerIndex = (UINT)meshData.Vertices.size() - 1;

	for (UINT i = 0; i < sliceCount; ++i)
	{
		meshData.Indices.push_back(centerIndex);
		meshData.Indices.push_back(baseIndex + i + 1);
		meshData.Indices.push_back(baseIndex + i);
	}
}


void CResourcesManager::BuildCylinderBottomCap(float bottomRadius, float topRadius, float height,
	UINT sliceCount, UINT stackCount, MeshData<Vertex>& meshData)
{
	// 
	// Build bottom cap.
	//

	UINT baseIndex = (UINT)meshData.Vertices.size();
	float y = -0.5f*height;

	// vertices of ring
	float dTheta = 2.0f*XM_PI / sliceCount;
	for (UINT i = 0; i <= sliceCount; ++i)
	{
		float x = bottomRadius*cosf(i*dTheta);
		float z = bottomRadius*sinf(i*dTheta);

		// Scale down by the height to try and make top cap texture coord area
		// proportional to base.
		float u = x / height + 0.5f;
		float v = z / height + 0.5f;

		meshData.Vertices.push_back(Vertex(x, y, z, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v));
	}

	// Cap center vertex.
	meshData.Vertices.push_back(Vertex(0.0f, y, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f));

	// Cache the index of center vertex.
	UINT centerIndex = (UINT)meshData.Vertices.size() - 1;

	for (UINT i = 0; i < sliceCount; ++i)
	{
		meshData.Indices.push_back(centerIndex);
		meshData.Indices.push_back(baseIndex + i);
		meshData.Indices.push_back(baseIndex + i + 1);
	}
}

void CResourcesManager::CreateBox(float width, float height, float depth, MeshData<Vertex>& meshData)
{
	//
	// Create the vertices.
	//

	Vertex v[24];

	float w2 = 0.5f*width;
	float h2 = 0.5f*height;
	float d2 = 0.5f*depth;

	// Fill in the front face vertex data.
	v[0] = Vertex(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[1] = Vertex(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[2] = Vertex(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[3] = Vertex(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the back face vertex data.
	v[4] = Vertex(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[5] = Vertex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[6] = Vertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[7] = Vertex(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the top face vertex data.
	v[8] = Vertex(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[9] = Vertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[10] = Vertex(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[11] = Vertex(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the bottom face vertex data.
	v[12] = Vertex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[13] = Vertex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[14] = Vertex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[15] = Vertex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the left face vertex data.
	v[16] = Vertex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[17] = Vertex(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[18] = Vertex(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[19] = Vertex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// Fill in the right face vertex data.
	v[20] = Vertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[21] = Vertex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[22] = Vertex(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	v[23] = Vertex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

	meshData.Vertices.assign(&v[0], &v[24]);

	//
	// Create the indices.
	//

	UINT i[36];

	// Fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Fill in the back face index data
	i[6] = 4; i[7] = 5; i[8] = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Fill in the top face index data
	i[12] = 8; i[13] = 9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// Fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// Fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// Fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	meshData.Indices.assign(&i[0], &i[36]);
}

CBuffer* CResourcesManager::CreateSphere(float radius, UINT sliceCount, UINT stackCount)
{
	MeshData<Vertex> meshData;

	meshData.Vertices.clear();
	meshData.Indices.clear();

	//
	// Compute the vertices stating at the top pole and moving down the stacks.
	//

	// Poles: note that there will be texture coordinate distortion as there is
	// not a unique point on the texture map to assign to the pole when mapping
	// a rectangular texture onto a sphere.
	Vertex topVertex(0.0f, +radius, 0.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	Vertex bottomVertex(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);


	meshData.Vertices.push_back(topVertex);

	float phiStep = XM_PI / stackCount;
	float thetaStep = 2.0f*XM_PI / sliceCount;

	// Compute vertices for each stack ring (do not count the poles as rings).
	for (UINT i = 1; i <= stackCount - 1; ++i)
	{
		float phi = i*phiStep;

		// Vertices of ring.
		for (UINT j = 0; j <= sliceCount; ++j)
		{
			float theta = j*thetaStep;

			Vertex v;

			// spherical to cartesian
			v.Position.x = radius*sinf(phi)*cosf(theta);
			v.Position.y = radius*cosf(phi);
			v.Position.z = radius*sinf(phi)*sinf(theta);

			// Partial derivative of P with respect to theta
			v.TangentU.x = -radius*sinf(phi)*sinf(theta);
			v.TangentU.y = 0.0f;
			v.TangentU.z = +radius*sinf(phi)*cosf(theta);

			XMVECTOR T = XMLoadFloat3(&v.TangentU);
			XMStoreFloat3(&v.TangentU, XMVector3Normalize(T));

			XMVECTOR p = XMLoadFloat3(&v.Position);
			XMStoreFloat3(&v.Normal, XMVector3Normalize(p));

			v.TexC.x = theta / XM_2PI;
			v.TexC.y = phi / XM_PI;

			meshData.Vertices.push_back(v);
		}
	}

	meshData.Vertices.push_back(bottomVertex);


	//
	// Compute indices for top stack.  The top stack was written first to the vertex buffer
	// and connects the top pole to the first ring.
	//

	for (UINT i = 1; i <= sliceCount; ++i)
	{
		meshData.Indices.push_back(0);
		meshData.Indices.push_back(i + 1);
		meshData.Indices.push_back(i);
	}

	//
	// Compute indices for inner stacks (not connected to poles).
	//

	// Offset the indices to the index of the first vertex in the first ring.
	// This is just skipping the top pole vertex.
	UINT baseIndex = 1;
	UINT ringVertexCount = sliceCount + 1;
	for (UINT i = 0; i < stackCount - 2; ++i)
	{
		for (UINT j = 0; j < sliceCount; ++j)
		{
			meshData.Indices.push_back(baseIndex + i*ringVertexCount + j);
			meshData.Indices.push_back(baseIndex + i*ringVertexCount + j + 1);
			meshData.Indices.push_back(baseIndex + (i + 1)*ringVertexCount + j);

			meshData.Indices.push_back(baseIndex + (i + 1)*ringVertexCount + j);
			meshData.Indices.push_back(baseIndex + i*ringVertexCount + j + 1);
			meshData.Indices.push_back(baseIndex + (i + 1)*ringVertexCount + j + 1);
		}
	}

	//
	// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
	// and connects the bottom pole to the bottom ring.
	//

	// South pole vertex was added last.
	UINT southPoleIndex = (UINT)meshData.Vertices.size() - 1;

	// Offset the indices to the index of the first vertex in the last ring.
	baseIndex = southPoleIndex - ringVertexCount;

	for (UINT i = 0; i < sliceCount; ++i)
	{
		meshData.Indices.push_back(southPoleIndex);
		meshData.Indices.push_back(baseIndex + i);
		meshData.Indices.push_back(baseIndex + i + 1);
	}


	int nTriNum = meshData.Indices.size() / 3;
	for (int i = 0; i < nTriNum; ++i)
	{
		int i0 = meshData.Indices[i * 3 + 0];
		int i1 = meshData.Indices[i * 3 + 1];
		int i2 = meshData.Indices[i * 3 + 2];

		Vertex v0 = meshData.Vertices[i0];
		Vertex v1 = meshData.Vertices[i1];
		Vertex v2 = meshData.Vertices[i2];

		//XMFLOAT3 e0 = v1.Position - v0.Position;

		Vec3 e0(v1.Position.x - v0.Position.x,
			v1.Position.y - v0.Position.y,
			v1.Position.z - v0.Position.z);

		XMVECTOR vVec1 = _VEC3(&e0);

		Vec3 e1(v2.Position.x - v0.Position.x,
			v2.Position.y - v0.Position.y,
			v2.Position.z - v0.Position.z);
		XMVECTOR vVec2 = _VEC3(&e1);


		//Vec3 vFaceNormal = Cross(e0, e1);
		Vec3 vFaceNormal;
		XMStoreFloat3(&vFaceNormal, XMVector3Cross(_VEC3(&e0), _VEC3(&e0)));


		meshData.Vertices[i0].Normal = XMFLOAT3(meshData.Vertices[i0].Normal.x + vFaceNormal.x,
			meshData.Vertices[i0].Normal.y + vFaceNormal.y,
			meshData.Vertices[i0].Normal.z + vFaceNormal.z);

		meshData.Vertices[i1].Normal = XMFLOAT3(meshData.Vertices[i1].Normal.x + vFaceNormal.x,
			meshData.Vertices[i1].Normal.y + vFaceNormal.y,
			meshData.Vertices[i1].Normal.z + vFaceNormal.z);

		meshData.Vertices[i2].Normal = XMFLOAT3(meshData.Vertices[i2].Normal.x + vFaceNormal.x,
			meshData.Vertices[i2].Normal.y + vFaceNormal.y,
			meshData.Vertices[i2].Normal.z + vFaceNormal.z);
	}


	CResBuffer* pBuffer = new CResBuffer();

	ID3DX11EffectTechnique* pTech = SINGLE(CRenderManager)->FindShader(SHADER_FXKEY_DEFAULT)->GetShader()->GetTechniqueByName("ColorTech");
	D3DX11_PASS_DESC passDesc;
	pTech->GetPassByIndex(0)->GetDesc(&passDesc);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	pBuffer->CreateLayout(layout, ARRAYSIZE(layout), passDesc.pIAInputSignature, passDesc.IAInputSignatureSize);
	pBuffer->CreateBuffer(meshData);

	return (CBuffer*)pBuffer;


}

void CResourcesManager::CreateQuad(float fWidth, float fHeight, MeshData<Vertex>& meshData)
{
	Vertex v[4];

	v[0] = Vertex(-fWidth / 2, -fHeight / 2, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
	v[1] = Vertex(fWidth / 2, -fHeight / 2, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
	v[2] = Vertex(fWidth / 2, fHeight / 2, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
	v[3] = Vertex(-fWidth / 2, fHeight / 2, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f);

	meshData.Vertices.assign(&v[0], &v[4]);

	UINT i[6];
	i[0] = 0;
	i[1] = 1;
	i[2] = 2;

	i[3] = 0;
	i[4] = 2;
	i[5] = 3;

	meshData.Indices.assign(&i[0], &i[6]);
}


XMFLOAT3 CResourcesManager::Cross(XMFLOAT3 u, XMFLOAT3 v)
{
	return XMFLOAT3((u.y*v.z) - (u.z*v.y),
		((u.z * v.x) - (u.x * v.z)),
		((u.x*v.y) - (u.y*v.x)));

	return NULL;
}


XMFLOAT3 CResourcesManager::Normalize(XMFLOAT3 v1)
{
	float x = v1.x * v1.x;
	float y = v1.y * v1.y;
	float z = v1.z * v1.z;

	float fLength = sqrt(x + y + z);

	return XMFLOAT3(v1.x / fLength, v1.y / fLength, v1.z / fLength);
}

CBuffer* CResourcesManager::CreateTerrainResource(int iVertexCountX, int iVertexCountZ, MeshData<VertexPNT>& meshData)
{
	int iMinX = -(iVertexCountX / 2);
	int iMaxX = (iVertexCountX / 2);

	int iMinZ = (iVertexCountZ / 2);
	int iMaxZ = -(iVertexCountZ / 2);

	for (int i = iMinX; i < iMaxX; ++i)
	{
		for (int j = iMinZ; j > iMaxZ; --j)
		{
			VertexPNT v0;
			v0.vPosition = XMFLOAT3(j, 0.f, i);
			v0.vTex.x = (i + (iVertexCountX / 2)) / (float)iVertexCountX;
			v0.vTex.y = (j - (iVertexCountZ / 2)) / (float)-iVertexCountZ;

			meshData.Vertices.push_back(v0);
		}
	}

	vector<vector<XMFLOAT3>*> vecNormal;
	vecNormal.reserve(meshData.Vertices.size());

	for (int i = 0; i < meshData.Vertices.size(); ++i)
	{
		vector<XMFLOAT3>* pVecNormal = new vector<XMFLOAT3>;
		vecNormal.push_back(pVecNormal);
	}
	

	for (int i = 0; i < iVertexCountZ -1; ++i)
	{
		for (int j = 0; j < iVertexCountX -1; ++j)
		{
			int idx0 = i * iVertexCountZ + j;
			int idx1 = i * iVertexCountZ + j + 1;
			int idx2 = (i + 1) * iVertexCountZ + j;
			int idx3 = (i + 1) * iVertexCountZ + j + 1;

			// 위쪽 삼각형
			XMVECTOR v1 = XMVectorSet(meshData.Vertices[idx0].vPosition.x - meshData.Vertices[idx1].vPosition.x,
										meshData.Vertices[idx0].vPosition.y - meshData.Vertices[idx1].vPosition.y,
										meshData.Vertices[idx0].vPosition.z - meshData.Vertices[idx1].vPosition.z,
										1);

			XMVECTOR v2 = XMVectorSet(meshData.Vertices[idx0].vPosition.x - meshData.Vertices[idx3].vPosition.x,
										meshData.Vertices[idx0].vPosition.y - meshData.Vertices[idx3].vPosition.y,
										meshData.Vertices[idx0].vPosition.z - meshData.Vertices[idx3].vPosition.z,
										1);

			XMVECTOR vNormal = XMVector3Cross(v1, v2);
			XMFLOAT3 fNormal;
			XMStoreFloat3(&fNormal, vNormal);

			vecNormal[idx0]->push_back(fNormal);
			vecNormal[idx1]->push_back(fNormal);
			vecNormal[idx3]->push_back(fNormal);

			meshData.Indices.push_back(idx0);
			meshData.Indices.push_back(idx1);
			meshData.Indices.push_back(idx3);


			// 아래쪽 삼각형
			v1 = XMVectorSet(meshData.Vertices[idx0].vPosition.x - meshData.Vertices[idx3].vPosition.x,
				meshData.Vertices[idx0].vPosition.y - meshData.Vertices[idx3].vPosition.y,
				meshData.Vertices[idx0].vPosition.z - meshData.Vertices[idx3].vPosition.z,
				1);

			v2 = XMVectorSet(meshData.Vertices[idx0].vPosition.x - meshData.Vertices[idx2].vPosition.x,
				meshData.Vertices[idx0].vPosition.y - meshData.Vertices[idx2].vPosition.y,
				meshData.Vertices[idx0].vPosition.z - meshData.Vertices[idx2].vPosition.z,
				1);

			vNormal = XMVector3Cross(v1, v2);
			XMStoreFloat3(&fNormal, vNormal);

			vecNormal[idx0]->push_back(fNormal);
			vecNormal[idx3]->push_back(fNormal);
			vecNormal[idx2]->push_back(fNormal);

			meshData.Indices.push_back(idx0);
			meshData.Indices.push_back(idx3);
			meshData.Indices.push_back(idx2);

		}
	}



	for (int i = 0; i < vecNormal.size(); ++i)
	{
		XMVECTOR vNormalAvr = XMVectorZero();

		for (int j = 0; j < vecNormal[i]->size(); ++j)
		{
			XMFLOAT3 fV = (*vecNormal[i])[j];
			XMVECTOR vNormal = XMLoadFloat3(&fV);

			vNormalAvr += vNormal;
		}

		vNormalAvr /= vecNormal[i]->size();

		XMFLOAT3 fFinalNormal;
		XMStoreFloat3(&fFinalNormal, vNormalAvr);

		meshData.Vertices[i].vNormal = fFinalNormal;

		delete vecNormal[i];
	}



	CResBuffer* pBuffer = new CResBuffer();

	ID3DX11EffectTechnique* pTech = NULL;
	
	if(SINGLE(CRenderManager)->GetRenderMode() == RM_FORWARD)
		pTech = SINGLE(CRenderManager)->FindShader(SHADER_FXKEY_TERRAIN)->GetShader()->GetTechniqueByName(SHADER_TECH_TERRAINFX_DEFAULT);
	else if (SINGLE(CRenderManager)->GetRenderMode() == RM_DEFERRED)
		pTech = SINGLE(CRenderManager)->FindShader(SHADER_FXKEY_TERRAIN)->GetShader()->GetTechniqueByName(SHADER_TECH_TERRAINFX_DEFERRED);

	D3DX11_PASS_DESC passDesc;
	pTech->GetPassByIndex(0)->GetDesc(&passDesc);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	pBuffer->CreateLayout(layout, ARRAYSIZE(layout), passDesc.pIAInputSignature, passDesc.IAInputSignatureSize);
	pBuffer->CreateBuffer(meshData);

	return (CBuffer*)pBuffer;
}


CBuffer* CResourcesManager::CreateObjFile(wstring strFileName)
{

	MeshData<Vertex> meshData;

	bool result;
	char filename[256];
	int vertexCount, textureCount, normalCount, faceCount;
	char garbage;

	strFileName = SINGLE(CFilePathManager)->FindPath(MESH_PATH_KEY) + strFileName;

	// Read in the name of the model file.
	//GetModelFilename(strFileName.c_str());

	// Read in the number of vertices, tex coords, normals, and faces so that the data structures can be initialized with the exact sizes needed.
	result = ReadFileCounts(strFileName.c_str(), vertexCount, textureCount, normalCount, faceCount);
	if (!result)
	{
		return NULL;
	}

	// Display the counts to the screen for information purposes.
	cout << endl;
	cout << "Vertices: " << vertexCount << endl;
	cout << "UVs:      " << textureCount << endl;
	cout << "Normals:  " << normalCount << endl;
	cout << "Faces:    " << faceCount << endl;

	// Now read the data from the file into the data structures and then output it in our model format.
	result = LoadDataStructures(strFileName.c_str(), vertexCount, textureCount, normalCount, faceCount, meshData);
	if (!result)
	{
		return NULL;
	}

	CResBuffer* pBuffer = new CResBuffer();

	ID3DX11EffectTechnique* pTech = SINGLE(CRenderManager)->FindShader(SHADER_FXKEY_DEFAULT)->GetShader()->GetTechniqueByName("ObjTech");
	D3DX11_PASS_DESC passDesc;
	pTech->GetPassByIndex(0)->GetDesc(&passDesc);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	pBuffer->CreateLayout(layout, ARRAYSIZE(layout), passDesc.pIAInputSignature, passDesc.IAInputSignatureSize);
	pBuffer->CreateBuffer(meshData);

	return (CBuffer*)pBuffer;
}

void CResourcesManager::GetModelFilename(const WCHAR* szFileName)
{
	bool done;
	ifstream fin;

	// Loop until we have a file name.
	done = false;
	while (!done)
	{
		// Ask the user for the filename.


		// Attempt to open the file.
		fin.open(szFileName);

		if (fin.good())
		{
			// If the file exists and there are no problems then exit since we have the file name.
			done = true;
		}
		else
		{
			// If the file does not exist or there was an issue opening it then notify the user and repeat the process.
			fin.clear();
			cout << endl;
			cout << "File " << szFileName << " could not be opened." << endl << endl;
		}
	}

	return;

}
bool CResourcesManager::ReadFileCounts(const WCHAR* szFileName, int& vertexCount, int& textureCount, int& normalCount, int& faceCount)
{
	ifstream fin;
	char input;


	// Initialize the counts.
	vertexCount = 0;
	textureCount = 0;
	normalCount = 0;
	faceCount = 0;

	// Open the file.
	fin.open(szFileName);

	// Check if it was successful in opening the file.
	if (fin.fail() == true)
	{
		return false;
	}

	// Read from the file and continue to read until the end of the file is reached.
	fin.get(input);
	while (!fin.eof())
	{
		// If the line starts with 'v' then count either the vertex, the texture coordinates, or the normal vector.
		if (input == 'v')
		{
			fin.get(input);
			if (input == ' ') { vertexCount++; }
			if (input == 't') { textureCount++; }
			if (input == 'n') { normalCount++; }
		}

		// If the line starts with 'f' then increment the face count.
		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ') { faceCount++; }
		}

		// Otherwise read in the remainder of the line.
		while (input != '\n')
		{
			fin.get(input);
		}

		// Start reading the beginning of the next line.
		fin.get(input);
	}

	// Close the file.
	fin.close();

	return true;
}
bool CResourcesManager::LoadDataStructures(const WCHAR* szFileName, int vertexCount, int textureCount, int normalCount, int faceCount, MeshData<Vertex>& meshData)
{
	VertexType *vertices, *texcoords, *normals;
	FaceType *faces;
	ifstream fin;
	int vertexIndex, texcoordIndex, normalIndex, faceIndex, vIndex, tIndex, nIndex;
	char input, input2;
	ofstream fout;

	int iTriIndex = 0;

	meshData.Indices.clear();
	meshData.Vertices.clear();


	// Initialize the four data structures.
	vertices = new VertexType[vertexCount];
	if (!vertices)
	{
		return false;
	}

	texcoords = new VertexType[textureCount];
	if (!texcoords)
	{
		return false;
	}

	normals = new VertexType[normalCount];
	if (!normals)
	{
		return false;
	}

	faces = new FaceType[faceCount];
	if (!faces)
	{
		return false;
	}

	// Initialize the indexes.
	vertexIndex = 0;
	texcoordIndex = 0;
	normalIndex = 0;
	faceIndex = 0;

	// Open the file.
	fin.open(szFileName);

	// Check if it was successful in opening the file.
	if (fin.fail() == true)
	{
		return false;
	}


	// Read in the vertices, texture coordinates, and normals into the data structures.
	// Important: Also convert to left hand coordinate system since Maya uses right hand coordinate system.
	fin.get(input);
	while (!fin.eof())
	{
		if (input == 'v')
		{
			fin.get(input);

			// Read in the vertices.
			if (input == ' ')
			{
				fin >> vertices[vertexIndex].x >> vertices[vertexIndex].y >> vertices[vertexIndex].z;

				// Invert the Z vertex to change to left hand system.
				vertices[vertexIndex].z = vertices[vertexIndex].z * -1.0f;
				vertexIndex++;
			}

			// Read in the texture uv coordinates.
			if (input == 't')
			{
				fin >> texcoords[texcoordIndex].x >> texcoords[texcoordIndex].y;

				// Invert the V texture coordinates to left hand system.
				texcoords[texcoordIndex].y = 1.0f - texcoords[texcoordIndex].y;
				texcoordIndex++;
			}

			// Read in the normals.
			if (input == 'n')
			{
				fin >> normals[normalIndex].x >> normals[normalIndex].y >> normals[normalIndex].z;

				// Invert the Z normal to change to left hand system.
				normals[normalIndex].z = normals[normalIndex].z * -1.0f;
				normalIndex++;
			}
		}

		// Read in the faces.
		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ')
			{
				// Read the face data in backwards to convert it to a left hand system from right hand system.
				fin >> faces[faceIndex].vIndex3 >> input2 >> faces[faceIndex].tIndex3 >> input2 >> faces[faceIndex].nIndex3
					>> faces[faceIndex].vIndex2 >> input2 >> faces[faceIndex].tIndex2 >> input2 >> faces[faceIndex].nIndex2
					>> faces[faceIndex].vIndex1 >> input2 >> faces[faceIndex].tIndex1 >> input2 >> faces[faceIndex].nIndex1;

				Vertex v1, v2, v3;
				ZeroMemory(&v1, sizeof(v1));
				ZeroMemory(&v2, sizeof(v2));
				ZeroMemory(&v3, sizeof(v3));


				// 1번 삼각형
				XMFLOAT3 vPosition = XMFLOAT3(vertices[faces[faceIndex].vIndex1 - 1].x,
					vertices[faces[faceIndex].vIndex1 - 1].y,
					vertices[faces[faceIndex].vIndex1 - 1].z);
				//인덱스가 1부터 시작이라 -1을 해주어야 한다.

				XMFLOAT3 vNormal = XMFLOAT3(normals[faces[faceIndex].nIndex1].x,
					normals[faces[faceIndex].nIndex1].y,
					normals[faces[faceIndex].nIndex1].z);


				float fU = texcoords[faces[faceIndex].tIndex1 - 1].x;
				float fV = texcoords[faces[faceIndex].tIndex1 - 1].y;

				v1.Position = vPosition;
				v1.Normal = vNormal;
				v1.TexC.x = fU;
				v1.TexC.y = fV;

				meshData.Vertices.push_back(v1);
				meshData.Indices.push_back(iTriIndex++);

				// 2번 삼각형
				vPosition = XMFLOAT3(vertices[faces[faceIndex].vIndex2 - 1].x,
					vertices[faces[faceIndex].vIndex2 - 1].y,
					vertices[faces[faceIndex].vIndex2 - 1].z);

				vNormal = XMFLOAT3(normals[faces[faceIndex].nIndex2 - 1].x,
					normals[faces[faceIndex].nIndex2 - 1].y,
					normals[faces[faceIndex].nIndex2 - 1].z);

				fU = texcoords[faces[faceIndex].tIndex2].x;
				fV = texcoords[faces[faceIndex].tIndex2].y;

				v2.Position = vPosition;
				v2.Normal = vNormal;
				v2.TexC.x = fU;
				v2.TexC.y = fV;

				meshData.Vertices.push_back(v2);
				meshData.Indices.push_back(iTriIndex++);

				//3번 삼각형
				vPosition = XMFLOAT3(vertices[faces[faceIndex].vIndex3 - 1].x,
					vertices[faces[faceIndex].vIndex3 - 1].y,
					vertices[faces[faceIndex].vIndex3 - 1].z);

				vNormal = XMFLOAT3(normals[faces[faceIndex].nIndex3 - 1].x,
					normals[faces[faceIndex].nIndex3 - 1].y,
					normals[faces[faceIndex].nIndex3 - 1].z);

				fU = texcoords[faces[faceIndex].tIndex3 - 1].x;
				fV = texcoords[faces[faceIndex].tIndex3 - 1].y;

				v3.Position = vPosition;
				v3.Normal = vNormal;
				v3.TexC.x = fU;
				v3.TexC.y = fV;

				meshData.Vertices.push_back(v3);
				meshData.Indices.push_back(iTriIndex++);

				faceIndex++;

			}
		}

		// Read in the remainder of the line.
		while (input != '\n')
		{
			fin.get(input);
		}

		// Start reading the beginning of the next line.
		fin.get(input);
	}
	fin.close();



	// Release the four data structures.
	if (vertices)
	{
		delete[] vertices;
		vertices = 0;
	}
	if (texcoords)
	{
		delete[] texcoords;
		texcoords = 0;
	}
	if (normals)
	{
		delete[] normals;
		normals = 0;
	}
	if (faces)
	{
		delete[] faces;
		faces = 0;
	}

	return true;
}


CBuffer* CResourcesManager::CreateBufferFromFBX(const char* szFileName, list<CMaterial*>* pOutMatList, CAnimation3D** pAnimation)
{
	char szFilePath[MAX_PATH] = { 0, };
	char szFileFullPath[MAX_PATH] = { 0, };
	WCHAR szFilePathW[MAX_PATH] = { 0, };
	wstring strFilePath = SINGLE(CFilePathManager)->FindPath(MESH_PATH_KEY);

	WideCharToMultiByte(CP_ACP, 0, strFilePath.c_str(), MAX_PATH, szFilePath, MAX_PATH, NULL, NULL);
	strcpy(szFileFullPath, szFilePath);
	strcat(szFileFullPath, szFileName);

	strcpy(szFilePath, szFileFullPath);

	for (int i = strlen(szFilePath); i > 0; --i)
	{
		if (szFilePath[i] == '\\')
		{
			memset(&szFilePath[i+1], '\0', strlen(szFilePath) - i);
			break;
		}
	}

	//MeshData meshData;


	//Assimp::Importer importer;
	//aiProcess_ConvertToLeftHanded로 좌표계 변환
	const aiScene* pScene = m_AssimpImporter.ReadFile(szFileFullPath, 
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	if (!pScene)
	{
		const char* szError = m_AssimpImporter.GetErrorString();
		int a = 3;
		return NULL;
	}

	const aiNode* pRootNode = pScene->mRootNode;
	aiMatrix4x4 matGlobalInverse = pScene->mRootNode->mTransformation;


	CHierarchicalBuffer* pHieBuffer = CHierarchicalBuffer::Create("");
	pHieBuffer->Init();
	
	ID3DX11EffectTechnique* pTech = NULL;
	

	vector<D3D11_INPUT_ELEMENT_DESC> layout;

	if (pScene->mNumAnimations > 0)
	{
		(*pAnimation) = CAnimation3D::Create("");
		(*pAnimation)->SetAssimpScene(pScene);

		(*pAnimation)->InitEntries(pScene);
		(*pAnimation)->SetGlobalInverseMatrix(matGlobalInverse.Inverse());

		if(SINGLE(CRenderManager)->GetRenderMode() == RM_FORWARD)
			pTech = SINGLE(CRenderManager)->FindShader(SHADER_FXKEY_DEFAULT)->GetShader()->GetTechniqueByName(SHADER_TECH_DEFULATFX_DEFAULTFBX);
		else if(SINGLE(CRenderManager)->GetRenderMode() == RM_DEFERRED)
			pTech = SINGLE(CRenderManager)->FindShader(SHADER_FXKEY_DEFERRED)->GetShader()->GetTechniqueByName(SHADER_TECH_DEFERREDFX_GBUFFERSKINNING);

		layout.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
		layout.push_back({ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 });
		layout.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 });
		layout.push_back({ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 });
		layout.push_back({ "BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	
	}
	else
	{
		if (SINGLE(CRenderManager)->GetRenderMode() == RM_FORWARD)
			pTech = SINGLE(CRenderManager)->FindShader(SHADER_FXKEY_DEFAULT)->GetShader()->GetTechniqueByName(SHADER_TECH_DEFAULTFX_DEFAULT);
		else if(SINGLE(CRenderManager)->GetRenderMode() == RM_DEFERRED)
			pTech = SINGLE(CRenderManager)->FindShader(SHADER_FXKEY_DEFERRED)->GetShader()->GetTechniqueByName(SHADER_TECH_DEFERREDFX_GBUFFERDEFAULT);

		layout.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
		layout.push_back({ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 });
		layout.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 });
		layout.push_back({ "Color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 });

	}
	D3DX11_PASS_DESC passDesc;
	pTech->GetPassByIndex(0)->GetDesc(&passDesc);



	int iMeshCount = pScene->mNumMeshes;
	for (int i = 0; i < pScene->mNumMeshes; ++i)
	{
		aiMesh* pMesh = pScene->mMeshes[i];
		MeshData<Vertex> meshData;

		CResBuffer* pBuffer = CResBuffer::Create(pMesh->mName.data);

		for (int j = 0; j < pMesh->mNumVertices; ++j)
		{
			Vertex v;
			v.Position.x = pMesh->mVertices[j].x;
			v.Position.y = pMesh->mVertices[j].y;
			v.Position.z = pMesh->mVertices[j].z;

			v.Normal.x = pMesh->mNormals[j].x;
			v.Normal.y = pMesh->mNormals[j].y;
			v.Normal.z = pMesh->mNormals[j].z;

			v.TexC.x = pMesh->mTextureCoords[0][j].x;
			v.TexC.y = pMesh->mTextureCoords[0][j].y;


			meshData.Vertices.push_back(v);
		}

		for (int j = 0; j < pMesh->mNumFaces; ++j)
		{
			aiFace pFace = pMesh->mFaces[j];

			for (int j = 0; j < pFace.mNumIndices; ++j)
			{
				meshData.Indices.push_back(pFace.mIndices[j]);
			}

		}



		if ((*pAnimation) && pMesh->mNumBones > 0)
		{

			(*pAnimation)->LoadBones(i, pMesh, meshData.Vertices);

			/*for (int i = 0; i < pMesh->mNumBones; ++i)
			{
			Bone tBone;
			int iBoneIdx = 0;

			tBone.strName = pMesh->mBones[i]->mName.C_Str();
			tBone.matOffset = pMesh->mBones[i]->mOffsetMatrix;

			for (int j = 0; j < pMesh->mBones[i]->mNumWeights; ++j)
			{
			meshData.Vertices[pMesh->mBones[i]->mWeights[j].mVertexId].AddBlendWeight(pMesh->mBones[i]->mWeights[j].mWeight);
			meshData.Vertices[pMesh->mBones[i]->mWeights[j].mVertexId].AddBoneIndex(i);
			}
			boneArr.push_back(tBone);
			}*/


			//for (int i = 0; i < pScene->mNumAnimations; ++i)
			//{
			//	aiAnimation* anim = new aiAnimation;
			//	memcpy(&anim, &pScene->mAnimations[i],sizeof(aiAnimation));
			//	(*pAnimation)->AddAnimation(anim);
			//}


		}
		pBuffer->CreateLayout(&layout[0], layout.size(), passDesc.pIAInputSignature, passDesc.IAInputSignatureSize);
		pBuffer->CreateBuffer(meshData);

		string strFullName = "";

		strFullName = pMesh->mName.data + i;

		pHieBuffer->AddResource(strFullName, (CBaseResource*)pBuffer);

	}

	if ((*pAnimation))
		(*pAnimation)->CreateTextureForVTF();

	//Material

	// 애니메이션이 존재할 경우 Skinning Shader
	CShader* pShader = NULL;
	char szTech[64] = { 0 ,};
	int iPass = 0;
	
	if (SINGLE(CRenderManager)->GetRenderMode() == RM_FORWARD)
	{
		if ((*pAnimation))
		{
			pShader = SINGLE(CRenderManager)->FindShader(SHADER_FXKEY_DEFAULT);
			strcpy(szTech, SHADER_TECH_DEFULATFX_DEFAULTFBX);
			iPass = SHADER_PASS_DEFAULTFX_DEFAULTFBXTECH::SPDDF_DEFAULT;
			(*pAnimation)->SetShader(pShader);
		}
		else
		{
			pShader = SINGLE(CRenderManager)->FindShader(SHADER_FXKEY_DEFAULT);
			strcpy(szTech, SHADER_TECH_DEFAULTFX_DEFAULT);
			iPass = SHADER_PASS_DEFAULTFX_DEFAULTTECH::SPDD_DEFAULT;
		}
	}
	else if (SINGLE(CRenderManager)->GetRenderMode() == RM_DEFERRED)
	{
		if ((*pAnimation))
		{
			pShader = SINGLE(CRenderManager)->FindShader(SHADER_FXKEY_DEFERRED);
			strcpy(szTech,SHADER_TECH_DEFERREDFX_GBUFFERSKINNING);
			iPass = SHADER_PASS_DEFERREDFX_GBUFFER_SKINNINGTECH::SPDGS_DEFAULT;
			(*pAnimation)->SetShader(pShader);
		}
		else
		{
			pShader = SINGLE(CRenderManager)->FindShader(SHADER_FXKEY_DEFERRED);
			strcpy(szTech, SHADER_TECH_DEFERREDFX_GBUFFERDEFAULT);
			iPass = SHADER_PASS_DEFERREDFX_GBUFFER_DEFAULTTECH::SPDGD_DEFAULT;
		}
	}

	for (int i = 0; i < pScene->mNumMaterials; ++i)
	{
		const aiMaterial* pAiMaterial = pScene->mMaterials[i];
		CMaterial* pMaterial = CMaterial::Create("");

		pMaterial->SetShader(pShader);
		pMaterial->SetTech(szTech);
		pMaterial->AddPass(iPass);

		// Diffuse Texture
		if(pAiMaterial->GetTextureCount(aiTextureType::aiTextureType_DIFFUSE) > 0)
		{
			aiString strPath;
			if (pAiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &strPath, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				char szPath[MAX_PATH] = { 0, };
				strcpy(szPath, szFilePath);
				strcat(szPath, strPath.data);
				

				CTexture* pTexture = CTexture::Create("DiffuseTexture");
				pTexture->CreateResourceFullPath(szPath);

				pMaterial->AddTexture(SHADER_HANDLE_SHARE_TEXTURE_DIFFUSE, pTexture);
			}
		}

		// Normal Texture
		if (pAiMaterial->GetTextureCount(aiTextureType::aiTextureType_NORMALS) > 0)
		{
			aiString strPath;
			if (pAiMaterial->GetTexture(aiTextureType_NORMALS, 0, &strPath, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				char szPath[MAX_PATH] = { 0, };
				strcpy(szPath, szFilePath);
				strcat(szPath, strPath.data);


				CTexture* pTexture = CTexture::Create("NormalTexture");
				pTexture->CreateResourceFullPath(szPath);

				pMaterial->AddTexture(SHADER_HANDLE_SHARE_TEXTURE_NORMAL, pTexture);
			}
		}


		// Specular Texture
		if (pAiMaterial->GetTextureCount(aiTextureType::aiTextureType_SPECULAR) > 0)
		{
			aiString strPath;
			if (pAiMaterial->GetTexture(aiTextureType_SPECULAR, 0, &strPath, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				char szPath[MAX_PATH] = { 0, };
				strcpy(szPath, szFilePath);
				strcat(szPath, strPath.data);


				CTexture* pTexture = CTexture::Create("SpecularTexture");
				pTexture->CreateResourceFullPath(szPath);

				pMaterial->AddTexture(SHADER_HANDLE_SAHRE_TEXTURE_SPECULAR, pTexture);
			}
		}

		pOutMatList->push_back(pMaterial);
	}
	


	return (CBuffer*)pHieBuffer;

	


	/*CResBuffer* pBuffer = new CResBuffer();

	ID3DX11EffectTechnique* pTech = SINGLE(CRenderManager)->FindShader(SHADER_FXKEY_DEFAULT)->GetShader()->GetTechniqueByName(SHADER_TECH_DEFULATFX_DEFAULTFBX);
	D3DX11_PASS_DESC passDesc;
	pTech->GetPassByIndex(0)->GetDesc(&passDesc);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	pBuffer->CreateLayout(layout, ARRAYSIZE(layout), passDesc.pIAInputSignature, passDesc.IAInputSignatureSize);
	pBuffer->CreateBuffer(meshData);

	return (CBuffer*)pBuffer;*/

	////// Fbx Sdk //////

	//bool bSuccess = m_pFbxImporter->Initialize(szFilePath, -1, m_pFbxManager->GetIOSettings());
	//if (!bSuccess)
	//{
	//	MessageBox(NULL, L"Fail Loading FBX", L"Message", MB_OK);
	//	return NULL;
	//}
	//FbxScene* pScene = FbxScene::Create(m_pFbxManager, "");

	//bSuccess = m_pFbxImporter->Import(pScene);
	//if (!bSuccess)
	//{
	//	MessageBox(NULL, L"Fail Loading FBX", L"Message", MB_OK);
	//	return NULL;
	//}

	//FbxNode* pFbxRootNode = pScene->GetRootNode();
	//FbxMesh* pMesh = pFbxRootNode->GetChild(0)->GetMesh();
	//
	//vector<Bone> boneArr;

	//ProcessSkeletonHierarchy(pFbxRootNode, &boneArr);

	//BoneNode* tRootBone = CreateBoneHierarchy(pFbxRootNode,NULL);
	//// 주의

	////int iVertexCounter = 0;
	//if (pFbxRootNode)
	//{

	//	CHierarchicalBuffer* pHieBuffer = CHierarchicalBuffer::Create("");
	//	pHieBuffer->Init();
	//	//
	//	ID3DX11EffectTechnique* pTech = SINGLE(CRenderManager)->FindShader(SHADER_FXKEY_DEFAULT)->GetShader()->GetTechniqueByName(SHADER_TECH_DEFULATFX_DEFAULTFBX);
	//	D3DX11_PASS_DESC passDesc;
	//	pTech->GetPassByIndex(0)->GetDesc(&passDesc);


	//	D3D11_INPUT_ELEMENT_DESC layout[] =
	//	{
	//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	};
	//	
	//	(*pAnimation) = CAnimation3D::Create("");
	//	vector<KeyFrame>* keyFrameArr = (*pAnimation)->GetKeyFrameList();
	//	(*pAnimation)->SetBoneHierarchyRootNode(tRootBone);


	//	for (int i = 0; i < pFbxRootNode->GetChildCount(); ++i)
	//	{
	//		int a = pFbxRootNode->GetChildCount();

	//		FbxNode* pFbxChildNode = pFbxRootNode->GetChild(i);
	//		
	//		if (pFbxChildNode->GetNodeAttribute() == NULL)
	//			continue;

	//		FbxNodeAttribute::EType attributeType = pFbxChildNode->GetNodeAttribute()->GetAttributeType();

	//		if (attributeType != FbxNodeAttribute::eMesh)
	//			continue;

	//		FbxMeshData tMeshdata;
	//		int iVertexCounter = 0;

	//		FbxMesh* pMesh = (FbxMesh*)pFbxChildNode->GetNodeAttribute();
	//		tMeshdata.strName = pFbxChildNode->GetName();

	//		MeshData meshData;
	//		GetFbxMeshInfo(pFbxChildNode,pMesh, meshData, pScene,boneArr);
	//		
	//		// Animation

	//		

	//		//

	//		CResBuffer* pChildBuffer = CResBuffer::Create("");
	//		
	//		pChildBuffer->CreateLayout(layout, ARRAYSIZE(layout), passDesc.pIAInputSignature, passDesc.IAInputSignatureSize);
	//		pChildBuffer->CreateBuffer(meshData);
	//		pHieBuffer->AddResource(pFbxChildNode->GetName(),pChildBuffer);

	//		//(*pAnimation)->AddResource((CBuffer*)pChildBuffer);

	//		(*pAnimation)->AddBonePair(pChildBuffer, meshData.Bones);

	//	}

	//	AssociateMaterialToMesh(pFbxRootNode->GetChild(0));

	//	//Material

	//	CShader* pShader = SINGLE(CRenderManager)->FindShader(SHADER_FXKEY_DEFAULT);


	//	int iMaterialCount = pFbxRootNode->GetChild(0)->GetMaterialCount();

	//	if (iMaterialCount == 0)
	//	{
	//		CMaterial* pMaterial = CMaterial::Create();
	//		pMaterial->SetShader(pShader);
	//		pMaterial->SetTech(SHADER_TECH_DEFULATFX_DEFAULTFBX);
	//		pMaterial->AddPass(SHADER_PASS_DEFAULTFX_DEFAULTFBXTECH::SPDDF_DEFAULT);
	//		pOutMatList->push_back(pMaterial);
	//	}
	//	else
	//	{
	//		for (int i = 0; i < iMaterialCount; ++i)
	//		{
	//			CMaterial* pMaterial = CMaterial::Create();
	//			pMaterial->SetShader(pShader);
	//			pMaterial->SetTech(SHADER_TECH_DEFULATFX_DEFAULTFBX);
	//			pMaterial->AddPass(SHADER_PASS_DEFAULTFX_DEFAULTFBXTECH::SPDDF_DEFAULT);

	//			FbxSurfaceMaterial* pFbxMaterial = pFbxRootNode->GetChild(0)->GetMaterial(i);
	//			ProcessMaterialAttribute(pFbxMaterial, pMaterial);
	//			ProcessMaterialTexture(pFbxMaterial, pMaterial);
	//			pOutMatList->push_back(pMaterial);
	//		}
	//	}
	//	

	//	pScene->Destroy();
	//	(*pAnimation)->CreateTextureForVTF();
	//	(*pAnimation)->ConnectBoneNode(tRootBone);
	//	return (CBuffer*)pHieBuffer;
	//}
	//else
	//	pScene->Destroy();



	/*CHierarchicalBuffer* pHieBuffer = CHierarchicalBuffer::Create("");
	pHieBuffer->Init();

	ID3DX11EffectTechnique* pTech = SINGLE(CRenderManager)->FindShader(SHADER_FXKEY_DEFAULT)->GetShader()->GetTechniqueByName(SHADER_TECH_DEFULATFX_DEFAULTFBX);
	D3DX11_PASS_DESC passDesc;
	pTech->GetPassByIndex(0)->GetDesc(&passDesc);


	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};*/


	/*for (int i = 0; i < meshDataArr.size(); ++i)
	{
	CResBuffer* pBuffer = CResBuffer::Create();
	pBuffer->Init();

	for (list<Triangle>::iterator iter = meshDataArr[i].triList.begin(); iter != meshDataArr[i].triList.end(); ++iter)
	{

	if (iter->iCurIndexSize == 3)
	{
	for (int j = 0; j< 3; ++j)
	meshDataArr[i].meshData.Indices.push_back(iter->mIndices[j]);

	}
	else if (iter->iCurIndexSize == 4)
	{
	meshDataArr[i].meshData.Indices.push_back(iter->mIndices[0]);
	meshDataArr[i].meshData.Indices.push_back(iter->mIndices[1]);
	meshDataArr[i].meshData.Indices.push_back(iter->mIndices[2]);

	meshDataArr[i].meshData.Indices.push_back(iter->mIndices[0]);
	meshDataArr[i].meshData.Indices.push_back(iter->mIndices[2]);
	meshDataArr[i].meshData.Indices.push_back(iter->mIndices[3]);
	}

	}

	pBuffer->CreateLayout(layout, ARRAYSIZE(layout), passDesc.pIAInputSignature, passDesc.IAInputSignatureSize);
	pBuffer->CreateBuffer(meshDataArr[i].meshData);

	pHieBuffer->AddResource(meshDataArr[i].strName,pBuffer);
	}

	return (CBuffer*)pHieBuffer;


	/*

	for (int i = 0; i < m_vecVertices.size(); ++i)
	{
	Vertex v;
	v.Position = m_vecVertices[i].mPosition;
	v.Normal = m_vecVertices[i].mNormal;

	meshData.Vertices.push_back(v);
	}

	int iSize = m_vecTriangle.size();
	for (int i = 0; i < m_vecTriangle.size(); ++i)
	{
	meshData.Indices.push_back(m_vecTriangle[i].mIndices[0]);
	meshData.Indices.push_back(m_vecTriangle[i].mIndices[1]);
	meshData.Indices.push_back(m_vecTriangle[i].mIndices[2]);

	}
	int a = m_Skeleton.mJoints.size();
	*/

	/*CResBuffer* pBuffer = new CResBuffer();

	ID3DX11EffectTechnique* pTech = SINGLE(CRenderManager)->FindShader(SHADER_FXKEY_DEFAULT)->GetShader()->GetTechniqueByName(SHADER_TECH_DEFULATFX_DEFAULTFBX);
	D3DX11_PASS_DESC passDesc;
	pTech->GetPassByIndex(0)->GetDesc(&passDesc);


	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	pBuffer->CreateLayout(layout, ARRAYSIZE(layout), passDesc.pIAInputSignature, passDesc.IAInputSignatureSize);
	pBuffer->CreateBuffer(meshData);

	return (CBuffer*)pBuffer;


	pOutBuffer = (CBuffer*)pBuffer; */


}


PBoneNode CResourcesManager::CreateBoneHierarchy(const aiNode* pNode, PBoneNode pParentNode)
{
	PBoneNode tBone = new BoneNode;
	tBone->strName = pNode->mName.C_Str();
	
	for (int i = 0; i < pNode->mNumChildren; ++i)
	{
		PBoneNode tChildNode = CreateBoneHierarchy(pNode->mChildren[i], tBone);
		tBone->childNodeList.push_back(tChildNode);
	}


	return tBone;
}

CGameObject* CResourcesManager::CreateTerrainObject(int iVertexCountX, int iVertexCountZ)
{
	CGameObject* pObj = CGameObject::Create("Terrain");

	CTerrain* pTerrainComponent = CTerrain::Create("");
	
	pTerrainComponent->CreateTerrain(iVertexCountX, iVertexCountZ);


	CRenderer* pRenderer = CRenderer::Create("");
	pRenderer->SetResource(pTerrainComponent->GetTerrainBuffer());

	CShader* pShader = SINGLE(CRenderManager)->FindShader(SHADER_FXKEY_TERRAIN);

	char szTech[64] = { 0, };
	int  iPass = 0;

	if (SINGLE(CRenderManager)->GetRenderMode() == RM_FORWARD)
	{
		strcpy(szTech, SHADER_TECH_TERRAINFX_DEFAULT);
		iPass = 0;
	}
	else if (SINGLE(CRenderManager)->GetRenderMode() == RM_DEFERRED)
	{
		strcpy(szTech, SHADER_TECH_TERRAINFX_DEFERRED);
		iPass = 0;
	}


	CMaterial* pMaterial = CMaterial::Create("");
	pMaterial->SetShader(pShader);
	pMaterial->SetTech(szTech);
	pMaterial->AddPass(iPass);

	pRenderer->AddMaterial(pMaterial);

	pObj->AddComponent(pRenderer);
	pObj->AddComponent(pTerrainComponent);

	SAFE_RELEASE(pRenderer);
	SAFE_RELEASE(pTerrainComponent);

	return pObj;
}
