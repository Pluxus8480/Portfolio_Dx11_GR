
#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4x4 g_ViewMatrixInv, g_ProjMatrixInv;
float4x4 g_TrueViewMatrix, g_TrueProjMatrix;

vector g_vLightDir;
vector g_vLightPos;
float g_fLightRange;
float g_fSpotInnerConeAngle;
float g_fSpotOuterConeAngle;
float g_fLightFalloff;
float g_fScatteringDensity = 0.2; // �� ��� �е�
float g_fScatteringWeight = 0.5; // �� ��� ����ġ
float2 g_ScreenSize = float2(1280,720);
float g_fBlackScreenRate = 0.0f;

bool g_bDoubleBuffered = false;
bool g_bBlackScreen = false;

vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

texture2D g_NormalTexture;
texture2D g_GlowTexture;
texture2D g_GlowOriginalTexture;
texture2D g_VerticalTexture;
/* �������� �ʿ��� ��� �ȼ����� ��ǻ�� ������ �ȼ����� �ٸ��� ���ڴ�. */
texture2D g_DiffuseTexture;

/* �������� �ʿ��� ��� �ȼ����� �ں��Ʈ ������ ��δ� �� ������ �� ĥ�� */
//texture2D		g_AmbientTexture;
vector g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

texture2D g_ShadeTexture;
texture2D g_DepthTexture;
texture2D g_SpecularTexture;
texture2D g_DeferredTexture;
texture2D g_Texture;

texture2D g_DeathTexture;

vector g_vCamPosition;

bool g_bUseRedGlitch = false;

// Glitch effect parameters
float shake_power = 0.03f;
float shake_rate = 0.1f; // Reduced shake rate to lower frequency of glitches
float shake_speed = 5.0f;
float shake_block_size = 30.5f;
float shake_color_rate = 0.01f;
float red_mix_rate = 0.5f; // Red color mix rate
float red_block_min_length = 300.0f; // Minimum length of the red glitch block in pixels
float red_block_max_length = 800.0f; // Maximum length of the red glitch block in pixels
float red_block_height = 15.0f; // Height of the red glitch block in pixels
float TIME : register(c0); // Time should be passed to the shader

float2 g_Center = float2(0.5f, 0.5f); // ȭ�� �߽� (0.5, 0.5)
float g_BlurStrength = 5.0f; // �� ����

bool g_bUseBlueScreen = false;
bool g_bUseRedScreen = false;
float g_fBlueScreenRate = 0.f;
int numSamples = 8; // �ʱ� ���� ����

static const float weights[15] =
{
    0.079665, 0.102204, 0.120285, 0.132762, 0.138776,
    0.138776, 0.132762, 0.120285, 0.102204, 0.079665,
    0.059208, 0.041426, 0.027119, 0.016270, 0.008864
};

float3 SobelX[3] = { float3(-1, 0, 1), float3(-2, 0, 2), float3(-1, 0, 1) };
float3 SobelY[3] = { float3(-1, -2, -1), float3(0, 0, 0), float3(1, 2, 1) };

float random(float2 seed)
{
    
    return frac(543.2543 * sin(dot(seed, float2(3525.46, -54.3415))));
}

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};



struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

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
PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    return Out;
}

struct PS_OUT_LIGHT
{
    float4 vShade : SV_TARGET0;
    float4 vSpecular : SV_TARGET1;
};

