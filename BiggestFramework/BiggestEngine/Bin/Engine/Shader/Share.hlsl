float4x4 g_matWorld;
float4x4 g_matView;
float4x4 g_matProj;
float4x4 g_matWV;
float4x4 g_matWVP;

float4x4 g_matInvView;

Texture2D g_DiffuseTex;
Texture2D g_SpecularTex;
Texture2D g_NormalTex;
Texture2D g_EmissiveTex;

Texture2D g_VTFTex;

int g_iBoneVTFWidth;

float4x4 g_matBones[60];


float3 g_vDirLight = float3(1.0f, 0.0f, 0.0f);

float3 g_vLightColor;
float3 g_vLightPos;
float g_fLightRadius;


struct VS_INPUT_SKNNING
{
	float3 vPos : POSITION;
	float3 vNormal : NORMAL;
	float2 vTex : TEXCOORD;
	float4 vBlendWeight : WEIGHTS;
	uint4 vBoneIndex : BONEINDICES;
};

struct VS_OUTPUT_SKNNING
{
	float4 vPos : SV_POSITION;
	float4 vColor : COLOR;
	float2 vTex : TEXCOORD;
};


struct VS_INPUT_DEFAULT
{
	float3 vPos : POSITION;
	float4 vColor : COLOR;
	float3 vNormal : NORMAL;
	float2 vTex : TEXCOORD;
};

struct VS_OUTPUT_DEFAULT
{
	float4 vPos : SV_POSITION;
	float4 vColor : COLOR;
	float2 vTex : TEXCOORD;
};

struct VS_INPUT_P
{
	float3 vPos : POSITION;
};

struct VS_INPUT_PT
{
	float3 vPos : POSITION;
	float2 vTex : TEXCOORD;
};

struct VS_INPUT_PNT
{
	float3 vPos : POSITION;
	float3 vNormal : NORMAL;
	float2 vTex : TEXCOORD;
};

struct VS_OUTPUT_PT
{
	float4 vPos : SV_POSITION;
	float2 vTex : TEXCOORD;
};

struct PS_OUTPUT_DEFERRED_GBUFFER
{
	float4 vAlbedo : SV_TARGET0;
	float4 vNormal : SV_TARGET1;
	float4 vDepth :  SV_TARGET2;
};



SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;

	AddressU = WRAP;
	AddressV = WRAP;
};

RasterizerState WireframeRS
{
	FillMode = Wireframe;
	//CullMode = Back;
	//FrontCounterClockwise = false;
};


SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
};


float4x4 GetBoneMatrix(int iIndex)
{
	float2	vBoneUV;

	vBoneUV = float2((iIndex * 4 + 0.5) / (float)g_iBoneVTFWidth, 0);

	float4x4	matBone =
	{
		g_VTFTex.SampleLevel(samPoint, vBoneUV, 0),
		g_VTFTex.SampleLevel(samPoint, vBoneUV + float2(1.f / g_iBoneVTFWidth, 0.f), 0),
		g_VTFTex.SampleLevel(samPoint, vBoneUV + float2(2.f / g_iBoneVTFWidth, 0.f), 0),
		g_VTFTex.SampleLevel(samPoint, vBoneUV + float2(3.f / g_iBoneVTFWidth, 0.f), 0),
	};

	return matBone;
}

void ConvertToSkinningInfo(inout float3 vOutPos, inout float3 vOutNormal, uint4 iBoneIndices, float4 fBlendWeights)
{
	float4 fVTF = g_VTFTex.Sample(samPoint, iBoneIndices.x * 4, 0.5);

	float3 vPos = (float3)0;
	float3 vNormal = (float3)0;


	int iBoneIndexArray[4] = (int[4])iBoneIndices;
	float fBlendWeightsArray[4] = (float[4])fBlendWeights;


	for (int i = 0; i < 4; ++i)
	{
		vPos += fBlendWeightsArray[i] * mul(float4(vOutPos, 1.0f), GetBoneMatrix(iBoneIndexArray[i])).xyz;
		vNormal += fBlendWeightsArray[i] * mul(float4(vOutNormal, 1.0f), GetBoneMatrix(iBoneIndexArray[i])).xyz;
	}

	vOutPos = vPos;
	vOutNormal = vNormal;
}
