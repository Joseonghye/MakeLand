#include "Shader_RenderState.hpp"

cbuffer Matrices
{
	matrix g_WorldMatrix = (matrix)0;
	matrix g_ViewMatrix;
	matrix g_ProjMatrix;
};

//뼈 행렬 배열
struct BoneMatrixArray
{
	matrix Bone[128];
};
cbuffer BoneMatricesBuffer
{
	BoneMatrixArray g_BoneMatrices;
};

texture2D g_DiffuseTexture;

sampler DefaultSampler = sampler_state
{
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};

struct VS_IN
{
	float3	vPosition : POSITION;
	float3	vNormal : NORMAL;
	float2	vTexUV : TEXCOORD0;
	float3	vTangent : TANGENT;
	uint4	vBlendIndex : BLENDINDEX;
	float4	vBlendWeight: BLENDWEIGHT;
};

struct VS_OUT
{
	float4	vPosition: SV_POSITION;
	float4	vNormal : NORMAL;
	float2	vTexUV : TEXCOORD0;
	float4	vProjPos : TEXCOORD1;
};


VS_OUT VS_MAIN_DEFAULT(VS_IN In)
{
	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	VS_OUT Out = (VS_OUT)0;
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP); 
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;

	return Out;
}

VS_OUT VS_MAIN_ANIM(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix matWV, matWVP;

	//실수 대비 무조건 1이 될 수 있도록 
	float fWeightw = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	matrix	BoneMatrix = g_BoneMatrices.Bone[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_BoneMatrices.Bone[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrices.Bone[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrices.Bone[In.vBlendIndex.w] * In.vBlendWeight.w;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	vector		vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
	vPosition = mul(vPosition, matWVP);

	Out.vPosition = vPosition;

	vector		vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));

	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};
struct PS_OUT
{
	vector		vDiffuse : SV_TARGET0;
	vector		vNormal : SV_TARGET1;
	vector		vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	Out.vDiffuse.a = 1.f;

	/* -1 ~ 1 */
	/* 0 ~ 1 */
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.0f, 0.0f, 0.0f);

	return Out;
}

technique11	DefaultTechnique
{
	pass Default
	{
		SetRasterizerState(CullMode_Default);
		SetDepthStencilState(ZDefault, 0);
		SetBlendState(BlendDisable, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_DEFAULT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0  PS_MAIN();
	}

	pass AnimMesh
	{
		SetRasterizerState(CullMode_Default);
		SetDepthStencilState(ZDefault, 0);
		SetBlendState(BlendDisable, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_ANIM();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0  PS_MAIN();
	}
	
}