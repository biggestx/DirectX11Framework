#include "Animation3D.h"
#include "../Rendering/Shader/Shader.h"
#include "../Core/Device.h"
#include "../Resource/ResourcesManager.h"
#include "../Resource/Texture.h"
#include "../Rendering/RenderManager.h"
#include "../Core/Input.h"

USING_BIGGEST

CAnimation3D::CAnimation3D() :
m_pVTFTexture(NULL),
m_pShader(NULL),
m_iBoneCount(0),
m_iCurAnimationKey(0)
{
}

CAnimation3D::CAnimation3D(const CAnimation3D& com)
{
	*this = com;
}

CAnimation3D::~CAnimation3D()
{

	SAFE_RELEASE(m_pVTFTexture);
}


CComponent* CAnimation3D::Clone()
{
	return new CAnimation3D(*this);
}

bool CAnimation3D::Init()
{
	m_eComType = CT_ANIMATION;

	//m_pShader = SINGLE(CRenderManager)->FindShader(SHADER_FXKEY_DEFAULT);

	m_fWeight = 0.f;

	return true;
}

void CAnimation3D::InitEntries(const aiScene* pScene)
{
	m_meshEntries.resize(pScene->mNumMeshes);

	int iVertices = 0;
	int iIndices = 0;

	for (int i = 0; i < m_meshEntries.size(); ++i)
	{
		m_meshEntries[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
		m_meshEntries[i].BaseVertex = iVertices;
		m_meshEntries[i].BaseIndex = iIndices;

		iVertices += pScene->mMeshes[i]->mNumVertices;
		iIndices += m_meshEntries[i].NumIndices;
	}

}

void CAnimation3D::Update(float fTime)
{
	if (SINGLE(CInput)->FindKey('w')->bPress)
	{
		if (m_iCurAnimationKey >= m_pAssimpScene->mNumAnimations-1)
			m_iCurAnimationKey = 0;
		else
			m_iCurAnimationKey ++;
	}
	m_fWeight += fTime;
	CalculateBones(m_fWeight);
}
void CAnimation3D::LateUpdate(float fTime)
{

}
void CAnimation3D::Render(float fTime)
{

}



void CAnimation3D::CreateTextureForVTF()
{
	int iWidth = m_iBoneCount * 4;;

	m_pVTFTexture = CTexture::Create("VTFTexture");
	if (!m_pVTFTexture->CreateWriteableTexture(iWidth, 1))
	{
		MessageBox(NULL, L"Failed to create VTF Texture", L"Fail", MB_OK);
		SAFE_DELETE(m_pVTFTexture);
	}

}




void CAnimation3D::CalculateBones(const float& fTime)
{
	vector<XMFLOAT4X4> matArr;
	BoneTransform(fTime, matArr);

	D3D11_MAPPED_SUBRESOURCE resource;
	ID3D11Texture2D* pVTFTexture = m_pVTFTexture->GetTexture2D();


	DEVICECONTEXT->Map(pVTFTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

	XMFLOAT4X4* matrices = (XMFLOAT4X4*)resource.pData;

	for (int i = 0; i < matArr.size(); ++i)
	{
		XMFLOAT4X4 matFloat = matArr[i];


		matrices[i] = matFloat;
	}

	DEVICECONTEXT->Unmap(pVTFTexture, 0);

}


void CAnimation3D::ConstantTable()
{
	if (!m_pShader)
		return;

	int iWidth = m_iBoneCount * 4;
	m_pShader->SetData(SHADER_HANDLE_SHARE_TEXTRUE_VTF, m_pVTFTexture->GetResourceView());
	m_pShader->SetData(SHADER_HANDLE_SHARE_INT_VTFTEXTUREWIDTH, &iWidth);

}



void CAnimation3D::BoneTransform(float fTime, vector<XMFLOAT4X4>& matArr)
{
	XMMATRIX matIdentity;
	matIdentity = XMMatrixIdentity();

	float fTickPerSecond = (float)(m_pAssimpScene->mAnimations[m_iCurAnimationKey]->mTicksPerSecond != 0 ? m_pAssimpScene->mAnimations[m_iCurAnimationKey]->mTicksPerSecond : 25.0f);
	float fTimeInTick = fTime * fTickPerSecond;
	float fAnimationTime = fmod(fTimeInTick, (float)m_pAssimpScene->mAnimations[m_iCurAnimationKey]->mDuration);

	ReadNodeHeirarchy(fAnimationTime, m_pAssimpScene->mRootNode, matIdentity);
	
	matArr.resize(m_iBoneCount);

	for (int i = 0; i < m_iBoneCount; ++i)
		matArr[i] = m_BoneInfoArr[i].matFinal;
}

void CAnimation3D::ReadNodeHeirarchy(float fAnimationTime, const aiNode* pNode, const XMMATRIX& matParent)
{
	string strNodeName = pNode->mName.data;

	const aiAnimation* pAnimation = m_pAssimpScene->mAnimations[m_iCurAnimationKey];

	XMMATRIX matNodeTransformation = _MATRIX(&ConvertAssimpMatrixToXMFLOAT4X4((pNode->mTransformation)));
	matNodeTransformation = XMMatrixTranspose(matNodeTransformation);
	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, strNodeName);

	if (pNodeAnim)
	{
		aiVector3D vScale;
		InterpolateScaling(vScale, fAnimationTime, pNodeAnim);
		XMMATRIX matScale = XMMatrixScaling(vScale.x, vScale.y, vScale.z);

		aiQuaternion qRot;
		InterpolateRotation(qRot, fAnimationTime, pNodeAnim);
		XMMATRIX matRot;
		
		XMVECTOR vRot = XMVectorSet(qRot.x, qRot.y, qRot.z, qRot.w);
		matRot = XMMatrixRotationQuaternion(XMVECTOR(vRot));

		aiVector3D vTrans;
		InterpolatePosition(vTrans, fAnimationTime, pNodeAnim);
		XMMATRIX matTrans;
		matTrans = XMMatrixTranslation(vTrans.x, vTrans.y, vTrans.z);


		matNodeTransformation = matScale * matRot * matTrans;
	}

	XMMATRIX matGlobal = matNodeTransformation * matParent;

	if (m_boneMapping.find(strNodeName) != m_boneMapping.end())
	{
		int iBoneIdx = m_boneMapping[strNodeName];

		XMMATRIX matGlobalInverse = XMLoadFloat4x4(&m_matGlobalInverse);
		matGlobalInverse = XMMatrixTranspose(matGlobalInverse);

		XMMATRIX matOffset = XMLoadFloat4x4(&m_BoneInfoArr[iBoneIdx].matOffset);
		matOffset = XMMatrixTranspose(matOffset);


		XMMATRIX matFinal;
	/*	matFinal = XMMatrixMultiply(matOffset, matGlobal);
		matFinal = XMMatrixMultiply(matFinal, matGlobalInverse);*/
		matFinal = matOffset * matGlobal * matGlobalInverse;

		XMStoreFloat4x4(&m_BoneInfoArr[iBoneIdx].matFinal, matFinal);

	}

	for (int i = 0; i < pNode->mNumChildren; ++i)
	{
		ReadNodeHeirarchy(fAnimationTime, pNode->mChildren[i], matGlobal);
	}


}

void CAnimation3D::InterpolatePosition(aiVector3D& vOut, float fAnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1)
	{
		vOut = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	int iPosIdx = FindPosition(fAnimationTime, pNodeAnim);
	int iNextPosIdx = (iPosIdx + 1);
	assert(iNextPosIdx < pNodeAnim->mNumPositionKeys);

	float fDeltaTime = (float)(pNodeAnim->mPositionKeys[iNextPosIdx].mTime - pNodeAnim->mPositionKeys[iPosIdx].mTime);
	float fFactor = (fAnimationTime - (float)pNodeAnim->mPositionKeys[iPosIdx].mTime) / fDeltaTime;
	assert(fFactor >= 0.f && fFactor <= 1.0f);

	const aiVector3D& vStart = pNodeAnim->mPositionKeys[iPosIdx].mValue;
	const aiVector3D& vEnd = pNodeAnim->mPositionKeys[iNextPosIdx].mValue;

	aiVector3D vDelta = vEnd - vStart;
	vOut = vStart + fFactor * vDelta;


}

void CAnimation3D::InterpolateRotation(aiQuaternion& qOut, float fAnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumRotationKeys == 1) {
		qOut = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	int iRotIdx = FindRotation(fAnimationTime, pNodeAnim);
	int iNextRotIdx = (iRotIdx + 1);
	assert(iNextRotIdx < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[iNextRotIdx].mTime - pNodeAnim->mRotationKeys[iRotIdx].mTime);
	float Factor = (fAnimationTime - (float)pNodeAnim->mRotationKeys[iRotIdx].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[iRotIdx].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[iNextRotIdx].mValue;
	aiQuaternion::Interpolate(qOut, StartRotationQ, EndRotationQ, Factor);
	qOut = qOut.Normalize();
}


void CAnimation3D::InterpolateScaling(aiVector3D& vOut, float fAnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1) {
		vOut = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	int iScaleIdx = FindScaling(fAnimationTime, pNodeAnim);
	int iNextScaleIdx = (iScaleIdx + 1);
	assert(iNextScaleIdx < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[iNextScaleIdx].mTime - pNodeAnim->mScalingKeys[iScaleIdx].mTime);
	float Factor = (fAnimationTime - (float)pNodeAnim->mScalingKeys[iScaleIdx].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[iScaleIdx].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[iNextScaleIdx].mValue;
	aiVector3D Delta = End - Start;
	vOut = Start + Factor * Delta;
}


int CAnimation3D::FindPosition(float fAnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (int i = 0; i < pNodeAnim->mNumPositionKeys - 1; ++i)
	{
		if (fAnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime)
			return i;
	}
	
	return 0;
}

int CAnimation3D::FindRotation(float fAnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (int i = 0; i < pNodeAnim->mNumRotationKeys - 1; ++i)
	{
		if (fAnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime)
			return i;
	}


	return 0;
}

int CAnimation3D::FindScaling(float fAnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (int i = 0; i < pNodeAnim->mNumScalingKeys - 1; ++i)
	{
		if (fAnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime)
			return i;
	}


	return 0;

}


const aiNodeAnim* CAnimation3D::FindNodeAnim(const aiAnimation* pAnimation, const string strNodeName)
{
	for (int i = 0; i < pAnimation->mNumChannels; ++i)
	{
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (string(pNodeAnim->mNodeName.data) == strNodeName)
		{
			return pNodeAnim;
		}
	}

	return NULL;
}


void CAnimation3D::LoadBones(int iMeshIdx, const aiMesh* pMesh, vector<Vertex>& boneArr)
{
	for (int i = 0; i < pMesh->mNumBones; ++i)
	{
		int iBoneIdx = 0;
		string strBoneName = pMesh->mBones[i]->mName.data;

		if (m_boneMapping.find(strBoneName) == m_boneMapping.end())
		{
			iBoneIdx = m_iBoneCount++;

			BoneInfo tBone;
			m_BoneInfoArr.push_back(tBone);

		}
		else
			iBoneIdx = m_boneMapping[strBoneName];

		m_boneMapping[strBoneName] = iBoneIdx;
		m_BoneInfoArr[iBoneIdx].matOffset = ConvertAssimpMatrixToXMFLOAT4X4(pMesh->mBones[i]->mOffsetMatrix);

		for (int j = 0; j < pMesh->mBones[i]->mNumWeights; ++j)
		{
			int idx = pMesh->mBones[i]->mWeights[j].mVertexId;
			float fWeight = pMesh->mBones[i]->mWeights[j].mWeight;

			boneArr[idx].AddBoneIndex(iBoneIdx);
			boneArr[idx].AddBlendWeight(fWeight);
		}

	}
}


XMFLOAT4X4 CAnimation3D::ConvertAssimpMatrixToXMFLOAT4X4(aiMatrix4x4 matAssimp)
{
	XMFLOAT4X4 matReturn;
	matReturn.m;
	memcpy(&matReturn, &matAssimp, sizeof(aiMatrix4x4));
	return matReturn;
}

void CAnimation3D::DeleteBones(BoneNode* pNode)
{
	for (int i = 0; i < pNode->childNodeList.size(); ++i)
	{
		DeleteBones(pNode->childNodeList[i]);
	}



	delete pNode;
}