/* ���̵� Ÿ�ٿ� �簢�� ���۸� �׸��� = ������ ���� ��� �������� ����ش�. 48������ �ȼ��� �� �����Ӵ� ���� ������ŭ ������. */
PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir) * -1.f, normalize(vNormal)), 0.f) + (g_vLightAmbient * g_vMtrlAmbient));
	
    vector vReflect = reflect(normalize(g_vLightDir), normalize(vNormal));

	/* �������� �ʿ��� �ȼ��� ������ġ�� �ʿ��ϴ�. */
	/* ������ġ�� ������ ���! */
	/* 1. �������� �ʿ��� ��ü���� �׸� ��, �ϳ��� ����Ÿ���� �� �߰��Ͽ� �ȼ����� ������ġ�� ����޴´�. ������ġ�� �����ϱ⶧���� ����Ÿ���ȼ��� �����ϱ⿡ �������� �ʴ�. */
	/* 2. ������ġ���� �ȼ��� ������ġ�� �߷��غ���. */

    vector vWorldPos;

	/* ������ġ * ������� * ����� * ������� * 1/w == ���������̽� ���� ��ġ�� ���ϳ�.*/
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

	/* ������ġ * ������� * ����� * ������� */
    vWorldPos = vWorldPos * (vDepthDesc.y * 3000.f);

	/* ������ġ * ������� * ����� */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* ������ġ * ������� */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    vector vLook = vWorldPos - g_vCamPosition;

    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f);

    return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	/* �������� �ʿ��� �ȼ��� ������ġ�� �ʿ��ϴ�. */
	/* ������ġ�� ������ ���! */
	/* 1. �������� �ʿ��� ��ü���� �׸� ��, �ϳ��� ����Ÿ���� �� �߰��Ͽ� �ȼ����� ������ġ�� ����޴´�. ������ġ�� �����ϱ⶧���� ����Ÿ���ȼ��� �����ϱ⿡ �������� �ʴ�. */
	/* 2. ������ġ���� �ȼ��� ������ġ�� �߷��غ���. */

    vector vWorldPos;

	/* ������ġ * ������� * ����� * ������� * 1/w == ���������̽� ���� ��ġ�� ���ϳ�.*/
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

	/* ������ġ * ������� * ����� * ������� */
    vWorldPos = vWorldPos * (vDepthDesc.y * 3000.f);

	/* ������ġ * ������� * ����� */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* ������ġ * ������� */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    vector vLightDir = vWorldPos - g_vLightPos;

    float fDistance = length(vLightDir);

	/* �ȼ��� ������ġ�� ������ ��ġ�� ������ 1�� ������. ������ �������� �־����� 0����. */
    float fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);

    Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(vLightDir) * -1.f, normalize(vNormal)), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;

    vector vReflect = reflect(normalize(vLightDir), normalize(vNormal));
	
    vector vLook = vWorldPos - g_vCamPosition;

    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f) * fAtt;

    return Out;
}

PS_OUT_LIGHT PS_MAIN_SPOT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT)0;

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    // �ȼ��� ���� ��ġ ���
    vector vWorldPos;
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

    vWorldPos = vWorldPos * (vDepthDesc.y * 3000.f);
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    vector vLightDir = g_vLightPos - vWorldPos;
    float fDistance = length(vLightDir);
    vLightDir = normalize(vLightDir);

    // ����Ʈ����Ʈ ���� ���
    float fTheta = dot(vLightDir, normalize(-g_vLightDir)); // �� ����� ���� ������ ����
    float fInnerCone = cos(g_fSpotInnerConeAngle);
    float fOuterCone = cos(g_fSpotOuterConeAngle);
    float fSpotAtten = saturate((fTheta - fOuterCone) / (fInnerCone - fOuterCone));

    // �Ÿ� ���� ���
    float fDistanceAtten = saturate((g_fLightRange - fDistance) / g_fLightRange);

    // ���� ���� ��
    float fAtten = fSpotAtten * fDistanceAtten;

    // ��ǻ�� �� �ں��Ʈ ���� ���
    Out.vShade = g_vLightDiffuse * saturate(max(dot(-vLightDir, normalize(vNormal)), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtten;

    // ����ŧ�� ���� ���
    vector vReflect = reflect(vLightDir, normalize(vNormal));
    vector vLook = normalize(g_vCamPosition - vWorldPos);
    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(vLook, vReflect), 0.f), 30.f) * fAtten;

    return Out;
}

/* ���������� ����ۿ� �������� ����ֱ����� ���̴�. 48������ �ȼ��� �� �����Ӵ� �ѹ� ������. */
PS_OUT PS_MAIN_DEFERRED(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (0.0f == vDiffuse.a)
        discard;

    vector vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexcoord);

    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);

    Out.vColor = vDiffuse * vShade + vSpecular;
    
    return Out;
}

