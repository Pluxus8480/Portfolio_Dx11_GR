#include "..\Public\Renderer.h"
#include "GameObject.h"

#include "GameInstance.h"

#include "VIBuffer_Rect.h"
#include "Shader.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }

{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderer::Initialize()
{
	/* ���۵� ���̴��� ���� ����Ÿ�ٵ��� �߰��ϰڴ�. */

	/* ������ �� ���ۿ� �׷�������� ���� �״�� �ٸ� ����Ÿ�ٿ� �׸��� �۾��� �ϹǷ� */
	/* �����ϴ� ����Ÿ�ٵ��� ũ�⸦ ����ۿ� ���߰ڴ�. */
	D3D11_VIEWPORT			ViewportDesc{};
	_uint					iNumViewports = { 1 };
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	m_pDeathTexture = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GhostRunner/Texture/Death/DeadScene.png"));
	
	if(nullptr == m_pDeathTexture)
		return E_FAIL;

	/* For.Target_Diffuse */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* For.Target_Normal */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* For.Target_Depth */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* For.Target_Shade */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* For.Target_Specular */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* For.Target_ORM */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Orm"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 0.f, 1.f, 1.f))))
		return E_FAIL;

	/* For.Target_Glow */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_GlowBack"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* For.Target_Glow */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_GlowFront"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;


	/* For.Target_Deferred */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_DeferredFront"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_DeferredBack"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;


	/* For.MRT_GameObjects ( Diffuse + Normal + Depth ) */
	/* �� �׷쿡 �߰��� ����Ÿ�ٵ��� ��ü�κ��� �޾ƿ;��� ������ �Ǹ��Ѥ�.�� */
	/* ��ü���� �׸��� ������ ���ε� �Ǿ���� Ÿ�ٵ��̴�. */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Orm"))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GlowBack"), TEXT("Target_GlowBack"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GlowFront"), TEXT("Target_GlowFront"))))
		return E_FAIL;

	/* For.MRT_LightAcc ( Shade + Specular )*/
	/* �� �׷쿡 �߰��� ����Ÿ�ٵ��� ����κ��� �޾ƿ;��� ������ �Ǹ��Ѥ�.�� */
	/* �������� �ϱ� ������ ���ε� �Ǿ���� Ÿ�ٵ��̴�. */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
		return E_FAIL;



	/* ��ó�� ���۵���̿��� */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_DeferredBack"), TEXT("Target_DeferredBack"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_DeferredFront"), TEXT("Target_DeferredFront"))))
		return E_FAIL;

	/* ȭ���� �� ä���� �����������ֱ����� ������� */
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	m_WorldMatrix._11 = ViewportDesc.Width;
	m_WorldMatrix._22 = ViewportDesc.Height;

	/*m_WorldMatrix._41 = ViewportDesc.Width * 0.5f - ViewportDesc.Width * 0.5f;
	m_WorldMatrix._42 = -ViewportDesc.Height * 0.5f + ViewportDesc.Height * 0.5f;*/

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;



#ifdef _DEBUG

	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Diffuse"), 340.f, 100.f, 120.f, 120.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Normal"), 490.f, 100.f, 120.f, 120.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Depth"), 640.f, 100.f, 120.f, 120.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_DeferredBack"), 790.f, 100.0f, 120.f, 120.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_DeferredFront"), 940.f, 100.0f, 120.f, 120.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_GlowBack"), 340.f, 250.f, 120.f, 120.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_GlowFront"), 490.f, 250.f, 120.f, 120.f)))
		return E_FAIL;

#endif

	return S_OK;
}

HRESULT CRenderer::Add_RenderObjects(RENDERGROUP eRenderGroup, CGameObject* pRenderObject)
{
	if (eRenderGroup >= RENDER_END)
		return E_FAIL;

	m_RenderObjects[eRenderGroup].push_back(pRenderObject);

	Safe_AddRef(pRenderObject);

	return S_OK;
}

