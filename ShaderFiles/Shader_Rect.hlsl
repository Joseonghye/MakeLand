#include "Shader_RenderState.hpp"

cbuffer Matrices
{
	matrix g_WorldMatrix = (matrix)0;
	matrix g_ViewMatrix;
	matrix g_ProjMatrix;
};

texture2D g_DiffuseTexture;
float g_fPersent;


int g_iImageCountX;
int g_iImageCountY;
int g_iFrame;

sampler DefaultSampler = sampler_state
{
	filter = min_mag_mip_linear;
};

struct VS_IN
{
	float3	vPosition: POSITION;
	float2	vTexUV: TEXCOORD0;
};

struct VS_OUT
{
	float4	vPosition: SV_POSITION;
	float2	vTexUV: TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	VS_OUT Out = (VS_OUT)0;
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP); //아직 w 나누기를 안해서 뷰스페이스상에 존재
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4	vPosition: SV_POSITION;
	float2	vTexUV: TEXCOORD0;
};
struct PS_OUT
{
	vector		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler,In.vTexUV);
	if (Out.vColor.a < 0.1)
		discard;

	return Out;
}

PS_OUT PS_Slider (PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	if (Out.vColor.a < 0.1)
		discard;
	if (g_fPersent < In.vTexUV.x)
		discard;

	return Out;
}

PS_OUT PS_Effect(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	In.vTexUV.x = (In.vTexUV.x / g_iImageCountX) + (g_iFrame%g_iImageCountX)*(1.f / g_iImageCountX);
	In.vTexUV.y = (In.vTexUV.y / g_iImageCountY) + (g_iFrame / g_iImageCountX)*(1.f / g_iImageCountY);

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	if (Out.vColor.a < 0.1)
		discard;

	return Out;
}

technique11	DefaultTechnique
{
	pass Normal
	{
		SetRasterizerState(CullMode_Default);
		SetDepthStencilState(ZDefault, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0  PS_MAIN();
	}
	pass Slider
	{
		SetRasterizerState(CullMode_Default);
		SetDepthStencilState(ZDefault, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0  PS_Slider();
	}

	pass Effect
	{
		SetRasterizerState(CullMode_Default);
		SetDepthStencilState(ZDefault, 0);
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0  PS_Effect();
	}
}