#pragma once
#include "../Core/Ref.h"
#include "../Core/CustomMath.h"

BIGGEST_BEGIN

//

class Material
{
public:
	std::string mName;
	XMFLOAT3 mAmbient;
	XMFLOAT3 mDiffuse;
	XMFLOAT3 mEmissive;
	double mTransparencyFactor;
	std::string mDiffuseMapName;
	std::string mEmissiveMapName;
	std::string mGlossMapName;
	std::string mNormalMapName;
	std::string mSpecularMapName;

	virtual void WriteToStream(std::ostream& inStream) = 0;
};

class LambertMaterial : public Material
{
public:

	void WriteToStream(std::ostream& inStream)
	{
		inStream << "Ambient: " << mAmbient.x << " " << mAmbient.y << " " << mAmbient.z << std::endl;
		inStream << "Diffuse: " << mDiffuse.x << " " << mDiffuse.y << " " << mDiffuse.z << std::endl;
		inStream << "Emissive: " << mEmissive.x << " " << mEmissive.y << " " << mEmissive.z << std::endl;

		if (!mDiffuseMapName.empty())
		{
			inStream << "DiffuseMap: " << mDiffuseMapName << std::endl;
		}

		if (!mNormalMapName.empty())
		{
			inStream << "NormalMap: " << mNormalMapName << std::endl;
		}
	}
};

class PhongMaterial : public Material
{
public:
	XMFLOAT3 mSpecular;
	XMFLOAT3 mReflection;
	double mSpecularPower;
	double mShininess;
	double mReflectionFactor;

	void WriteToStream(std::ostream& inStream)
	{
		inStream << "Ambient: " << mAmbient.x << " " << mAmbient.y << " " << mAmbient.z << std::endl;
		inStream << "Diffuse: " << mDiffuse.x << " " << mDiffuse.y << " " << mDiffuse.z << std::endl;
		inStream << "Emissive: " << mEmissive.x << " " << mEmissive.y << " " << mEmissive.z << std::endl;
		inStream << "Specular: " << mSpecular.x << " " << mSpecular.y << " " << mSpecular.z << std::endl;
		inStream << "SpecPower: " << mSpecularPower << std::endl;
		inStream << "Reflectivity: " << mReflection.x << " " << mReflection.y << " " << mReflection.z << std::endl;

		if (!mDiffuseMapName.empty())
		{
			inStream << "DiffuseMap: " << mDiffuseMapName << std::endl;
		}

		if (!mNormalMapName.empty())
		{
			inStream << "NormalMap: " << mNormalMapName << std::endl;
		}
	}
};

struct BlendingIndexWeightPair
{
	unsigned int mBlendingIndex;
	double mBlendingWeight;

	BlendingIndexWeightPair() :
		mBlendingIndex(0),
		mBlendingWeight(0)
	{}
};

// Each Control Point in FBX is basically a vertex
// in the physical world. For example, a cube has 8
// vertices(Control Points) in FBX
// Joints are associated with Control Points in FBX
// The mapping is one joint corresponding to 4
// Control Points(Reverse of what is done in a game engine)
// As a result, this struct stores a XMFLOAT3 and a 
// vector of joint indices
struct CtrlPoint
{
	XMFLOAT3 mPosition;
	std::vector<BlendingIndexWeightPair> mBlendingInfo;

	CtrlPoint()
	{
		mBlendingInfo.reserve(4);
	}
};

// This stores the information of each key frame of each joint
// This is a linked list and each node is a snapshot of the
// global transformation of the joint at a certain frame
struct Keyframe
{
	FbxLongLong mFrameNum;
	FbxAMatrix mGlobalTransform;
	Keyframe* mNext;

	Keyframe() :
		mNext(nullptr)
	{}
};

// This is the actual representation of a joint in a game engine
struct Joint
{
	std::string mName;
	int mParentIndex;
	FbxAMatrix mGlobalBindposeInverse;
	Keyframe* mAnimation;
	FbxNode* mNode;

	Joint() :
		mNode(nullptr),
		mAnimation(nullptr)
	{
		mGlobalBindposeInverse.SetIdentity();
		mParentIndex = -1;
	}

	~Joint()
	{
		while (mAnimation)
		{
			Keyframe* temp = mAnimation->mNext;
			delete mAnimation;
			mAnimation = temp;
		}
	}
};

struct Skeleton
{
	std::vector<Joint> mJoints;
};

struct Triangle
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



struct PNTVertex
{
	XMFLOAT3 mPosition;
	XMFLOAT3 mNormal;
	XMFLOAT2 mUV;

