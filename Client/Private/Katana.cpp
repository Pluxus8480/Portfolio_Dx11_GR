#include "Katana.h"
#include "GameInstance.h"
#include "Trail.h"
#include "PlayerManager.h"

CKatana::CKatana(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CKatana::CKatana(const CKatana& rhs)
	: CPartObject{ rhs }
{
}

HRESULT CKatana::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CKatana::Initialize(void* pArg)
{
	WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);

	m_pBoneMatrix = pDesc->pBoneMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_float3(100.f, 100.f, 100.f));
	//m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(90.0f));
	//m_pTransformCom->PureTurn(XMVectorSet(0.f, 0.f, 1.f, 1.f), XMConvertToRadians(-90.0f));
	m_pTransformCom->PureTurn(XMVectorSet(1.f, 0.f, 0.f, 1.f), XMConvertToRadians(90.0f));
	m_pTransformCom->PureTurn(XMVectorSet(0.f, 0.f, 1.f, 1.f), XMConvertToRadians(180.0f));
	//m_pTransformCom->Rotation_XYZ(_float3(XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(0.f)));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.8f, 0.f, 0.f, 1.f));

	CTrail::TRAILOBJ_DESC tTrailDesc{};
	tTrailDesc.pBoneMatrix = m_pModelCom->Get_BoneMatrixPtr("Root");
	tTrailDesc.pBoneMatrix2 = m_pModelCom->Get_BoneMatrixPtr("Root");
	tTrailDesc.pTarget = &m_WorldMatrix;

	m_pTrailCom = dynamic_cast<CTrail*>(GAMEINSTANCE->Add_Clone_Return(LEVEL_STATIC,L"Trail", L"Prototype_Trail_Effect", &tTrailDesc));


	return S_OK;
}

void CKatana::PriorityTick(_float fTimeDelta)
{
}

void CKatana::Tick(_float fTimeDelta)
{
	m_pTrailCom->Set_TrailOn(false);

	if ((PLAYER->Get_IsAttack() == true || m_bOn)/* || (PLAYER->Get_PlayerState() == CPlayer_Manager::PLAYER_JUMP) || (PLAYER->Get_PlayerState() == CPlayer_Manager::PLAYER_MOVE)*/)
		m_pTrailCom->Set_TrailOn(true);

	_matrix		BoneMatrix = XMLoadFloat4x4(m_pBoneMatrix);

	for (size_t i = 0; i < 3; i++)
		BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix() * BoneMatrix);

	//m_pModelCom->Play_Animation(fTimeDelta);
}

void CKatana::LateTick(_float fTimeDelta)
{
	//m_pModelCom->Play_Animation(fTimeDelta);
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);

}

HRESULT CKatana::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
			return E_FAIL;

		m_pModelCom->Render((_uint)i);
	}


	return S_OK;
}

HRESULT CKatana::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_GhostRunner_Katana"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CKatana::Bind_ShaderResources()
{
	/* 부모의 행렬을 완전히 무시한 상황. */
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CCamera_Manager::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CCamera_Manager::D3DTS_PROJ))))
		return E_FAIL; 

	return S_OK;
}

CKatana* CKatana::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CKatana* pInstance = new CKatana(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CKatana"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CKatana::Clone(void* pArg)
{
	CKatana* pInstance = new CKatana(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CKatana"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CKatana::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
