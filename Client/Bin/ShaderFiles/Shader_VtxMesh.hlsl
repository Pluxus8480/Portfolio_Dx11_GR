
#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;
texture2D g_NormalTexture;

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
    float4 vNormal : NORMAL; /* �ȼ��� ���� z���� ����� ��ȯ��Ų��  */
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float3 vTangent : TANGENT; /* �ȼ��� ���� x���� ����� ��ȯ��Ų��  */
    float3 vBinormal : BINORMAL; /* �ȼ��� ���� y���� ����� ��ȯ��Ų��  */
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
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix)).xyz;
    Out.vBinormal = cross(Out.vNormal.xyz, Out.vTangent);

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
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
};

/* �ȼ��� ���� �����ϳ�. */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

	// Out.vColor = g_Texture.Sample(�������� �����ðǵ�?, ��� ���� �����ðǵ�?);
    Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal.xyz);

    vNormal = mul(vNormal, WorldMatrix);
	/* �����׽�Ʈ �� �̻� �������������� �����Ѵ�(x) */
    if (Out.vDiffuse.a <= 0.1f)
        discard;

    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.f, 0.f);

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
}

