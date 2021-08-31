#pragma once

#include "../Core/Ref.h"
#include "../Core/CustomMath.h"

BIGGEST_BEGIN

typedef struct _tagVertexType
{
	float x, y, z;
}VertexType, *PVertexType;

typedef struct _tagFaceType
{
	int vIndex1, vIndex2, vIndex3;
	int tIndex1, tIndex2, tIndex3;
	int nIndex1, nIndex2, nIndex3;
}FaceType, *PFaceType;

////////////////////////////////

typedef struct Triangle
{
	int mIndices[4];
	int iCurIndexSize;

	std::string mMaterialName;
	unsigned int mMaterialIndex;

	void AddIndex(int idx)
	{
		if (iCurIndexSize >= 4)
		{
			return;
		}
		mIndices[iCurIndexSize++] = idx;
	}

	bool operator<(const Triangle& rhs)
	{
		return mMaterialIndex < rhs.mMaterialIndex;
	}

	Triangle() : iCurIndexSize(0){}
};


typedef struct _tagSkeleton
{
	vector<Bone> boneList;
};




////////////////////////////////
class BIGGEST_DLL CResourcesManager
	: public CRef
{
private:

	list<XMFLOAT2>	m_uvList;

	Assimp::Importer m_AssimpImporter;

	/*Skeleton		m_Skeleton;

	map<int, CtrlPoint*> m_mapControlPoints;
	string			m_strAnimationName;
	FbxLongLong		m_lAnimationLength;

	vector<Triangle> m_vecTriangle;
	vector<PNTIWVertex> m_vecVertices;
	bool			m_bHasAnimation;

	int				m_iTriangleCount;
	*/

public:
	bool Init();
	bool InitFbxManager();

	void CreateCylinder(float bottomRadius, float topRadius, float height, UINT sliceCount,
		UINT stackCount, MeshData<Vertex>& meshData);
	void CreateBox(float width, float height, float depth, MeshData<Vertex>& meshData);
	class CBuffer* CreateSphere(float radius, UINT sliceCount, UINT stackCount);
	void CreateQuad(float fWidth, float fHeight, MeshData<Vertex>& meshData);
	CBuffer* CreateTerrainResource(int iVertexCountX, int iVertexCountZ, MeshData<VertexPNT>& meshData);

	CBuffer* CreateObjFile(wstring strFileName);

	class CGameObject* CreateTerrainObject(int iVertexCountX, int iVertexCountZ);

private:
	void BuildCylinderTopCap(float bottomRadius, float topRadius, float height,
		UINT sliceCount, UINT stackCount, MeshData<Vertex>& meshData);
	void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height,
		UINT sliceCount, UINT stackCount, MeshData<Vertex>& meshData);

	void GetModelFilename(const WCHAR* szFileName);
	bool ReadFileCounts(const WCHAR* szFileName, int& vertexCount, int& textureCount, int& normalCount, int& faceCount);
	bool LoadDataStructures(const WCHAR* szFileName, int vertexCount, int textureCount, int normalCount, int faceCount, MeshData<Vertex>& meshData);


	// Fbx Field
public:
	CBuffer* CreateBufferFromFBX(const char* szFileName, list<class CMaterial*>* pOutMatList, class CAnimation3D** pAnimation);
private:

	PBoneNode CreateBoneHierarchy(const aiNode* pNode, PBoneNode pParentNode);

	//int FindJointIndexUsingName(const string& strName);



public:
	XMFLOAT3 Cross(XMFLOAT3 u, XMFLOAT3 v);
	XMFLOAT3 Normalize(XMFLOAT3 v1);


	DECLARE_SINGLE(CResourcesManager)
};

BIGGEST_END