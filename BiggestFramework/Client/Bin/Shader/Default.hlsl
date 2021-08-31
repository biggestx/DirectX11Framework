


float4x4 g_matWorld;
float4x4 g_matView;
float4x4 g_matProj;

float4x4 g_matWVP;

float3 vDirLight = float3(1.0f, 0.0f, 0.0f);


Texture2D g_DiffuseTex;
SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;

	AddressU = WRAP;
	AddressV = WRAP;
};

struct VertexIn
{
	float3 Pos : POSITION;
	float4 Color : COLOR;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float4 Color : COLOR;
	float2 Tex : TEXCOORD;
};

struct PixelInput
{
	float3 mDiffuse : TEXCOORD1;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	vout.PosH = mul(float4(vin.Pos, 1.0f), g_matWVP);
	/*vout.PosH = mul(float4(vin.Pos, 1.f), g_matWorld);
	vout.PosH = mul(vout.PosH, g_matView);
	vout.PosH = mul(vout.PosH, g_matProj);*/
	
	float3 worldNormal = mul(float4(vin.Normal,1.0f), g_matWorld).xyz;

	float fDot = dot(normalize(-vDirLight), worldNormal);
	vout.Color.xyzw = fDot;

	vout.Tex = vin.Tex;

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{


	//return float4(0.f, 0.f, 1.f, 1.f);
	return pin.Color;
}

float4 TestPS(VertexOut pin) : SV_Target
{
	return float4(0.5f, 0.0f, 0.0f,1.0f);
}


struct VertexInOBJ
{
	float3 Pos : POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;
};

struct VertexOutOBJ
{
	float4 PosH : SV_POSITION;
	float4 Color : COLOR;
	float2 Tex : TEXCOORD;
};

VertexOutOBJ VS_OBJ(VertexInOBJ vin)
{
	VertexOutOBJ vout;

	//vout.PosH = mul(float4(vin.Pos,1.f), g_matWVP);
	vout.PosH = mul(float4(vin.Pos,1.f), g_matWorld);
	vout.PosH = mul(vout.PosH, g_matView);
	vout.PosH = mul(vout.PosH, g_matProj);


	float3 worldNormal = mul(float4(vin.Normal, 1.0f), g_matWorld).xyz;
	float fDot = dot(normalize(-vDirLight), vin.Normal);
	vout.Color.xyzw = fDot;

	vout.Tex = vin.Tex;

	return vout;
}

float4 PS_OBJ(VertexOutOBJ pin) : SV_Target
{
	//float4 texColor = g_DiffuseTex.Sample(samAnisotropic, pin.Tex);

	//return pin.Color;
	return float4(0.5f, 0.0f, 0.0f, 1.0f);
}

RasterizerState DefaultRS
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};

RasterizerState WireframeRS
{
	FillMode = Wireframe;
	CullMode = Back;
	FrontCounterClockwise = false;
};


technique11 ColorTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader (CompileShader(ps_5_0, PS()));

		SetRasterizerState(WireframeRS);
	}
}

technique11 ColorTech1
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, TestPS()));

	}
}

technique11 ObjTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_OBJ()));
		SetPixelShader(CompileShader(ps_5_0, PS_OBJ()));
		//SetRasterizerState(WireframeRS);
	}
}