PS_OUT PS_MAIN_GLITCH(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    // Determine if the glitch effect should be applied
    float enable_shift = (random(float2(floor(TIME * shake_speed), 0.0)) < shake_rate) ? 1.0f : 0.0f;
    
    // Calculate the adjusted UV coordinates with the glitch effect
    float2 fixed_uv = In.vTexcoord;
    fixed_uv.x += (random(float2(floor(In.vTexcoord.y * shake_block_size) / shake_block_size, TIME)) - 0.5f) * shake_power * enable_shift;

    // Sample the texture at the adjusted UV coordinates
    float4 pixel_color = g_DeferredTexture.SampleLevel(LinearSampler, fixed_uv, 0.0f);
    float4 Death_color = g_DeathTexture.Sample(LinearSampler, In.vTexcoord);
    
    // Apply color separation effect
    pixel_color.r = lerp(pixel_color.r, g_DeferredTexture.SampleLevel(LinearSampler, fixed_uv + float2(shake_color_rate, 0.0f), 0.0f).r, enable_shift);
    pixel_color.b = lerp(pixel_color.b, g_DeferredTexture.SampleLevel(LinearSampler, fixed_uv + float2(-shake_color_rate, 0.0f), 0.0f).b, enable_shift);

    if (g_bUseRedGlitch == true)
    {
    
        for (int i = 0; i < 36; ++i)
        {
        // Generate random center for the rectangle within the available screen height
            float2 center = float2(random(float2(TIME, i * 0.1f)) * g_ScreenSize.x, i * red_block_height * 1.5f * g_ScreenSize.y / 720.f);
        
        // Generate random width between min and max lengths
            float rect_width = red_block_min_length + (red_block_max_length - red_block_min_length) * random(float2(TIME, i * 0.3f));
            float rect_height = red_block_height;
        
        // Convert dimensions to texture coordinates
            float2 rect_half_size = float2(rect_width / g_ScreenSize.x, rect_height / g_ScreenSize.y) * 0.5f;
        
        // Calculate rectangle bounds
            float2 rect_min = center / float2(g_ScreenSize.x, g_ScreenSize.y) - rect_half_size;
            float2 rect_max = center / float2(g_ScreenSize.x, g_ScreenSize.y) + rect_half_size;

        // Check if the current UV coordinate is within the rectangle
            if (In.vTexcoord.x >= rect_min.x && In.vTexcoord.x <= rect_max.x &&
            In.vTexcoord.y >= rect_min.y && In.vTexcoord.y <= rect_max.y)
            {
                pixel_color.rgb = lerp(pixel_color.rgb, float3(1.0f, 0.0f, 0.0f), red_mix_rate);
            }
        }
    }
    
    if (g_bBlackScreen == true)
    {
        pixel_color.rgb = lerp(pixel_color.rgb, float3(0.0f, 0.0f, 0.0f), g_fBlackScreenRate);
    }
    
    if(g_fBlackScreenRate > 0.9f)
        pixel_color = Death_color;
    
    Out.vColor = pixel_color;

    return Out;
}

