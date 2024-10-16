
#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;
float4 g_vCamPosition;


struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD;
    float3 vTangent : TANGENT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float3 WorldPos : TEXCOORD0;
};

// 리턴값 VS_MAIN(float3	vPosition : POSITION, float2 vTexcoord : TEXCOORD)
/* IA : VertexBuffer(4) + IndexBuffer(6 -> 012023) */
/* SV_ : Shader Value : 내가 셰이더에서 처리해야할 모든 연산을 끝냈으니 앞으로는 니가 알아서 해야할 과정을 수행해라. */

/* VertexShader */
/* 1. 정점의 기본적인 변환(월드, 뷰, 투영)을 수행한다.: 로컬스페이스상의 정점정보를 투영행렬까지 곱해서 처리해주는 과정이 필요하다 . */
/* 2. 정점의 구성을 변경한다.  */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    Out.WorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);

    return Out;
}

/* Triangle List로 그리고 있는 경우, 정점 세개가 정점 쉐이더를 통과할 때까지 모으는 과정을 거치낟. */
/* w나누기 연산을 통해서 투영에 해당하는 과정을 거치낟. 정점들은 투영스페이스(-1, 1 ~ 1, -1) 상에 존재하낟. */
/* 뷰포트 스페이스로 변환한다. 정점들은 윈도우 좌표공간(0, 0 ~ winsizex, winsizey)에 존재한다.  */
/* 래스라이즈를 수행하게된다. : 정점 정보를 보간하여 픽셀의 정보를 생성한다. */
/* 생성된 픽셀을 픽셀 쉐이더로 던져서 픽셀의 색을 결정한다. */

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float3 WorldPos : TEXCOORD0;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
};

/* 픽셀의 색을 결정하낟. */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float3 normal = normalize(In.vNormal);
    float3 viewDir = normalize(g_vCamPosition.xyz - In.WorldPos);
    
    float edgeFactor = dot(normal, viewDir);
    //float edge = smoothstep(0.2, 0.6, 1.0 - abs(edgeFactor));
    float edge = 1.0 - abs(edgeFactor);
    
    float3 color = lerp(float3(1.0, 1.0, 1.0), float3(0.81, 0.16, 0.16), edge);
    
    Out.vDiffuse = float4(color.xyz, 1.f);

	/* 알파테스트 더 이상 렌더스테이츠로 설정한다(x) */
    if (Out.vDiffuse.a <= 0.1f)
        discard;

    //Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 1.f);


    return Out;
}

PS_OUT PS_MAIN_3(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float3 normal = normalize(In.vNormal);
    float3 viewDir = normalize(g_vCamPosition.xyz - In.WorldPos);
    
    float edgeFactor = dot(normal, viewDir);
    //float edge = smoothstep(0.2, 0.6, 1.0 - abs(edgeFactor));
    float edge = 1.0 - abs(edgeFactor);
    
    float3 color = lerp(float3(1.0, 1.0, 1.0), float3(0.16, 0.16, 0.81), edge);
    
    Out.vDiffuse = float4(color.xyz, 1.f);

	/* 알파테스트 더 이상 렌더스테이츠로 설정한다(x) */
    if (Out.vDiffuse.a <= 0.1f)
        discard;

    //Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 1.f);


    return Out;
}

PS_OUT PS_MAIN_2(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float3 normal = normalize(In.vNormal);
    float3 viewDir = normalize(g_vCamPosition.xyz - In.WorldPos);
    
    float edgeFactor = dot(normal, viewDir);
    //float edge = smoothstep(0.2, 0.6, 1.0 - abs(edgeFactor));
    float edge = 1.0 - abs(edgeFactor);
    
    float3 color = lerp(float3(1.0, 1.0, 1.0), float3(0.f, 0.f, 0.96f), edge * 1.2);
	// Out.vColor = g_Texture.Sample(어떤방식으로 가져올건데?, 어디에 색을 가져올건데?);
    Out.vDiffuse = float4(color.xyz, 1.f);
    //g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

	/* 알파테스트 더 이상 렌더스테이츠로 설정한다(x) */
    if (Out.vDiffuse.a <= 0.1f)
        discard;

    //Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 1.f);


    return Out;
}

technique11 DefaultTechnique
{
	/* Pass : 셰이더 기능의 캡슐화 (여러기법을 모아놓은) */

	/* 빛연산 + 림라이트 + 그림자 + 메탈릭 */
    pass Default
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Default2
    {
         SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_2();
    }

    pass Default3
    {
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_3();
    }
}

