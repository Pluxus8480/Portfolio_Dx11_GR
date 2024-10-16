#include "Blood.h"
#include "GameInstance.h"
#include "PlayerManager.h"
#include "Collision_Manager.h"

CBlood::CBlood(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CBlood::CBlood(const CBlood& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CBlood::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBlood::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_float3(3.f, 3.f, 1.f));

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CBlood::PriorityTick(_float fTimeDelta)
{

}

void CBlood::Tick(_float fTimeDelta)
{
	m_fFrame += fTimeDelta;

	if (m_fFrame >= m_fFrameMax) {
		m_fFrame = m_fFrame - m_fFrameMax;
	
		m_iTextureIndex++;
	
		if (m_iTextureIndex >= m_iTextureMax)
			Set_IsDead(true);
	
	}






	if (GAMEINSTANCE->Get_CurrentLevelIndex() == LEVEL_TOOL)
		return;

	m_pTransformCom->LookAt(GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_POSITION));
	m_pTransformCom->PureTurn(m_pTransformCom->Get_State(CTransform::STATE_UP), 3.141592f);

	m_pTransformCom->Set_Scale(_float3(2.6f, 2.6f, 1.f));
}

void CBlood::LateTick(_float fTimeDelta)
{
	//m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_BLEND, this);
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONLIGHT, this);
	/*for (auto& pParts : m_vecParts)
		pParts->LateTick(fTimeDelta);*/

}

HRESULT CBlood::Render()
{
	Bind_ShaderResources();

	if (FAILED(m_pShaderCom->Begin(8)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	//pBlendState->Release();
	//pBlendState = nullptr;

	return S_OK;
}

void CBlood::Collision_RayHit()
{
	if (GAMEINSTANCE->Get_CurrentLevelIndex() == LEVEL_TOOL)
		return;

	if (GAMEINSTANCE->GetKeyDown(eKeyCode::RButton) && PLAYER->Get_PlayerToTargetDistance(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) <= 40.f) {
		PLAYER->Get_PlayerArm()->Hard_Animation_Change(6, false);
		PLAYER->Set_GrabTarget(this);
		PLAYER->Set_WireState(CPlayer_Manager::WIRE_STATE::WIRE_START);
		PLAYER->Set_PlayerState(CPlayer_Manager::PLAYER_STATE::PLAYER_WIREACTION);
	}

	//m_pTransformCom->Set_Scale(_float3(3.f, 3.f, 1.f));

}

HRESULT CBlood::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Blood"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBlood::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CCamera_Manager::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CCamera_Manager::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIndex)))
		return E_FAIL;

	return S_OK;
}

CBlood* CBlood::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBlood* pInstance = new CBlood(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBlood"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CBlood::Clone(void* pArg)
{
	CBlood* pInstance = new CBlood(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBlood"));
		Safe_Release(pInstance);
	}

	GAMEINSTANCE->Imgui_SetSelectObject(pInstance);

	return pInstance;
}

void CBlood::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);

}