	bool operator==(const PNTVertex& rhs) const
	{
		uint32_t position;
		uint32_t normal;
		uint32_t uv;

		XMVectorEqualR(&position, XMLoadFloat3(&(this->mPosition)), XMLoadFloat3(&rhs.mPosition));
		XMVectorEqualR(&normal, XMLoadFloat3(&(this->mNormal)), XMLoadFloat3(&rhs.mNormal));
		XMVectorEqualR(&uv, XMLoadFloat2(&(this->mUV)), XMLoadFloat2(&rhs.mUV));

		return XMComparisonAllTrue(position) && XMComparisonAllTrue(normal) && XMComparisonAllTrue(uv);
	}
};

struct VertexBlendingInfo
{
	unsigned int mBlendingIndex;
	double mBlendingWeight;

	VertexBlendingInfo() :
		mBlendingIndex(0),
		mBlendingWeight(0.0)
	{}

	bool operator < (const VertexBlendingInfo& rhs)
	{
		return (mBlendingWeight > rhs.mBlendingWeight);
	}
};

struct PNTIWVertex
{
	XMFLOAT3 mPosition;
	XMFLOAT3 mNormal;
	XMFLOAT2 mUV;
	std::vector<VertexBlendingInfo> mVertexBlendingInfos;

	void SortBlendingInfoByWeight()
	{

		std::sort(mVertexBlendingInfos.begin(), mVertexBlendingInfos.end());
	}

	bool operator==(const PNTIWVertex& rhs) const
	{
		bool sameBlendingInfo = true;

		// We only compare the blending info when there is blending info
		if (!(mVertexBlendingInfos.empty() && rhs.mVertexBlendingInfos.empty()))
		{
			// Each vertex should only have 4 index-weight blending info pairs
			for (unsigned int i = 0; i < 4; ++i)
			{
				if (mVertexBlendingInfos[i].mBlendingIndex != rhs.mVertexBlendingInfos[i].mBlendingIndex ||
					abs(mVertexBlendingInfos[i].mBlendingWeight - rhs.mVertexBlendingInfos[i].mBlendingWeight) > 0.001)
				{
					sameBlendingInfo = false;
					break;
				}
			}
		}

		bool result1 = CCustomMath::CompareVector3WithEpsilon(mPosition, rhs.mPosition);
		bool result2 = CCustomMath::CompareVector3WithEpsilon(mNormal, rhs.mNormal);
		bool result3 = CCustomMath::CompareVector2WithEpsilon(mUV, rhs.mUV);

		return result1 && result2 && result3 && sameBlendingInfo;
	}
};




//
class BIGGEST_DLL CFbxImporter :
	public CRef
{
public:
	CFbxImporter();
	~CFbxImporter();
	bool Initialize();
	bool LoadScene(const char* inFileName, const char* inOutputPath);

	void ExportFBX(MeshData& meshData);

private:
	FbxManager* mFBXManager;
	FbxScene* mFBXScene;
	std::string mInputFilePath;
	std::string mOutputFilePath;
	bool mHasAnimation;
	std::unordered_map<unsigned int, CtrlPoint*> mControlPoints;
	unsigned int mTriangleCount;
	std::vector<Triangle> mTriangles;
	std::vector<PNTIWVertex> mVertices;
	Skeleton mSkeleton;
	std::unordered_map<unsigned int, Material*> mMaterialLookUp;
	FbxLongLong mAnimationLength;
	std::string mAnimationName;
	LARGE_INTEGER mCPUFreq;


private:
	void ProcessGeometry(FbxNode* inNode);
	void ProcessSkeletonHierarchy(FbxNode* inRootNode);
	void ProcessSkeletonHierarchyRecursively(FbxNode* inNode, int inDepth, int myIndex, int inParentIndex);
	void ProcessControlPoints(FbxNode* inNode);
	void ProcessJointsAndAnimations(FbxNode* inNode);
	unsigned int FindJointIndexUsingName(const std::string& inJointName);
	void ProcessMesh(FbxNode* inNode);
	void ReadUV(FbxMesh* inMesh, int inCtrlPointIndex, int inTextureUVIndex, int inUVLayer, XMFLOAT2& outUV);
	void ReadNormal(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, XMFLOAT3& outNormal);
	void ReadBinormal(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, XMFLOAT3& outBinormal);
	void ReadTangent(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, XMFLOAT3& outTangent);
	void Optimize();
	int FindVertex(const PNTIWVertex& inTargetVertex, const std::vector<PNTIWVertex>& uniqueVertices);

	void AssociateMaterialToMesh(FbxNode* inNode);
	void ProcessMaterials(FbxNode* inNode);
	void ProcessMaterialAttribute(FbxSurfaceMaterial* inMaterial, unsigned int inMaterialIndex);
	void ProcessMaterialTexture(FbxSurfaceMaterial* inMaterial, Material* ioMaterial);
	void PrintMaterial();
	void PrintTriangles();

	void CleanupFbxManager();
	void WriteMeshToStream(std::ostream& inStream);
	void WriteAnimationToStream(std::ostream& inStream);
	FbxAMatrix CFbxImporter::GetGeometryTransformation(FbxNode* pNode);

};

BIGGEST_END