#include "SkillEnergyBar.h"
#include "PlayerManager.h"
#include "GameInstance.h"

CSkillEnergyBar::CSkillEnergyBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUi{ pDevice, pContext }
{
}

CSkillEnergyBar::CSkillEnergyBar(const CSkillEnergyBar& rhs)
	: CUi{ rhs }
{
}

HRESULT CSkillEnergyBar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Initialize(nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkillEnergyBar::Initialize(void* pArg)
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

void CSkillEnergyBar::PriorityTick(_float fTimeDelta)
{
}

void CSkillEnergyBar::Tick(_float fTimeDelta)
{
}

void CSkillEnergyBar::LateTick(_float fTimeDelta)
{
	if (Get_Active())
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_UI, this);
}

HRESULT CSkillEnergyBar::Render()
{

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Float("weight", PLAYER->Get_SkillGauge())))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CSkillEnergyBar::Initialize_Set_Scale_Pos_Rotation(void* pArg)
{

	m_fSizeX = 180.f;
	m_fSizeY = 180.f;

	Set_Pos({ -500,-200 });
	Set_Divide(1.0f);

	m_fScale = { 180.f / Get_Divide() , 180.f / Get_Divide(), 1.f };
	m_fRotation = { 0.f, 0.f, -5.f };

	m_pTransformCom->Set_Scale(m_fScale);
	m_pTransformCom->Rotation_XYZ(m_fRotation);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-500.f, -200.f, 0.f, 1.f));

}

HRESULT CSkillEnergyBar::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UiGauge"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SKILL_Hud_Bar_Energy_Bg"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUi* CSkillEnergyBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkillEnergyBar* pInstance = new CSkillEnergyBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSkillEnergyBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CUi* CSkillEnergyBar::Clone(void* pArg)
{
	CSkillEnergyBar* pInstance = new CSkillEnergyBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSkillEnergyBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkillEnergyBar::Free()
{
	__super::Free();
}