HRESULT CRenderer::Draw()
{
	if (FAILED(m_pShader->Bind_SRV("g_DiffuseTexture", nullptr)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_SRV("g_NormalTexture", nullptr)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_SRV("g_ShadeTexture", nullptr)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_SRV("g_DepthTexture", nullptr)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_SRV("g_SpecularTexture", nullptr)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_SRV("g_DeferredTexture", nullptr)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_SRV("g_GlowTexture", nullptr)))
		return E_FAIL;

	m_pShader->Begin(3);

	/* ����ۿ� ��ī�̹ڽ��� �׸���.  */
	if (FAILED(Render_Priority()))
		return E_FAIL;

	/* ���� + �����̾� + ���� �� ������ �ʿ��� ��ü���̴�. */
	/* �ٸ� Ÿ�ٿ� �����꿡 �ʿ��� �����͵��� ������(����)���ֱ����� (�������� �Ҷ� �ȼ��� ��ǻ�� ���� + �ȼ��� �븻����) */
	/* ����۸� ��� ���ΰ� �����͵��� ����ޱ����� ����Ÿ������ ��ü�ϴ� �۾��� �ϰڴ�. */
	/* ��������δ� ��ǻ��, �븻Ÿ�ٿ� �������� �ʿ��� ��ü���� �ȼ�(��ǻ������, �븻������ ������ �����ߴ�.) */
	if (FAILED(Render_NonBlend()))
		return E_FAIL;

	/* �������� ����� ShadeŸ�ٿ� ����޴´�. */
	/* ShadeŸ���� ��ġ�� ���ε��۾��� �ʿ��ϴ�.*/
	/* ������ �׸��� == �簢�� ���۸� ������������ �׸���. shadeŸ���� ��ä���� �׸��� �簡���� �׸��ڴ�. */
	/* �簢���� �׸��� �� ������ ��ǻ��� �븻�� �̿��ؼ� �������� ����� ShadeŸ�ٿ� �׷��ٲ���. */
	if (FAILED(Render_LightAcc()))
		return E_FAIL;

	/* Diffuse Shade �ΰ��� Ÿ���� ���ؼ� ����ۿ� �׷��ش�. */
	/* �簢���� ������������ �׸���. �׸����ؼ��� ���̴��� �ʿ���., */
	/* �� ���̴��� Diffuse SRV, Shade SRV�ΰ��� ������ ���ؼ� ��´�. */
	if (FAILED(Render_Deferred()))
		return E_FAIL;

	if (FAILED(Render_Glow()))
		return E_FAIL;

	if (FAILED(Final_Deferred()))
		return E_FAIL;

	if (FAILED(Render_NonLight()))
		return E_FAIL;

	if (FAILED(Render_Blend()))
		return E_FAIL;

	if (FAILED(Render_UI()))
		return E_FAIL;

	if(GAMEINSTANCE->GetKeyDown(eKeyCode::O))
		m_bDebug = !m_bDebug;


#ifdef _DEBUG
	
	if (m_bDebug) {
		if (FAILED(Render_Debug()))
			return E_FAIL;
	}
#endif

	return S_OK;
}

void CRenderer::Clear()
{
	for (auto& RenderObjects : m_RenderObjects)
	{
		for (auto& pRenderObject : RenderObjects)
			Safe_Release(pRenderObject);
		RenderObjects.clear();
	}
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_PRIORITY])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	/* Diffuse + Normal + Depth + Hit Ÿ���� ���ε��ϳ�. */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_NONBLEND])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RENDER_NONBLEND].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_NONLIGHT])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RENDER_NONLIGHT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{

	for (auto& pRenderObject : m_RenderObjects[RENDER_BLEND])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RENDER_BLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Glow()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GlowBack"))))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_GLOW])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RENDER_GLOW].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_UI])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RENDER_UI].clear();

	return S_OK;
}

HRESULT CRenderer::Final_Deferred()
{



#pragma region GlowPass
	
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GlowFront"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_GlowBack"), m_pShader, "g_GlowTexture")))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	m_pShader->Begin(8);

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;
	
