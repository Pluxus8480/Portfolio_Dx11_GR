
#include "Engine_Shader_Defines.hlsli"
// vector == float4 float3 float2 float
// matrix == float4x4 

/* 상수테이블. == 전역변수 */
float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;
float g_fRate = 100.f;

/* 디퍼드 세ㅜ이더르 수행하면서 빛연산이 필요한 픽셀들의 깊이를 저장해놓았다. */
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

/* Triangle List로 그리고 있는 경우, 정점 세개가 정점 쉐이더를 통과할 때까지 모으는 과정을 거치낟. */
/* w나누기 연산을 통해서 투영에 해당하는 과정을 거치낟. 정점들은 투영스페이스(-1, 1 ~ 1, -1) 상에 존재하낟. */
/* 뷰포트 스페이스로 변환한다. 정점들은 윈도우 좌표공간(0, 0 ~ winsizex, winsizey)에 존재한다.  */
/* 래스라이즈를 수행하게된다. : 정점 정보를 보간하여 픽셀의 정보를 생성한다. */
/* 생성된 픽셀을 픽셀 쉐이더로 던져서 픽셀의 색을 결정한다. */

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;

};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

/* 픽셀의 색을 결정하낟. */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

	// Out.vColor = g_Texture.Sample(어떤방식으로 가져올건데?, 어디에 색을 가져올건데?);
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor.a = Out.vColor.a * 0.5f;
	/* 알파테스트 더 이상 렌더스테이츠로 설정한다(x) */
    if (Out.vColor.a <= 0.1f)
        discard;

    return Out;
}

PS_OUT PS_AIM(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

	// Out.vColor = g_Texture.Sample(어떤방식으로 가져올건데?, 어디에 색을 가져올건데?);
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vColor.a = Out.vColor.a * 0.5f;
	/* 알파테스트 더 이상 렌더스테이츠로 설정한다(x) */
    if (Out.vColor.a <= 0.1f)
        discard;

    return Out;
}

PS_OUT PS_UI(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

	// Out.vColor = g_Texture.Sample(어떤방식으로 가져올건데?, 어디에 색을 가져올건데?);
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor.a = Out.vColor.a * 0.5f;
	/* 알파테스트 더 이상 렌더스테이츠로 설정한다(x) */
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

	// Out.vColor = g_Texture.Sample(어떤방식으로 가져올건데?, 어디에 색을 가져올건데?);
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor.a = Out.vColor.a * 0.5f;
	/* 알파테스트 더 이상 렌더스테이츠로 설정한다(x) */
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

    //// 부드러운 붉은색 강도 계산
    float redIntensity = smoothstep(0.3, 1.0, In.vTexcoord.y); // 0.3에서 1.0 사이에서 부드럽게 변화
    
    //// 최종 색상 결정
    Out.vColor = float4(redIntensity,0.18f, 0.18f, redIntensity); 

    if (In.vTexcoord.y > 0.9f)
        Out.vColor.a = 10.f * (1.f - In.vTexcoord.y);
   
    return Out;
}

PS_OUT PS_WIRE_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    //// 부드러운 붉은색 강도 계산
    //float redIntensity = smoothstep(0.3, 1.0, In.vTexcoord.y); // 0.3에서 1.0 사이에서 부드럽게 변화
    
    //// 최종 색상 결정
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

	// Out.vColor = g_Texture.Sample(어떤방식으로 가져올건데?, 어디에 색을 가져올건데?);
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
	/* 두개의 깊이. (내 픽셀의 깊이, 이 픽셀을 그릴려고 했던 위치에 이미 그려져있던 픽셀의 깊이 */
	/*In.vProjPos.z => 0 ~ f
	In.vProjPos.w => n ~ f
	In.vProjPos.z / In.vProjPow.w = > 0 ~ 1 보간된 깊이를 선형으로 저장하는 연산의 방법. */
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

	// Out.vColor = g_Texture.Sample(어떤방식으로 가져올건데?, 어디에 색을 가져올건데?);
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
	/* 알파테스트 더 이상 렌더스테이츠로 설정한다(x) */

    return Out;
}

PS_OUT PS_BLOOD(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float3 vRed = float3(0.8f, 0.2f, 0.2f);
	// Out.vColor = g_Texture.Sample(어떤방식으로 가져올건데?, 어디에 색을 가져올건데?);
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
    
	// Out.vColor = g_Texture.Sample(어떤방식으로 가져올건데?, 어디에 색을 가져올건데?);
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
	/* Pass : 셰이더 기능의 캡슐화 (여러기법을 모아놓은) */

	/* 빛연산 + 림라이트 + 그림자 + 메탈릭 */
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

