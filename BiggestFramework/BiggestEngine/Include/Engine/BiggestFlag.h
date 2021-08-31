#pragma once

//
//
//typedef D3DXVECTOR2		Vec2;
//typedef D3DXVECTOR2*	PVec2;
//typedef D3DXVECTOR3		Vec3;
//typedef D3DXVECTOR3*	PVec3;
//typedef D3DXVECTOR4		Vec4;
//typedef D3DXVECTOR4*	PVec4;
//typedef D3DXMATRIXA16	Matrix;
//typedef D3DXMATRIXA16*	PMatrix;
//typedef D3DXPLANE		Plane;
//typedef D3DXPLANE*		PPlane;


//typedef XMFLOAT2	Vec2;

typedef XMFLOAT4X4 Matrix;


typedef struct _tagVec2 : public XMFLOAT2
{
	_tagVec2 operator+ (_tagVec2 v1)
	{
		Vec2 vRet;
		vRet.x += v1.x;
		vRet.y += v1.y;
		return vRet;
	}

	_tagVec2 operator- (_tagVec2 v1)
	{
		Vec2 vRet;
		vRet.x -= v1.x;
		vRet.y -= v1.y;
		return vRet;
	}

	_tagVec2 operator* (_tagVec2 v1)
	{
		Vec2 vRet;
		vRet.x *= v1.x;
		vRet.y *= v1.y;
		return vRet;
	}

	_tagVec2 operator/ (_tagVec2 v1)
	{
		Vec2 vRet;
		vRet.x /= v1.x;
		vRet.y /= v1.y;
		return vRet;
	}

	void operator+= (_tagVec2 v1)
	{
		this->x += v1.x;
		this->y += v1.y;
	}

	_tagVec2() : XMFLOAT2(0.f, 0.f){}
	_tagVec2(float _x, float _y) : XMFLOAT2(_x, _y){}

}Vec2,*PVec2;

typedef struct _tagVec3 : public XMFLOAT3
{
	_tagVec3 operator+ (_tagVec3 v1)
	{
		_tagVec3 vRet;
		vRet.x += v1.x;
		vRet.y += v1.y;
		vRet.z += v1.z;
		return vRet;
	}

	_tagVec3 operator- (_tagVec3 v1)
	{
		_tagVec3 vRet;
		vRet.x -= v1.x;
		vRet.y -= v1.y;
		vRet.z -= v1.z;
		return vRet;
	}

	_tagVec3 operator* (_tagVec3 v1)
	{
		_tagVec3 vRet;
		vRet.x *= v1.x;
		vRet.y *= v1.y;
		vRet.z *= v1.z;
		return vRet;
	}

	_tagVec3 operator/ (_tagVec3 v1)
	{
		_tagVec3 vRet;
		vRet.x /= v1.x;
		vRet.y /= v1.y;
		vRet.z /= v1.z;
		return vRet;
	}

	void operator+= (_tagVec3 v1)
	{
		this->x += v1.x;
		this->y += v1.y;
		this->z += v1.z;
	}

	_tagVec3() : XMFLOAT3(0.f, 0.f,0.f){}
	_tagVec3(float _x, float _y, float _z) : XMFLOAT3(_x, _y,_z){}

}Vec3, *PVec3;

typedef struct _tagVec4: public XMFLOAT4
{
	_tagVec4 operator+ (_tagVec4 v1)
	{
		_tagVec4 vRet;
		vRet.x += v1.x;
		vRet.y += v1.y;
		vRet.z += v1.z;
		vRet.w += v1.w;
		return vRet;
	}

	_tagVec4 operator- (_tagVec4 v1)
	{
		_tagVec4 vRet;
		vRet.x -= v1.x;
		vRet.y -= v1.y;
		vRet.z -= v1.z;
		vRet.w -= v1.w;
		return vRet;
	}

	_tagVec4 operator* (_tagVec4 v1)
	{
		_tagVec4 vRet;
		vRet.x *= v1.x;
		vRet.y *= v1.y;
		vRet.z *= v1.z;
		vRet.w *= v1.w;
		return vRet;
	}

	_tagVec4 operator/ (_tagVec4 v1)
	{
		_tagVec4 vRet;
		vRet.x /= v1.x;
		vRet.y /= v1.y;
		vRet.z /= v1.z;
		vRet.w /= v1.w;
		return vRet;
	}
	void operator+= (_tagVec4 v1)
	{
		this->x += v1.x;
		this->y += v1.y;
		this->z += v1.z;
		this->w += v1.w;
	}

	_tagVec4() : XMFLOAT4(0.f, 0.f, 0.f,0.f){}
	_tagVec4(float _x, float _y, float _z, float _w) : XMFLOAT4(_x, _y, _z, _w){}

}Vec4, *PVec4;


enum SCENE_CHANGE
{
	SC_BACK = -1,
	SC_NONE,
	SC_NEXT,
	SC_DNEXT
};

enum SCENE_CHANGE_TYPE
{
	SCT_REPLACE,
	SCT_PUSH
};

// Component Type
enum COMPONENT_TYPE
{
	CT_RENDERER,
	CT_TRANSFORM,
	CT_SCRIPT,
	CT_CAMERA,
	CT_ANIMATION,
	CT_TERRAIN,
	CT_LIGHT
};

// World Axis
enum WORLD_AXIS
{
	WA_X,
	WA_Y,
	WA_Z,
	WA_MAX
};


// Camera Type
enum CAMERA_TYPE
{
	CT_PERSPECTIVE,
	CT_ORTHOGONAL
};

// Camera View Type
enum CAMERA_VIEW_TYPE
{
	CVT_AXIS,
	CVT_LOOKAT,
};

// Rotation Type
enum TRANSFORM_ROTATION_TYPE
{
	TRT_XYZ,
	TRT_AXIS,
};

// Light Type
enum LIGHT_TYPE
{
	LT_DIR = 1,
	LT_POINT,
	LT_SPOT
};


// Shader Constant Data Type
enum SHADER_CONSTANT_DATA_TYPE
{
	SCDT_BOOL,
	SCDT_BOOL_ARRAY,
	SCDT_INT,
	SCDT_INT_ARRAY,
	SCDT_FLOAT,
	SCDT_FLOAT_ARRAY,
	SCDT_FLOAT2,
	SCDT_FLOAT3,
	SCDT_FLOAT4,
	SCDT_MATRIX,
	SCDT_MATRIX_ARRAY,
	SCDT_TEXTURE,
	SCDT_USER_DATA,
	SCDT_VAR,
};


// Scene ObjectPool Type
enum SCENE_OBJECTPOOL_TYPE
{
	SOT_CURRENT,
	SOT_PREV,
	SOT_NEXT,
	SOT_MAX
};

// Resource Flag
enum RESOURCE_FLAG
{
	RF_SHADER,
	RF_VERTEXSHADER,
	RF_TEXTURE,
	RF_RENDERTARGET,
	RF_RESBUFFER,
	RF_MATERIAL,
};

// Rendering Mode
enum RENDER_MODE
{
	RM_FORWARD,
	RM_DEFERRED
};

// Rendering Flag
enum RENDERING_FLAG
{
	RF_ENVIRONMENT,
	RF_NORMAL_OBJECT,
	RF_ALPHA_OBJECT,
	RF_UI,
	RF_MAX
};

enum TEXTUREMAP_FLAG
{
	TMF_DIFFUSE,
	TMF_SPECULAR,
	TMF_AMBIENT,
	TMF_EMISSIVE,
};

