#include "Share.hlsl"

Texture2D g_DeferredAlbedoTex;
Texture2D g_DeferredNormalTex;
Texture2D g_DeferredDepthTex;
Texture2D g_DeferredSpecularTex;

Texture2D g_DeferredLightDiffuseTex;
Texture2D g_DeferredLightSpecularTex;


struct VS_OUTPUT_DEFERRED_DEFAULT
{
	float4 vPos : SV_POSITION;
	float4 vColor : COLOR;
	float2 vTex : TEXCOORD;
	float3 vNormal : TEXCOORD2;
	float4 vProjPos : TEXCOORD3;
};

struct VS_OUTPUT_DEFERRED_LIGHTACC_DIRECTIONAL
{
	float4 vPos : SV_POSITION;
	float2 vTex : TEXCOORD;
};

struct VS_OUTPUT_DEFERRED_LIGHTACC_POINT
{
	float4 vPos : SV_POSITION;
	float4 vProjPos : TEXCOORD;
};

struct PS_OUTPUT_DEFERRED_LIGHTACC
{
	float4 vDiffuse : SV_TARGET0;
	float4 vSpecular : SV_TARGET1;
};


VS_OUTPUT_DEFERRED_DEFAULT VS_Deferred_Default(VS_INPUT_DEFAULT input)
{
	VS_OUTPUT_DEFERRED_DEFAULT output = (VS_OUTPUT_DEFERRED_DEFAULT)0;

	output.vPos = mul(float4(input.vPos, 1.f), g_matWVP);

	output.vTex = input.vTex;

	float3 vViewNormal = mul(float4(input.vNormal, 0.0f), g_matWVP).xyz;

	// NOTE : 우선은 normal을 변경하지 않고 넣는다.
	//output.vNormal = normalize(vViewNormal);
	output.vNormal = input.vNormal;
	output.vProjPos = output.vPos;
	return output;
}


PS_OUTPUT_DEFERRED_GBUFFER PS_Deferred_Default(VS_OUTPUT_DEFERRED_DEFAULT input)
{
	PS_OUTPUT_DEFERRED_GBUFFER output = (PS_OUTPUT_DEFERRED_GBUFFER)0;

	output.vAlbedo = g_DiffuseTex.Sample(samAnisotropic, input.vTex);
	output.vAlbedo.w = 1.f;
	output.vNormal.xyz = input.vNormal.xyz *0.5f + 0.5;
	output.vNormal.w = input.vProjPos.w;
	output.vDepth = (float4)(input.vProjPos.z / input.vProjPos.w);

	return output;
}


VS_OUTPUT_DEFERRED_DEFAULT VS_Deferred_Skinning(VS_INPUT_SKNNING input)
{
	VS_OUTPUT_DEFERRED_DEFAULT output;

	float3 vPos = (float3)input.vPos;
	float3 vNormal = (float3)input.vNormal;

	ConvertToSkinningInfo(vPos, vNormal, input.vBoneIndex, input.vBlendWeight);

	float3 vViewNormal = mul(float4(vNormal, 1.0f), g_matWV).xyz;
	float fDot = dot(normalize(-vViewNormal), vNormal);

	output.vPos = mul(float4(vPos, 1.f), g_matWVP);
	output.vColor.xyzw = fDot;

	output.vNormal = normalize(vViewNormal);
	output.vTex = input.vTex;
	output.vProjPos = output.vPos;

	return output;
}


// Directional Light

VS_OUTPUT_DEFERRED_LIGHTACC_DIRECTIONAL VS_LightAcc_Directional(VS_INPUT_PT input)
{
	VS_OUTPUT_DEFERRED_LIGHTACC_DIRECTIONAL output = (VS_OUTPUT_DEFERRED_LIGHTACC_DIRECTIONAL)0;

	output.vPos = mul(float4(input.vPos, 1.f), g_matWVP);
	output.vTex = input.vTex;

	return output;
}

float4 PS_LightAcc_Directional(VS_OUTPUT_DEFERRED_LIGHTACC_DIRECTIONAL input) : SV_TARGET0
{

	float3 vNormal = ((g_DeferredNormalTex.Sample(samPoint, input.vTex).xyz));
	//vNormal = normalize(mul(float4(vNormal, 1.f), g_matWV).xyz);


	float3 vLightNormal = normalize(mul(float4(normalize(g_vDirLight), 1.f), g_matWV)).xyz;

	float3 fDot = g_vLightColor * dot(vLightNormal, vNormal);

	return float4(fDot.x,fDot.y,fDot.z,1.f);
}


// Point Light

