#pragma once


#include "BiggestMacro.h"
#include "BiggestFlag.h"
#include "BiggestKey.h"
#include "BiggestMemory.h"

typedef struct _tagResolution
{
	float		fWidth;
	float		fHeight;

	_tagResolution(float w, float h)
	{
		fWidth = w;
		fHeight = h;
	}

	_tagResolution()
	{
		fWidth = fHeight = 0;
	}

	_tagResolution(const _tagResolution& rs)
	{
		*this = rs;
	}
}RESOLUTION, *PRESOLUTION;


enum RESOLUTION_TYPE
{
	RT_NORMAL,
	RT_HD,
	RT_FHD,
	RT_MAX
};


static const RESOLUTION	g_tResolution[RT_MAX] =
{
	RESOLUTION(800.f, 600.f),
	RESOLUTION(1280.f, 720.f),
	RESOLUTION(1920.f, 1080.f)
};


// InputInfo
typedef struct _tagInputInfo
{
	bool	bPress;
	bool	bDown;
	bool	bUp;
}INPUTINFO, *PINPUTINFO;


// Mouse Mode
enum MOUSE_MODE
{
	MM_FREE,
	MM_CENTER
};



struct _tagCollInfo
{

};


struct Vertex
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 TexC;
	float BlendWeights[4];
	int BoneIndices[4];

	XMFLOAT3 TangentU;

	int iBlendCount;
	int iBoneCount;

	Vertex() : iBlendCount(0), iBoneCount(0)
	{
		memset(BlendWeights, 0, sizeof(float) * 4);
		memset(BoneIndices, 0, sizeof(int) * 4);
	}
	Vertex(const XMFLOAT3& p, const XMFLOAT3& n, const XMFLOAT3& t, const XMFLOAT2& uv)
		: Position(p), Normal(n), TangentU(t), TexC(uv), iBlendCount(0), iBoneCount(0)
	{
		memset(BlendWeights, 0, sizeof(float) * 4);
		memset(BoneIndices, 0, sizeof(int) * 4);
	}
	Vertex(
		float px, float py, float pz,
		float nx, float ny, float nz,
		float tx, float ty, float tz,
		float u, float v)
		: Position(px, py, pz), Normal(nx, ny, nz),
		TangentU(tx, ty, tz), TexC(u, v), iBlendCount(0), iBoneCount(0)
	{
		memset(BlendWeights, 0, sizeof(float) * 4);
		memset(BoneIndices, 0, sizeof(int) * 4);
	}

	void AddBlendWeight(float fBlendWeight)
	{
		if (iBlendCount >= 4)
			return;
		BlendWeights[iBlendCount++] = fBlendWeight;
	}

	void AddBoneIndex(int iBoneIndex)
	{
		if (iBoneCount >= 4)
			return;
		BoneIndices[iBoneCount++] = iBoneIndex;
	}


};

typedef struct _tagVertexP
{
	XMFLOAT3 vPosition;
}VertexP,*PVertexP;

typedef struct _tagVertexPT
{
	XMFLOAT3 vPosition;
	XMFLOAT2 vTex;
}VertexPT, *PVertexPT;


typedef struct _tagVertexUI
{
	XMFLOAT3 vPosition;
	XMFLOAT2 vTex;
}VertexUI, *PVertexUI;

typedef struct _tagSimpleVertex
{
	Vec3 vPosition;
	XMFLOAT4 vColor;
}SimpleVertex, *PSimpleVertex;

typedef struct _tagVertexPNT
{	
	XMFLOAT3 vPosition;
	XMFLOAT3 vNormal;
	XMFLOAT2 vTex;
}VertexPNT,*PVertexPNT;

typedef struct _tagKeyFrame
{
	int iFrameNum;
	_tagKeyFrame* pNext;

	vector<XMFLOAT3> keyT;
	vector<XMFLOAT3> keyR;
	vector<XMFLOAT3> keyS;

	_tagKeyFrame() : pNext(NULL)
	{}

}KeyFrame, *PKeyFrame;

typedef struct _tagBone
{
	string	strName;
	int		iParentIdx;

	vector<KeyFrame>	keyFrames;

	KeyFrame* pAnimation;


	aiMatrix4x4 matOffset;
	int iBoneIdx;


	_tagBone() : pAnimation(NULL), iBoneIdx(0)
	{

		iParentIdx = -1;
	}

}Bone, *PBone;

typedef struct _tagBoneNode
{
	string strName;
	Bone* pBoneInfo;
	_tagBoneNode* pParentNode;
	vector<_tagBoneNode*> childNodeList;
}BoneNode, *PBoneNode;

template <typename T>
struct MeshData
{
	vector<T> Vertices;
	vector<UINT> Indices;
};


// AABB
typedef struct _tagAABB :
public _tagCollInfo
{
	Vec3	vMin;
	Vec3	vMax;

	_tagAABB() :
		vMin(Vec3(0.f, 0.f, 0.f)),
		vMax(Vec3(0.f, 0.f, 0.f))
	{
	}

	_tagAABB(const Vec3& _vMin, const Vec3& _vMax) :
		vMin(_vMin),
		vMax(_vMax)
	{
	}
}AABB, *PAABB;


// Ray
typedef struct _tagRay :
public _tagCollInfo
{
	Vec3	vPos;
	Vec3	vDir;

	_tagRay() :
		vPos(Vec3(0.f, 0.f, 0.f)),
		vDir(Vec3(0.f, 0.f, 0.f))
	{
	}

	_tagRay(const Vec3& vP, const Vec3& vD) :
		vPos(vP),
		vDir(vD)
	{
	}
}RAY, *PRAY;


