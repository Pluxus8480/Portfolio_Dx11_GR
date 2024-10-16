
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

// ���ϰ� VS_MAIN(float3	vPosition : POSITION, float2 vTexcoord : TEXCOORD)
/* IA : VertexBuffer(4) + IndexBuffer(6 -> 012023) */
/* SV_ : Shader Value : ���� ���̴����� ó���ؾ��� ��� ������ �������� �����δ� �ϰ� �˾Ƽ� �ؾ��� ������ �����ض�. */

/* VertexShader */
/* 1. ������ �⺻���� ��ȯ(����, ��, ����)�� �����Ѵ�.: ���ý����̽����� ���������� ������ı��� ���ؼ� ó�����ִ� ������ �ʿ��ϴ� . */
/* 2. ������ ������ �����Ѵ�.  */
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

/* Triangle List�� �׸��� �ִ� ���, ���� ������ ���� ���̴��� ����� ������ ������ ������ ��ġ��. */
/* w������ ������ ���ؼ� ������ �ش��ϴ� ������ ��ġ��. �������� ���������̽�(-1, 1 ~ 1, -1) �� �����ϳ�. */
/* ����Ʈ �����̽��� ��ȯ�Ѵ�. �������� ������ ��ǥ����(0, 0 ~ winsizex, winsizey)�� �����Ѵ�.  */
/* ��������� �����ϰԵȴ�. : ���� ������ �����Ͽ� �ȼ��� ������ �����Ѵ�. */
/* ������ �ȼ��� �ȼ� ���̴��� ������ �ȼ��� ���� �����Ѵ�. */

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

/* �ȼ��� ���� �����ϳ�. */
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

	/* �����׽�Ʈ �� �̻� �������������� �����Ѵ�(x) */
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

	/* �����׽�Ʈ �� �̻� �������������� �����Ѵ�(x) */
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
	// Out.vColor = g_Texture.Sample(�������� �����ðǵ�?, ��� ���� �����ðǵ�?);
    Out.vDiffuse = float4(color.xyz, 1.f);
    //g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

	/* �����׽�Ʈ �� �̻� �������������� �����Ѵ�(x) */
    if (Out.vDiffuse.a <= 0.1f)
        discard;

    //Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 1.f);


    return Out;
}

technique11 DefaultTechnique
{
	/* Pass : ���̴� ����� ĸ��ȭ (��������� ��Ƴ���) */

	/* ������ + ������Ʈ + �׸��� + ��Ż�� */
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