//-----------------------------------------------------------------------------------------------------------------
	
	m_bDoubleBuffer = !m_bDoubleBuffer;

	if (m_bDoubleBuffer) {
		if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_DeferredBack"))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_DeferredFront"), m_pShader, "g_DeferredTexture")))
			return E_FAIL;
	}
	else {
		if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_DeferredFront"))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_DeferredBack"), m_pShader, "g_DeferredTexture")))
			return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_GlowFront"), m_pShader, "g_GlowTexture")))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_GlowBack"), m_pShader, "g_GlowOriginalTexture")))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	m_pShader->Begin(9);

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;
#pragma endregion

#pragma region GlitchPass

	if (m_pGameInstance->Get_UseGlitchLens()) {

		m_bDoubleBuffer = !m_bDoubleBuffer;

		if (m_bDoubleBuffer) {
			if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_DeferredBack"))))
				return E_FAIL;
			if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_DeferredFront"), m_pShader, "g_DeferredTexture")))
				return E_FAIL;
		}
		else {
			if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_DeferredFront"))))
				return E_FAIL;
			if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_DeferredBack"), m_pShader, "g_DeferredTexture")))
				return E_FAIL;
		}

		if (FAILED(m_pShader->Bind_Float("shake_power", m_pGameInstance->Get_GlitchPower())))
			return E_FAIL;

		if (FAILED(m_pShader->Bind_Float("shake_rate", m_pGameInstance->Get_GlitchRate())))
			return E_FAIL;

		if (FAILED(m_pShader->Bind_Float("shake_speed", m_pGameInstance->Get_GlitchSpeed())))
			return E_FAIL;

		if (FAILED(m_pShader->Bind_Float("TIME", m_pGameInstance->Random_Float(0.f, 1.f))))
			return E_FAIL;

		if (FAILED(m_pShader->Bind_Float("shake_color_rate", m_pGameInstance->Get_GlitchColorRate())))
			return E_FAIL;

		if (FAILED(m_pShader->Bind_Float("red_mix_rate", m_pGameInstance->Get_RedBlockMixRate())))
			return E_FAIL;

		m_pDeathTexture->Bind_ShaderResource(m_pShader, "g_DeathTexture", 0);

		_bool bUseRedBlock = m_pGameInstance->Get_UseRedBlockGlitch();

		if (FAILED(m_pShader->Bind_RawValue("g_bUseRedGlitch", &bUseRedBlock, sizeof(_bool))))
			return E_FAIL;

		_bool bUseBlackScreen = m_pGameInstance->Get_BlackScreen();

		if (FAILED(m_pShader->Bind_RawValue("g_bBlackScreen", &bUseBlackScreen, sizeof(_bool))))
			return E_FAIL;

		if (FAILED(m_pShader->Bind_Float("g_fBlackScreenRate", m_pGameInstance->Get_BlackScreenRate())))
			return E_FAIL;


		if (FAILED(m_pVIBuffer->Bind_Buffers()))
			return E_FAIL;

		m_pShader->Begin(6);

		m_pVIBuffer->Render();

		if (FAILED(m_pShader->Bind_SRV("g_DeferredTexture", nullptr)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->End_MRT()))
			return E_FAIL;
	}
#pragma endregion


#pragma region Blur
	if (m_pGameInstance->Get_BlurScreen()) {

		m_bDoubleBuffer = !m_bDoubleBuffer;

		if (m_bDoubleBuffer) {
			if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_DeferredBack"))))
				return E_FAIL;
			if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_DeferredFront"), m_pShader, "g_DeferredTexture")))
				return E_FAIL;
		}
		else {
			if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_DeferredFront"))))
				return E_FAIL;
			if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_DeferredBack"), m_pShader, "g_DeferredTexture")))
				return E_FAIL;
		}

		if (FAILED(m_pShader->Bind_Float("g_fBlueScreenRate", m_pGameInstance->Get_BlueScreenRate())))
			return E_FAIL;

		if (FAILED(m_pShader->Bind_Float("g_BlurStrength", m_pGameInstance->Get_BlurPower())))
			return E_FAIL;

		_bool bUseBlueScreen = m_pGameInstance->Get_BlueScreen();

		if (FAILED(m_pShader->Bind_RawValue("g_bUseBlueScreen", &bUseBlueScreen, sizeof(_bool))))
			return E_FAIL;

		_bool bUseRedScreen = m_pGameInstance->Get_RedScreen();

		if (FAILED(m_pShader->Bind_RawValue("g_bUseRedScreen", &bUseRedScreen, sizeof(_bool))))
			return E_FAIL;

		if (FAILED(m_pVIBuffer->Bind_Buffers()))
			return E_FAIL;

		m_pShader->Begin(7);

		m_pVIBuffer->Render();

		if (FAILED(m_pGameInstance->End_MRT()))
			return E_FAIL;
	}
