#pragma once
#include "Component.h"

BIGGEST_BEGIN

class BIGGEST_DLL CTransform :
	public CComponent
{
	FRIEND_CLASS(CGameObject)
		CREATE_INSTANCE_TAG(CTransform)

protected:
	Vec3		m_vLocalPos;
	Vec3		m_vWorldPos;

	Vec3		m_vLocalRotation;
	Vec3		m_vWorldRotation;

	Vec3		m_vOriginScale;
	Vec3		m_vScale;

	Vec3		m_vLocalAxis[WA_MAX];
	Vec3		m_vWorldAxis[WA_MAX];

	Vec3		m_vRotationAxis;
	float		m_fAxisAngle;

	Vec3		m_vOriginView;


	TRANSFORM_ROTATION_TYPE m_eRotationType;

public:
	
public:
	

	Vec3 GetLocalPos()
	{
		return m_vLocalPos;
	}

	Vec3 GetWorldPos()
	{
		return m_vWorldPos;
	}

	Vec3 GetScale()
	{
		return m_vScale;
	}

	Vec3 GetOriginScale()
	{
		return m_vOriginScale;
	}

	Vec3 GetLocalRotation()
	{
		return m_vLocalRotation;
	}

	Vec3 GetWorldRotation()
	{
		return m_vWorldRotation;
	}

	Vec3 GetLocalAxis(int idx = WA_X)
	{
		return m_vLocalAxis[idx];
	}

	Vec3 GetWorldAxis(int idx = WA_X)
	{
		return m_vWorldAxis[idx];
	}

public:
	void SetRotationMatrix(const Matrix& matRot)
	{
		m_matRotation = matRot;
	}

	void SetOriginView(const Vec3& vView)
	{
		m_vOriginView = vView;
	}

	void SetRotationAxis(const Vec3& vAxis)
	{
		m_vRotationAxis = vAxis;
	}

	void SetRotationAxis(float x, float y, float z)
	{
		m_vRotationAxis.x = x;
		m_vRotationAxis.y = y;
		m_vRotationAxis.z = z;
	}

	void SetRotationAxisAngle(float fAngle)
	{
		m_fAxisAngle = fAngle;
	}

	void SetWorldAxis(const PVec3 pAxis)
	{
		for (int i = 0; i < WA_MAX; ++i)
		{
			m_vWorldAxis[i] = pAxis[i];
		}
	}

	void SetWorldAxisZ(const Vec3& vAxis)
	{
		m_vWorldAxis[WA_Z] = vAxis;
	}




	void SetLocalPos(const Vec3& vLocalPos)
	{
		m_vLocalPos = vLocalPos;
	}

	void SetLocalPos(float x, float y, float z)
	{
		m_vLocalPos.x = x;
		m_vLocalPos.y = y;
		m_vLocalPos.z = z;
	}

	void SetWorldPos(const Vec3& vWorldPos)
	{
		m_vWorldPos = vWorldPos;
	}


	void SetWorldPos(float x, float y, float z)
	{
		m_vWorldPos.x = x;
		m_vWorldPos.y = y;
		m_vWorldPos.z = z;
	}

	void SetScale(const Vec3& vScale)
	{
		m_vOriginScale = vScale;
	}

	void SetScale(float x, float y, float z)
	{
		m_vOriginScale.x = x;
		m_vOriginScale.y = y;
		m_vOriginScale.z = z;
	}

	void SetLocalRotation(float x, float y, float z)
	{
		m_vLocalRotation.x = x;
		m_vLocalRotation.y = y;
		m_vLocalRotation.z = z;
	}

	void SetLocalRotation(const Vec3& vRot)
	{
		m_vLocalRotation = vRot;
	}

	void LocalRotation(const Vec3& vRot)
	{
		m_vLocalRotation += vRot;
	}

	void LocalRotation(float x, float y, float z)
	{
		m_vLocalRotation.x += x;
		m_vLocalRotation.y += y;
		m_vLocalRotation.z += z;
	}

	void LocalRotationX(float x)
	{
		m_vLocalRotation.x += x;
	}

	void LocalRotationY(float y)
	{
		m_vLocalRotation.y += y;
	}

	void LocalRotationZ(float z)
	{
		m_vLocalRotation.z += z;
	}

	void SetWorldRotation(float x, float y, float z)
	{
		m_vWorldRotation.x = x;
		m_vWorldRotation.y = y;
		m_vWorldRotation.z = z;
		CalculateWorldRotation();
	}

	void SetWorldRotation(const Vec3& vRot)
	{
		m_vWorldRotation = vRot;
		CalculateWorldRotation();
	}

	void WorldRotation(const Vec3& vRot)
	{
		m_vWorldRotation += vRot;
		CalculateWorldRotation();
	}

	void WorldRotation(float x, float y, float z)
	{
		m_vWorldRotation.x += x;
		m_vWorldRotation.y += y;
		m_vWorldRotation.z += z;
		CalculateWorldRotation();
	}

	void WorldRotationX(float x)
	{
		m_vWorldRotation.x += x;
		CalculateWorldRotation();
	}

	void WorldRotationY(float y)
	{
		m_vWorldRotation.y += y;
		CalculateWorldRotation();
	}

	void WorldRotationZ(float z)
	{
		m_vWorldRotation.z += z;
		CalculateWorldRotation();
	}

public:
	void CalculateLocalMatrix();



	void CalculateRotation();
	void CalculateWorldRotation();
	
	void CalculateAxis();

	void Move(WORLD_AXIS wa, float fSpeed);

protected:
	Matrix		m_matScale;
	Matrix		m_matRotation;
	Matrix		m_matLocalRotation;
	Matrix		m_matTrans;
	Matrix		m_matParent;
	Matrix		m_matWorld;

	Matrix		m_matLocal;
	Matrix		m_matLocalTrans;
	Matrix		m_matLocalRot;
	Matrix		m_matOriginScale;
	

public:
	Matrix GetWorldMatrix()
	{
		return m_matWorld;
	}

	Matrix GetTranslationMatrix()
	{
		return m_matTrans;
	}

	Matrix GetRotationMatrix()
	{
		return m_matRotation;
	}

private:
	bool		m_bStatic;

public:
	void SetDevice();
	void SetShader(class CShader* pShader);
	void Copy(CTransform* pTransform);

public:
	virtual bool Init();
	virtual void Update(float fTime);
	virtual void LateUpdate(float fTime);
	virtual CTransform* Clone();

protected:
	CTransform();
	CTransform(const CTransform& transform);
	~CTransform();
};

BIGGEST_END