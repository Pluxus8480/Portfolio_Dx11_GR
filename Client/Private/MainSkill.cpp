#include "MainSkill.h"
#include "PlayerManager.h"
#include "GameInstance.h"

CMainSkill::CMainSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUi{ pDevice, pContext }
{
}

CMainSkill::CMainSkill(const CMainSkill& rhs)
	: CUi{ rhs }
{
}

HRESULT CMainSkill::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Initialize(nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainSkill::Initialize(void* pArg)
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

void CMainSkill::PriorityTick(_float fTimeDelta)
{
}

void CMainSkill::Tick(_float fTimeDelta)
{
}

void CMainSkill::LateTick(_float fTimeDelta)
{
	if(Get_Active())
		m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_UI, this);
}

HRESULT CMainSkill::Render()
{

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

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(6)))
		return E_FAIL;
	
	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	//pBlendState->Release();
	//pBlendState = nullptr;

	return S_OK;
}

void CMainSkill::Initialize_Set_Scale_Pos_Rotation(void* pArg)
{
	m_fSizeX = 180.f;
	m_fSizeY = 180.f;

	Set_Pos({-500,-200});
	Set_Divide(1.6f);

	m_fScale = { 180.f / Get_Divide() , 180.f / Get_Divide(), 1.f };
	m_fRotation = { 0.f, 0.f, -5.f };

	m_pTransformCom->Set_Scale(m_fScale);
	m_pTransformCom->Rotation_XYZ(m_fRotation);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-500.f, -200.f, 0.1f, 1.f));
}

HRESULT CMainSkill::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SKILL_Hud_Bar_Bg"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUi* CMainSkill::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMainSkill* pInstance = new CMainSkill(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMainSkill"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CUi* CMainSkill::Clone(void* pArg)
{
	CMainSkill* pInstance = new CMainSkill(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMainSkill"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainSkill::Free()
{
	__super::Free();
}
