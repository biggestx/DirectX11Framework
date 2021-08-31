#include "Share.hlsl"

Texture2D g_TerrainDiffuseTex;
Texture2D g_TerrainHeightTex;

struct VS_OUTPUT_TERRAIN_DEFFERED
{
	float4 vPos : SV_POSITION;
	float3 vNormal : TEXCOORD;
	float2 vTex : TEXCOORD2;
	float4 vProjPos : TEXCOORD3;
};

VS_OUTPUT_DEFAULT VS_Terrain_Default(VS_INPUT_PNT input)
{
	VS_OUTPUT_DEFAULT output = (VS_OUTPUT_DEFAULT)0;

	output.vPos = mul(float4(input.vPos, 1.f), g_matWVP);
	
	output.vTex = input.vTex;

	return output;
}


float4 PS_Terrain_Default(VS_OUTPUT_DEFAULT input) : SV_TARGET
{
	return (float4)1.f;
}


VS_OUTPUT_TERRAIN_DEFFERED VS_Terrain_Deferred(VS_INPUT_PNT input)
{
	VS_OUTPUT_TERRAIN_DEFFERED output = (VS_OUTPUT_TERRAIN_DEFFERED)0;

	output.vPos = mul(float4(input.vPos, 1.f), g_matWVP);

	float3 vViewNormal = normalize(mul(float4(input.vNormal, 1.f), g_matWV).xyz);

	// NOTE : 우선은 normal을 변경하지 않고 넣는다
	output.vNormal = vViewNormal;
	//output.vNormal = input.vNormal;
	output.vTex = input.vTex;
	output.vProjPos = mul(float4(input.vPos, 1.f), g_matWVP);
	return output;
}

PS_OUTPUT_DEFERRED_GBUFFER PS_Terrain_Deferred(VS_OUTPUT_TERRAIN_DEFFERED input)
{
	PS_OUTPUT_DEFERRED_GBUFFER output = (PS_OUTPUT_DEFERRED_GBUFFER)0;

	output.vAlbedo = g_TerrainDiffuseTex.Sample(samAnisotropic, input.vTex);
	output.vNormal.xyz = input.vNormal * 0.5f + 0.5f;
	output.vNormal.w = input.vProjPos.w;
	output.vDepth = (float4)(input.vProjPos.z / input.vProjPos.w);
	
	return output;
}


technique11 TerrainDefault
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_Terrain_Default()));
		SetPixelShader(CompileShader(ps_5_0, PS_Terrain_Default()));
	}
}

technique11 TerrainDeferred
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_Terrain_Deferred()));
		SetPixelShader(CompileShader(ps_5_0, PS_Terrain_Deferred()));
	}
}
