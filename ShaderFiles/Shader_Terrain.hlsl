#include "Shader_RenderState.hpp"

//cbuffer : 상수 메모리 영역 / 모든 전역변수들을 '상수 테이블'이라 부름
// > 상수? 쉐이더 전역변수는 상수화 되어있다.(이용O,변경X) > 클라이언트로 부터 값을 받아옴
//전역변수 이름이 같으면 공간을 공유함
//전달 되는 타이밍이 같은 애들끼리 모아 두는 것이 좋음 

cbuffer Matrices
{
	matrix g_WorldMatrix;
	matrix g_ViewMatrix;
	matrix g_ProjMatrix;
};

cbuffer CamDesc
{
	vector g_vCamPosition;
};

cbuffer BrushDesc
{
	vector g_vBrushPosition = vector(20.f, 0.f, 10.f, 1.f);
	float g_fRadius = 10.f;
};

cbuffer LightDesc
{
	vector g_vLightDirection = vector(1.f, -1.f, 1.f,0.f);
	vector g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f);
	vector g_vLightAmbient = vector(1.f, 1.f, 1.f, 1.f);
	vector g_vLightSpecular = vector(1.f, 1.f, 1.f, 1.f);
};

cbuffer MtrlDesc
{
	vector g_vMtrlDiffuse = vector(1.f, 1.f, 1.f, 1.f);
	vector g_vMtrlAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);
	vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);
};

texture2D g_DiffuseSourceTexture;
texture2D g_DiffuseDestTexture;
texture2D g_FilterTexture;
texture2D g_BrushTexture;

//지형 스플릿

sampler DefaultSampler = sampler_state
{
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};

/*
1. m_pDeviceContext->DrawIndexed()
2. 인덱스가 가지고 있던 정점 세개를 정점버퍼로부터 얻어온다.
3. VS_MAIN함수를 호출하면서 가져온 정점 세개중 하나씩 전달해준다.
*/

struct VS_IN
{
	float3	vPosition: POSITION;
	float3	vNormal: NORMAL;
	float2	vTexUV: TEXCOORD0;
};

struct VS_OUT
{
	float4	vPosition : SV_POSITION;
	float2	vTexUV : TEXCOORD0;
	float	fShade : TEXCOORD1; //명암
	float	fSpecular : TEXCOORD2;
	float4	vWorldPos : TEXCOORD3;
};

VS_OUT VS_MAIN(VS_IN In)
{
	//VS_IN :정점 버퍼에 정의되어있던 정점정보를 그대로 받아온 것 (로컬)
	
	//1. 정점 스페이스 변환 
	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	//2. 픽셀에 전달할 데이터 결정
	VS_OUT Out = (VS_OUT)0;
	
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);//아직 w 나누기를 안해서 뷰스페이스상에 존재
	Out.vTexUV = In.vTexUV;

	vector WorldNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix);
	vector WorldPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

	//내적
	Out.fShade = saturate(dot(normalize(g_vLightDirection)* -1.f, normalize(WorldNormal)));
	// == Out.fShade = max(dot(normalize(g_vLightDirection)* -1.f, normalize(WorldNormal)), 0.f);
	// 앞의 수 혹은 0보다 작을 경우 0

	vector vReflect = reflect(normalize(g_vLightDirection), normalize(WorldNormal));
	vector vLook = WorldPosition - g_vCamPosition;
	Out.fSpecular = pow(saturate(dot(normalize(vReflect)*-1.f, normalize(vLook))),30.f);

	//이렇게 추가하면 월드 좌표 전달 가능 
	//Out.vWorldPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

	Out.vWorldPos = WorldPosition;

	return Out;
}
/* 버텍스 쉐이더가 끝난 후 ,
 SV_POSITION에 대해서만 원근 투영 (w 값으로 xyz를 나눔 > SV_POSITION 무조건 flaot4여야함)
 (2차원 투영 스페이스로 이동)
 뷰포트로 변환 (윈도우상 좌표로 이동)
 레스터라이즈 (둘러쌓여진 정점 정보를 바탕으로 픽셀 정보 생성)
*/

struct PS_IN
{
	//SV_POSITION에 대해서만 원근 투영. (w 값으로 xyz를 나눔 > SV_POSITION 무조건 flaot4여야함.)
	float4	vPosition: SV_POSITION;
	float2	vTexUV: TEXCOORD0;
	float	fShade : TEXCOORD1; //명암
	float	fSpecular: TEXCOORD2; 
	float4	vWorldPos : TEXCOORD3;

	//월드 위치를 가지고 싶은 경우 (시멘틱을 POSITION으로하면 스스로 계산하므로 안됨)
	//VS_OUT과 동일 해야하므로 위에도 추가해야함
	//float4  vWorldPosition:TEXCOORD1; << 월드 위치도 정점들을 선형보간해서 들어오게됨 
};
struct PS_OUT
{
	vector		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	/*
	if(In.vWorldPosition.y < 5.f)
	  vColor = BULE;
	> 이런식으로 월드좌표에서 5아래는 파란색 설정가능
	*/

	PS_OUT		Out = (PS_OUT)0;

	vector vSourDiffuse = g_DiffuseSourceTexture.Sample(DefaultSampler,In.vTexUV * 10.f);
	vector vDestDiffuse = g_DiffuseDestTexture.Sample(DefaultSampler, In.vTexUV * 20.f);
	vector vFilterColor = g_FilterTexture.Sample(DefaultSampler, In.vTexUV);
	
	//vector vBrushColor = g_BrushTexture.Sample(DefaultSampler, In.vTexUV);
	vector vBrushColor = (vector)0;

	if (g_vBrushPosition.x - g_fRadius <= In.vWorldPos.x && In.vWorldPos.x < g_vBrushPosition.x + g_fRadius &&
		g_vBrushPosition.z - g_fRadius <= In.vWorldPos.z && In.vWorldPos.z < g_vBrushPosition.z + g_fRadius)
	{
		float2		vTexUV = float2((In.vWorldPos.x - (g_vBrushPosition.x - g_fRadius)) / (2.f * g_fRadius),
			((g_vBrushPosition.z + g_fRadius) - In.vWorldPos.z) / (2.f * g_fRadius));
		vBrushColor = g_BrushTexture.Sample(DefaultSampler, vTexUV);
	}

	vector	vDiffuse = vSourDiffuse * vFilterColor.r + vDestDiffuse * (1.f - vFilterColor.r) + vBrushColor;


	Out.vColor = (g_vLightDiffuse * vDiffuse) * saturate(In.fShade + (g_vLightAmbient * g_vMtrlAmbient))
		+ ((g_vLightSpecular * g_vMtrlSpecular) *In.fSpecular);

	return Out;
}

technique11	DefaultTechnique
{
	pass Normal
	{
		SetRasterizerState(CullMode_Default);
		SetDepthStencilState(ZDefault, 0);
		SetBlendState(BlendDisable, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0  PS_MAIN();
	}
}