VS_OUTPUT_DEFERRED_LIGHTACC_POINT VS_LightAcc_Point(VS_INPUT_P input)
{
	VS_OUTPUT_DEFERRED_LIGHTACC_POINT output = (VS_OUTPUT_DEFERRED_LIGHTACC_POINT)0;
	
	output.vPos = mul(float4(input.vPos, 1.f), g_matWVP);
	output.vProjPos = output.vPos;

	return output;
}

PS_OUTPUT_DEFERRED_LIGHTACC PS_LightAcc_Point(VS_OUTPUT_DEFERRED_LIGHTACC_POINT input)
{
	PS_OUTPUT_DEFERRED_LIGHTACC output = (PS_OUTPUT_DEFERRED_LIGHTACC)0;

	input.vProjPos.xy /= input.vProjPos.w;

	float2 vTex = 0.5f * (float2(input.vProjPos.x, -input.vProjPos.y) + 1.f);

	float3 vAlbedo = g_DeferredAlbedoTex.Sample(samPoint, vTex).xyz *2.f - 1.f;
	float4 vNormal = g_DeferredNormalTex.Sample(samPoint, vTex);
	float3 vDepth = g_DeferredDepthTex.Sample(samPoint, vTex).xyz;
	
	float4 vPos;

	vPos.x = vTex.x * 2.f - 1.f;
	vPos.y = -vTex.y * 2.f + 1.f;
	vPos.z = vDepth.r;
	vPos.w = 1.f;

	vPos = vPos * vNormal.w;
	
	vNormal.xyz = vNormal.xyz * 2.1f - 1.f;

	float3 vLightPos = mul(float4(g_vLightPos, 1.f), g_matView).xyz;

	float fDist = distance(vLightPos, vPos.xyz);

	float3 vLightDir = normalize(vPos.xyz - vLightPos).xyz;

	float fDot = max(0.f, dot(vNormal.xyz, vLightDir.xyz));

	float fIntensity = 0;
	
	if (fDist > g_fLightRadius)
		fIntensity = 0.f;
	else
		fIntensity = 1.f / fDist + fDot;

	output.vDiffuse = float4(g_vLightColor * fIntensity , 1);
	output.vSpecular = float4(1.f, 0.f, 0.f, 0.f);

	return output;
}

VS_OUTPUT_PT VS_DeferredCombine(VS_INPUT_PT input)
{
	VS_OUTPUT_PT output = (VS_OUTPUT_PT)0;

	output.vPos = mul(float4(input.vPos, 1.f), g_matWVP);
	output.vTex = input.vTex;

	return output;
}
 
float4 PS_DeferredCombine(VS_OUTPUT_PT input) :SV_TARGET
{
	float3 vAlbedo = g_DeferredAlbedoTex.Sample(samPoint,input.vTex).xyz;
	float3 vDiffuse = g_DeferredLightDiffuseTex.Sample(samPoint, input.vTex).xyz;
	float3 vSpecular = g_DeferredLightSpecularTex.Sample(samPoint, input.vTex).xyz;

	float3 vFinal = vAlbedo * vDiffuse;// +vSpecular;

	return float4(vFinal, 1.f);
}


BlendState LightAccBlendState
{
	BlendEnable[0] = true;
	SrcBlend[0] = SRC_COLOR;
	DestBlend[0] = SRC_COLOR;
	SrcBlendAlpha[0] = one;
	DestBlendAlpha[0] = one;
	BlendOp[0] = ADD;

	BlendEnable[1] = true;
	SrcBlend[1] = SRC_COLOR;
	DestBlend[1] = SRC_COLOR;
	SrcBlendAlpha[1] = one;
	DestBlendAlpha[1] = one;
	BlendOp[1] = ADD;
};

RasterizerState NoBackfaceCullingRS
{
	FillMode = Solid;
	CullMode = None;
	FrontCounterClockwise = false;
};

technique11 DeferredGBufferDefault
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_Deferred_Default()));
		SetPixelShader(CompileShader(ps_5_0, PS_Deferred_Default()));
	}
}

technique11 DeferredGBufferSkinning
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_Deferred_Skinning()));
		SetPixelShader(CompileShader(ps_5_0, PS_Deferred_Default()));
	}
}

technique11 LightAccDirectional
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_LightAcc_Directional()));
		SetPixelShader(CompileShader(ps_5_0, PS_LightAcc_Directional()));
		//SetBlendState(LightAccBlendState, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
	}
}

technique11 LightAccPoint
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_LightAcc_Point()));
		SetPixelShader(CompileShader(ps_5_0, PS_LightAcc_Point()));
		SetRasterizerState(NoBackfaceCullingRS);
		//SetBlendState(LightAccBlendState, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
	}
}

technique11 DeferredCombine
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_DeferredCombine()));
		SetPixelShader(CompileShader(ps_5_0, PS_DeferredCombine()));
	}	
}