// �ȼ� ���̴�: ����� ��
PS_OUT PS_MAIN_BLUR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    // �Է� �ȼ� ����
    float4 pixel_color = g_DeferredTexture.Sample(LinearSampler, In.vTexcoord);

    // ȭ�� �߽����κ����� �Ÿ� ���
    float2 direction = In.vTexcoord - g_Center;
    float distance = length(direction);

    // ���� �� ���� (�� ������ ����)
    int adjustedNumSamples = max(8, (int) (numSamples * g_BlurStrength));
    float totalWeight = 0.0f;
    float4 blurredColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

    // ����� �� ���ø�
    for (int i = 0; i < adjustedNumSamples; ++i)
    {
        // �� ������ �Ÿ� ��� ����ġ ���
        float sampleOffset = (float(i) / float(adjustedNumSamples)) * distance;
        float2 sampleCoord = In.vTexcoord + normalize(direction) * sampleOffset;

        // ���ø��� �ؽ�ó ����
        float4 sampleColor = g_DeferredTexture.Sample(LinearSampler, sampleCoord);

        // ����ġ�� ����� ���� ȥ��
        float weight = 1.0f - (sampleOffset / distance);
        blurredColor += sampleColor * weight;
        totalWeight += weight;
    }

    // ���� �� ���� ���
    blurredColor /= totalWeight;

    // ���� ����� �� ������ ȥ���Ͽ� ���
    Out.vColor = lerp(pixel_color, blurredColor, distance);
    
    float4 targetColor = float4(0.29f, 0.29f, 0.95f, 1.0f);
    
    if (g_bUseBlueScreen == true)
        targetColor = float4(0.29f, 0.29f, 0.95f, 1.0f);

    if (g_bUseRedScreen == true)
        targetColor = float4(0.95f, 0.29f, 0.29f, 1.0f);
    
    if (g_fBlueScreenRate > 0.f)
        Out.vColor = lerp(Out.vColor, targetColor, g_fBlueScreenRate);
    

    return Out;
}

PS_OUT PS_MAIN_GLOW(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 glow_color = g_GlowTexture.Sample(LinearSampler, In.vTexcoord);
    
    float2 texelSize = 1.0 / g_ScreenSize * 2.f;
    
    float4 Horizon_color = g_GlowTexture.Sample(LinearSampler, In.vTexcoord) * weights[0];

    for (int i = 1; i < 7; ++i)
    {
        Horizon_color += g_GlowTexture.Sample(LinearSampler, In.vTexcoord + float2(i * texelSize.x, 0.0)) * weights[i];
        Horizon_color += g_GlowTexture.Sample(LinearSampler, In.vTexcoord - float2(i * texelSize.x, 0.0)) * weights[i];
    }
    
    Out.vColor = Horizon_color;
    
    return Out;
}

PS_OUT PS_MAIN_GLOWFRONT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    //�Է� �ȼ� ����
    float4 pixel_color = g_DeferredTexture.Sample(LinearSampler, In.vTexcoord);
    float4 glow_color = g_GlowTexture.Sample(LinearSampler, In.vTexcoord);
    float4 glow_original_color = g_GlowOriginalTexture.Sample(LinearSampler, In.vTexcoord);
    
    float2 texelSize = 1.0 / g_ScreenSize;

    float4 Vertical_color = g_GlowTexture.Sample(LinearSampler, In.vTexcoord) * weights[0];
    
    for (int j = 1; j < 15; ++j)
    {
        Vertical_color += g_GlowTexture.Sample(LinearSampler, In.vTexcoord + float2(0.0, j * texelSize.y)) * weights[j];
        Vertical_color += g_GlowTexture.Sample(LinearSampler, In.vTexcoord - float2(0.0, j * texelSize.y)) * weights[j];
    }
    
    Out.vColor = pixel_color + (Vertical_color * 0.5f) + glow_original_color;
    
    return Out;
}




PS_OUT PS_MAIN_FINAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_DeferredTexture.Sample(LinearSampler, In.vTexcoord);

    return Out;
}

technique11 DefaultTechnique
{
    pass RenderTarget_Debug
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass Light_Directional
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Sky, 0);
        SetBlendState(BS_LightBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }

    pass Light_Point
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Sky, 0);
        SetBlendState(BS_LightBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POINT();
    }

    pass Deferred
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Sky, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEFERRED();
    }

    pass Light_Spot
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Sky, 0);
        SetBlendState(BS_LightBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SPOT();

    }
    
    pass Deferred_Final
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Sky, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FINAL();
    }

    pass Deferred_Glitch
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Sky, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_GLITCH();
    }

    pass Deferred_Blur
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Sky, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR();
    }

    pass Deferred_Glow
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Sky, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_GLOW();
    }

    pass Deferred_GlowFront
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Sky, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_GLOWFRONT();
    }
}

