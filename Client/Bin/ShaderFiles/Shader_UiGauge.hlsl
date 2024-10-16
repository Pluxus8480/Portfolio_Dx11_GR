
#include "Engine_Shader_Defines.hlsli"
// vector == float4 float3 float2 float
// matrix == float4x4 

/* ������̺�. == �������� */
float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;
//textureCUBE
float weight = 0.f;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
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
    Out.vTexcoord = In.vTexcoord;

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
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

/* �ȼ��� ���� �����ϳ�. */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

	// Out.vColor = g_Texture.Sample(�������� �����ðǵ�?, ��� ���� �����ðǵ�?);
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    Out.vColor.a = Out.vColor.a / 4.f;
	/* �����׽�Ʈ �� �̻� �������������� �����Ѵ�(x) */
    if (Out.vColor.a <= 0.1f)
        discard;

     // �ؽ�ó ��ǥ �߽��� (0.5, 0.5)�� �̵�
    float2 centeredCoord = In.vTexcoord - float2(0.5, 0.5);
    
    // ���� ��� (����)
    float angle = atan2(centeredCoord.y, centeredCoord.x);

   // ������ 0~2PI���� 0~1�� ������ ��ȯ (12�� ������ 0)
   // ������ 0~2PI���� 0~1�� ������ ��ȯ (30�� ȸ��)
    float normalizedAngle = (angle + 3.14159265358979323846 / 2.0 - 3.14159265358979323846 / 6.0) / (2.0 * 3.14159265358979323846);
    if (normalizedAngle < 0.0)
    {
        normalizedAngle += 1.0; // ������ 0���� 1 ������ ����
    }
    
    // ����ġ�� ���� ���� ���� (0���� 100 ����)
    //float weight = 10.0; // ����: 50
    float angleRange = weight / 100.0;

    // ���� �ȿ� �ִ� ��� ���� ���������� ��ȯ
    if (normalizedAngle > (1.0 - angleRange))
    {
        if (weight >= 83.f)
        {
            Out.vColor = float4(1.f, 1.f, 1.f, 1.f);
        }
        else
        {
            Out.vColor = float4(1.f, 0.62f, 0.f, 1.f);
        }
    }
    
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
        SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}

