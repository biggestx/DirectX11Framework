#include "Share.hlsl"

struct VS_INPUT_UI_DEFAULT
{
	float3 vPos : POSITION;
	float2 vTex : TEXCOORD;
};

struct VS_OUTPUT_UI_DEFAULT
{
	float4 vPos : SV_POSITION;
	float2 vTex : TEXCOORD;
};



VS_OUTPUT_UI_DEFAULT UIDefaultVS(VS_INPUT_UI_DEFAULT input)
{
	VS_OUTPUT_UI_DEFAULT output = (VS_OUTPUT_UI_DEFAULT)0;

	output.vPos = mul(float4(input.vPos, 1.f), g_matWVP);
	output.vTex = input.vTex;

	return output;
}

float4 UIDefaultPS(VS_OUTPUT_UI_DEFAULT input) : SV_Target
{
	return g_DiffuseTex.Sample(samAnisotropic, input.vTex);
}


technique11 UIDefault
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, UIDefaultVS()));
		SetPixelShader(CompileShader(ps_5_0, UIDefaultPS()));
		//SetRasterizerState(WireframeRS);
	}
}

