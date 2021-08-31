#include "CustomMath.h"

USING_BIGGEST

const XMFLOAT2 CCustomMath::vector2Epsilon = XMFLOAT2(0.00001f, 0.00001f);
const XMFLOAT3 CCustomMath::vector3Epsilon = XMFLOAT3(0.00001f, 0.00001f, 0.00001f);


bool CCustomMath::CompareVector3WithEpsilon(const XMFLOAT3& lhs, const XMFLOAT3& rhs)
{
	return XMVector3NearEqual(XMLoadFloat3(&lhs), XMLoadFloat3(&rhs), XMLoadFloat3(&vector3Epsilon)) == TRUE;
}

bool CCustomMath::CompareVector2WithEpsilon(const XMFLOAT2& lhs, const XMFLOAT2& rhs)
{
	return XMVector3NearEqual(XMLoadFloat2(&lhs), XMLoadFloat2(&rhs), XMLoadFloat2(&vector2Epsilon)) == TRUE;
}