#pragma endregion


	if (m_bDoubleBuffer) {
		if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_DeferredBack"), m_pShader, "g_DeferredTexture")))
			return E_FAIL;
	}
	else {
		if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_DeferredFront"), m_pShader, "g_DeferredTexture")))
			return E_FAIL;
	}

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	m_pShader->Begin(5);

	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_LightAcc()
{
	/* Shade */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_LightAcc"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Normal"), m_pShader, "g_NormalTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return E_FAIL;

	/* �����س��� ����Ʈ Ÿ�ٿ� ���,�� �ȼ��� ä���ֱ����ؼ�. */
	/* ���̵� Ÿ���� �����ŭ �������п��� �簡�q�� ���۸� �׸���. */
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(CCamera_Manager::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(CCamera_Manager::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	/* �� ������ŭ �簢���� �׸���. */
	if (FAILED(m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Deferred()
{

	m_bDoubleBuffer = true;

	if (m_bDoubleBuffer) {
		if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_DeferredBack"))))
			return E_FAIL;
	}
	else {
		if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_DeferredFront"))))
			return E_FAIL;
	}


	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Shade"), m_pShader, "g_ShadeTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Specular"), m_pShader, "g_SpecularTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	m_pShader->Begin(3);

	m_pVIBuffer->Render();

	//m_bDoubleBuffer = !m_bDoubleBuffer;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;


#pragma region FinalPass
	/*if (m_bDoubleBuffer) {
		if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_DeferredBack"), m_pShader, "g_DeferredTexture")))
			return E_FAIL;
	}
	else {
		if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_DeferredFront"), m_pShader, "g_DeferredTexture")))
			return E_FAIL;
	}

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	m_pShader->Begin(5);

	m_pVIBuffer->Render();*/

#pragma endregion

	return S_OK;
}

#ifdef _DEBUG

HRESULT CRenderer::Render_Debug()
{
	for (auto& pDebugCom : m_DebugComponents)
	{
		pDebugCom->Render();
		Safe_Release(pDebugCom);
	}
	m_DebugComponents.clear();

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_RTDebug(TEXT("MRT_GameObjects"), m_pShader, m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_RTDebug(TEXT("MRT_LightAcc"), m_pShader, m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_RTDebug(TEXT("MRT_DeferredBack"), m_pShader, m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_RTDebug(TEXT("MRT_DeferredFront"), m_pShader, m_pVIBuffer)))
		return E_FAIL;

	if(FAILED(m_pGameInstance->Render_RTDebug(TEXT("MRT_GlowBack"), m_pShader, m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_RTDebug(TEXT("MRT_GlowFront"), m_pShader, m_pVIBuffer)))
		return E_FAIL;

	return S_OK;
}

#endif

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CRenderer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderer::Free()
{
	Safe_Release(m_pGameInstance);

	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);

#ifdef _DEBUG
	for (auto& pDebugCom : m_DebugComponents)
		Safe_Release(pDebugCom);
	m_DebugComponents.clear();
#endif

	for (auto& RenderObjects : m_RenderObjects)
	{
		for (auto& pRenderObject : RenderObjects)
			Safe_Release(pRenderObject);
		RenderObjects.clear();
	}

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pDeathTexture);
}
