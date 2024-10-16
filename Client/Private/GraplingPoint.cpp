#include "GraplingPoint.h"
#include "Mesh.h"
#include "GameInstance.h"
#include "PlayerManager.h"
#include "Collision_Manager.h"

CGraplingPointObject::CGraplingPointObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CGraplingPointObject::CGraplingPointObject(const CGraplingPointObject& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CGraplingPointObject::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGraplingPointObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Set_MainTag("GraplingPoint");
	Set_SubTag("GraplingPoint");

	m_pTransformCom->Set_Scale(_float3(2.3f,2.3f,1.f));

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CGraplingPointObject::PriorityTick(_float fTimeDelta)
{

}

void CGraplingPointObject::Tick(_float fTimeDelta)
{
	if (GAMEINSTANCE->Get_CurrentLevelIndex() == LEVEL_TOOL)
		return;

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	m_pTransformCom->LookAt(GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_POSITION));
	m_pTransformCom->PureTurn(m_pTransformCom->Get_State(CTransform::STATE_UP), 3.141592f);

	m_pTransformCom->Set_Scale(_float3(2.6f, 2.6f, 1.f));

	if (m_pColliderCom->IsPicked()) {
		m_pTransformCom->PureTurn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), m_fRotating);
		m_pTransformCom->Set_Scale(_float3(3.2f, 3.2f, 1.f));
		m_fRotating += fTimeDelta;
	} else {
		m_pTransformCom->Set_Scale(_float3(2.6f, 2.6f, 1.f));
		m_fRotating = 0.f;
	}
}

void CGraplingPointObject::LateTick(_float fTimeDelta)
{
	//m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_BLEND, this);
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONLIGHT, this);
	/*for (auto& pParts : m_vecParts)
		pParts->LateTick(fTimeDelta);*/

}

HRESULT CGraplingPointObject::Render()
{
	Bind_ShaderResources();

	if (FAILED(m_pShaderCom->Begin(3)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

#ifdef _DEBUG
	if (GAMEINSTANCE->Get_DebugMode())
		m_pColliderCom->Render();
#endif

	//pBlendState->Release();
	//pBlendState = nullptr;

	return S_OK;
}

void CGraplingPointObject::Collision_RayHit()
{
	if(GAMEINSTANCE->Get_CurrentLevelIndex() == LEVEL_TOOL)
		return;

	if (GAMEINSTANCE->GetKeyDown(eKeyCode::RButton) && PLAYER->Get_PlayerToTargetDistance(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) <= 40.f) {
		PLAYER->Get_PlayerArm()->Hard_Animation_Change(6, false);
		PLAYER->Set_GrabTarget(this);
		PLAYER->Set_WireState(CPlayer_Manager::WIRE_STATE::WIRE_START);
		PLAYER->Set_PlayerState(CPlayer_Manager::PLAYER_STATE::PLAYER_WIREACTION);
		
		switch (GAMEINSTANCE->Random_Int(1, 3))
		{
		case 1:
			GAMEINSTANCE->Play(TEXT("Sound_Player_Wire_01"), false);
			GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Wire_01"), 1.f);
			break;
		case 2:
			GAMEINSTANCE->Play(TEXT("Sound_Player_Wire_02"), false);
			GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Wire_02"), 1.f);
			break;
		case 3:
			GAMEINSTANCE->Play(TEXT("Sound_Player_Wire_03"), false);
			GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Wire_03"), 1.f);
			break;
		}

	}

	//m_pTransformCom->Set_Scale(_float3(3.f, 3.f, 1.f));

}

HRESULT CGraplingPointObject::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Grappling_Point_Outer_Ring"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;


	/* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC	BoundingDesc{};

	BoundingDesc.fRadius = 3.0f;
	BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	m_pColliderCom->Set_Owner(this);
	GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_MONSTER, m_pColliderCom);

	return S_OK;
}

HRESULT CGraplingPointObject::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CCamera_Manager::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CCamera_Manager::D3DTS_PROJ))))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
	//	return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

CGraplingPointObject* CGraplingPointObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGraplingPointObject* pInstance = new CGraplingPointObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CGraplingPointObject"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CGraplingPointObject::Clone(void* pArg)
{
	CGraplingPointObject* pInstance = new CGraplingPointObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CGraplingPointObject"));
		Safe_Release(pInstance);
	}

	GAMEINSTANCE->Imgui_SetSelectObject(pInstance);

	return pInstance;
}

void CGraplingPointObject::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pVIBufferCom);

}
