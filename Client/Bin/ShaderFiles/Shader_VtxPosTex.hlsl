
#include "Engine_Shader_Defines.hlsli"
// vector == float4 float3 float2 float
// matrix == float4x4 

/* ������̺�. == �������� */
float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;
float g_fRate = 100.f;

/* ���۵� �����̴��� �����ϸ鼭 �������� �ʿ��� �ȼ����� ���̸� �����س��Ҵ�. */
texture2D g_DepthTexture;
bool g_bSkillOn = false;


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

VS_OUT VS_TRAIL_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWVP;
    matWVP = mul(g_ViewMatrix, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}


struct VS_OUT_EFFECT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

VS_OUT_EFFECT VS_MAIN_EFFECT(VS_IN In)
{
    VS_OUT_EFFECT Out = (VS_OUT_EFFECT) 0;

    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;

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
    Out.vColor.a = Out.vColor.a * 0.5f;
	/* �����׽�Ʈ �� �̻� �������������� �����Ѵ�(x) */
    if (Out.vColor.a <= 0.1f)
        discard;

    return Out;
}

PS_OUT PS_AIM(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

	// Out.vColor = g_Texture.Sample(�������� �����ðǵ�?, ��� ���� �����ðǵ�?);
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vColor.a = Out.vColor.a * 0.5f;
	/* �����׽�Ʈ �� �̻� �������������� �����Ѵ�(x) */
    if (Out.vColor.a <= 0.1f)
        discard;

    return Out;
}

PS_OUT PS_UI(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

	// Out.vColor = g_Texture.Sample(�������� �����ðǵ�?, ��� ���� �����ðǵ�?);
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor.a = Out.vColor.a * 0.5f;
	/* �����׽�Ʈ �� �̻� �������������� �����Ѵ�(x) */
    if (Out.vColor.a <= 0.1f)
        discard;

    Out.vColor.r = Out.vColor.r + 0.42f;
    Out.vColor.g = Out.vColor.g + 0.46f;
    Out.vColor.b = Out.vColor.b + 0.52f;
    
    if(g_bSkillOn)
    {
        Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    }
    
    return Out;
}

PS_OUT PS_UI_BACK(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

	// Out.vColor = g_Texture.Sample(�������� �����ðǵ�?, ��� ���� �����ðǵ�?);
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor.a = Out.vColor.a * 0.5f;
	/* �����׽�Ʈ �� �̻� �������������� �����Ѵ�(x) */
    if (Out.vColor.a <= 0.09f)
        discard;

    if (g_bSkillOn)
    {
        Out.vColor = float4(1.f, 0.61f, 0.f, 1.f);
    }
    
    return Out;
}

PS_OUT PS_TRAIL_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    //// �ε巯�� ������ ���� ���
    float redIntensity = smoothstep(0.3, 1.0, In.vTexcoord.y); // 0.3���� 1.0 ���̿��� �ε巴�� ��ȭ
    
    //// ���� ���� ����
    Out.vColor = float4(redIntensity,0.18f, 0.18f, redIntensity); 

    if (In.vTexcoord.y > 0.9f)
        Out.vColor.a = 10.f * (1.f - In.vTexcoord.y);
   
    return Out;
}

PS_OUT PS_WIRE_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    //// �ε巯�� ������ ���� ���
    //float redIntensity = smoothstep(0.3, 1.0, In.vTexcoord.y); // 0.3���� 1.0 ���̿��� �ε巴�� ��ȭ
    
    //// ���� ���� ����
    Out.vColor = float4(0.18f, 0.18f, 0.88f, 1.f);

    //if (In.vTexcoord.y > 0.9f)
    //    Out.vColor.a = 10.f * (1.f - In.vTexcoord.y);
   
    return Out;
}

struct PS_IN_EFFECT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};


PS_OUT PS_MAIN_EFFECT(PS_IN_EFFECT In)
{
    PS_OUT Out = (PS_OUT) 0;

	// Out.vColor = g_Texture.Sample(�������� �����ðǵ�?, ��� ���� �����ðǵ�?);
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
	/* �ΰ��� ����. (�� �ȼ��� ����, �� �ȼ��� �׸����� �ߴ� ��ġ�� �̹� �׷����ִ� �ȼ��� ���� */
	/*In.vProjPos.z => 0 ~ f
	In.vProjPos.w => n ~ f
	In.vProjPos.z / In.vProjPow.w = > 0 ~ 1 ������ ���̸� �������� �����ϴ� ������ ���. */
    float fViewZ = In.vProjPos.w;

    float2 vDepthUV;
    vDepthUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    vDepthUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

    float fOldViewZ = g_DepthTexture.Sample(PointSampler, vDepthUV).y * 3000.f;

    Out.vColor.a = Out.vColor.a * saturate(fOldViewZ - fViewZ);

    return Out;
}

PS_OUT PS_MAIN_GRABOBJ(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

	// Out.vColor = g_Texture.Sample(�������� �����ðǵ�?, ��� ���� �����ðǵ�?);
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
   // Add a blue tint to the texture color.
    float4 blueTint = float4(0.0f, 0.89f, 1.0f, 1.0f);
    
    if (Out.vColor.a <= 0.1f)
        discard;
    
    
    Out.vColor = Out.vColor + blueTint * 0.5f;
       
    if (Out.vColor.r > 0.9 && Out.vColor.g > 0.9 && Out.vColor.b > 0.9)
    {
        // Apply a blue tint to the white parts.
        Out.vColor = lerp(Out.vColor, blueTint, 0.5); // Adjust the intensity as needed
    }
    //Out.vColor.a = Out.vColor.a;
	/* �����׽�Ʈ �� �̻� �������������� �����Ѵ�(x) */

    return Out;
}

PS_OUT PS_BLOOD(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float3 vRed = float3(0.8f, 0.2f, 0.2f);
	// Out.vColor = g_Texture.Sample(�������� �����ðǵ�?, ��� ���� �����ðǵ�?);
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    Out.vColor.a = (Out.vColor.x + Out.vColor.y + Out.vColor.z) / 3.f;
    
    if (Out.vColor.a <= 0.1f)
        discard;
    
    Out.vColor.xyz = lerp(Out.vColor.xyz, vRed, 0.5); // Adjust the intensity as needed
    Out.vColor.a = 0.9f;
    
    return Out;
}

PS_OUT PS_HP(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
	// Out.vColor = g_Texture.Sample(�������� �����ðǵ�?, ��� ���� �����ðǵ�?);
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (Out.vColor.a <= 0.1f)
        discard;
    
   // if (In.vTexcoord.x > 100.f / g_fRate);
    if (In.vTexcoord.x > 0.5f);
        discard;

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
        SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);


        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass SoftEffect
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);


        VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EFFECT();
    }

    pass UiEffect
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

    pass GrabObjEffect
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);


        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_GRABOBJ();
    }

    pass TrailEffect
    {
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);


        VertexShader = compile vs_5_0 VS_TRAIL_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_TRAIL_MAIN();
    }

    pass SkillEffect
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);


        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_UI();
    }

    pass SkillEffectBack
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);


        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_UI_BACK();
    }

    pass Aim
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_AIM();
    }

    pass Blood
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);


        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BLOOD();
    }

    pass Hp
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);


        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_HP();
    }

    pass WireEffect
    {
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);


        VertexShader = compile vs_5_0 VS_TRAIL_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_WIRE_MAIN();
    }

}

