#pragma once
#include "Component.h"

BIGGEST_BEGIN

typedef struct _tagBonePair
{
	int iNum;
	class CResBuffer* pBuffer;
	vector<Bone>	Bones;

	_tagBonePair() : iNum(0){}

}BonePair, *PBonePair;

typedef struct _tagBoneInfo
{
	XMFLOAT4X4 matOffset;
	XMFLOAT4X4 matFinal;

	_tagBoneInfo()
	{
		XMStoreFloat4x4(&matOffset,XMMatrixIdentity());
		XMStoreFloat4x4(&matFinal, XMMatrixIdentity());
	}
}BoneInfo, *PBoneInfo;


typedef struct MeshEntry {
	MeshEntry()
	{
		NumIndices = 0;
		BaseVertex = 0;
		BaseIndex = 0;
		//MaterialIndex = INVALID_MATERIAL;
	}

	unsigned int NumIndices;
	unsigned int BaseVertex;
	unsigned int BaseIndex;
	unsigned int MaterialIndex;
};



class BIGGEST_DLL CAnimation3D :
	public CComponent
{
	CREATE_INSTANCE_TAG(CAnimation3D)
private:
	class CTexture*		m_pVTFTexture;

	vector<class CBuffer*>	m_bufferList;

	int m_iBoneCount;
	float m_fWeight;

	int m_iCurAnimationKey;

	vector<aiAnimation*> m_animationArr;
	XMFLOAT4X4 m_matGlobalInverse;
	map<string, int> m_boneMapping;
	vector<BoneInfo> m_BoneInfoArr;


	vector<MeshEntry> m_meshEntries;

	const aiScene* m_pAssimpScene;

	class CShader* m_pShader;

public:

public:
	void AddAnimation(aiAnimation* pAnimation)
	{
		m_animationArr.push_back(pAnimation);
	}

public:
	void SetAssimpScene(const aiScene* pAssimpScene)
	{
		m_pAssimpScene = pAssimpScene;
	}


	void SetShader(CShader* pShader)
	{
		m_pShader = pShader;
	}



	void SetGlobalInverseMatrix(aiMatrix4x4 mat)
	{
		m_matGlobalInverse = ConvertAssimpMatrixToXMFLOAT4X4(mat);
	}

	void SetAnimationKey(int iAnimationKey)
	{
		if (m_pAssimpScene->mNumAnimations < iAnimationKey)
			return;

		m_iCurAnimationKey = iAnimationKey;
	}


public:
	void AddResource(CBuffer* pBuffer)
	{
		m_bufferList.push_back(pBuffer);
	}


	void CreateTextureForVTF();

public:

	void InitEntries(const aiScene* pScene);

	void DeleteBones(BoneNode* pNode);

private:
	void CalculateBones(const float& fTime);

public:
	void BoneTransform(float fTime, vector<XMFLOAT4X4>& matArr);
	void ReadNodeHeirarchy(float fAnimationTime, const aiNode* pNode, const XMMATRIX& matParent);

	void InterpolatePosition(aiVector3D& vOut, float fAnimationTime, const aiNodeAnim* pNodeAnim);
	void InterpolateRotation(aiQuaternion& qOut, float fAnimationTime, const aiNodeAnim* pNodeAnim);
	void InterpolateScaling(aiVector3D& vOut, float fAnimationTime, const aiNodeAnim* pNodeAnim);
	int FindPosition(float fAnimationTime, const aiNodeAnim* pNodeAnim);
	int FindRotation(float fAnimationTime, const aiNodeAnim* pNodeAnim);
	int FindScaling(float fAnimationTime, const aiNodeAnim* pNodeAnim);

	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const string strNodeName);

	void LoadBones(int iMeshIdx, const aiMesh* pMesh, vector<Vertex>& boneArr);

	XMFLOAT4X4 ConvertAssimpMatrixToXMFLOAT4X4(aiMatrix4x4 matAssimp);

public:
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CComponent* Clone();
	virtual void ConstantTable();

public:
	CAnimation3D();
	CAnimation3D(const CAnimation3D& com);
	~CAnimation3D();
};

BIGGEST_END