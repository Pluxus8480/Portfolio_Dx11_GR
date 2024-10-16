#include "MainSkillIcon.h"
#include "PlayerManager.h"
#include "GameInstance.h"

CMainSkillIcon::CMainSkillIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUi{ pDevice, pContext }
{
}

CMainSkillIcon::CMainSkillIcon(const CMainSkillIcon& rhs)
	: CUi{ rhs }
{
}

HRESULT CMainSkillIcon::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Initialize(nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainSkillIcon::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Initialize_Set_Scale_Pos_Rotation(pArg);

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));



	return S_OK;
}

void CMainSkillIcon::PriorityTick(_float fTimeDelta)
{
}

void CMainSkillIcon::Tick(_float fTimeDelta)
{
}

void CMainSkillIcon::LateTick(_float fTimeDelta)
{
	if (Get_Active())
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_UI, this);
}

HRESULT CMainSkillIcon::Render()
{
	// 블렌드 상태 설명 구조체 초기화
	//D3D11_BLEND_DESC blendDesc;
	//ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));

	//// 알파 블렌딩 설정
	//blendDesc.RenderTarget[0].BlendEnable = TRUE;
	//blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	//blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	//blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	//blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	//blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	//blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	//blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	//// 블렌드 상태 객체 생성
	//HRESULT hr = m_pDevice->CreateBlendState(&blendDesc, &pBlendState);
	//if (FAILED(hr)) {
	//	// 오류 처리
	//}

	//float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	//UINT sampleMask = 0xffffffff;
	//m_pContext->OMSetBlendState(pBlendState, blendFactor, sampleMask);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_Float("g_fRand", m_pGameInstance->Random_Float(0.f,0.3f))))
	//	return E_FAIL;

	_bool bSkillOn = PLAYER->Get_SkillUsable();

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bSkillOn", &bSkillOn, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", PLAYER->Get_Skill_Index())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(5)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	/*BlendState->Release();
	pBlendState = nullptr;*/

	return S_OK;
}

void CMainSkillIcon::Initialize_Set_Scale_Pos_Rotation(void* pArg)
{

	m_fSizeX = 180.f;
	m_fSizeY = 180.f;

	Set_Pos({ -500,-200 });
	Set_Divide(2.0f);

	m_fScale = { 180.f / Get_Divide() , 180.f / Get_Divide(), 1.f };
	m_fRotation = { 0.f, 0.f, -5.f };

	m_pTransformCom->Set_Scale(m_fScale);
	m_pTransformCom->Rotation_XYZ(m_fRotation);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-500.f, -200.f, 0.f, 1.f));

}

HRESULT CMainSkillIcon::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SKILL_Icon"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUi* CMainSkillIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMainSkillIcon* pInstance = new CMainSkillIcon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMainSkillIcon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CUi* CMainSkillIcon::Clone(void* pArg)
{
	CMainSkillIcon* pInstance = new CMainSkillIcon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMainSkillIcon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainSkillIcon::Free()
{
	__super::Free();
}
