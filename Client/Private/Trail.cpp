#include "Trail.h"
#include "GameInstance.h"
#include "VIBuffer_Trail.h"

CTrail::CTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CTrail::CTrail(const CTrail& rhs)
	: CPartObject{ rhs }
{
}

HRESULT CTrail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTrail::Initialize(void* pArg)
{
	TRAILOBJ_DESC* pDesc = static_cast<TRAILOBJ_DESC*>(pArg);

	m_pBoneMatrix = const_cast<_float4x4*>(pDesc->pBoneMatrix);
	m_pBoneMatrix2 = const_cast<_float4x4*>(pDesc->pBoneMatrix2);
	m_pTarget = pDesc->pTarget;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pBufferCom->Start(m_pBoneMatrix, m_pBoneMatrix2);
	m_pBufferCom->SetTarget(m_pTarget);
	return S_OK;
}

void CTrail::PriorityTick(_float fTimeDelta)
{
}

void CTrail::Tick(_float fTimeDelta)
{
	m_pBufferCom->Tick(fTimeDelta);
	/*_matrix		BoneMatrix = XMLoadFloat4x4(m_pBoneMatrix);

	for (size_t i = 0; i < 3; i++)
		BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix() * BoneMatrix);*/

	//m_pModelCom->Play_Animation(fTimeDelta);
}

void CTrail::LateTick(_float fTimeDelta)
{
	if(m_bOn) m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_GLOW, this);
}

HRESULT CTrail::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(4);

	m_pBufferCom->Bind_Buffers();
	
	m_pBufferCom->Render();

	return S_OK;
}

HRESULT CTrail::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
	
	CVIBuffer_Trail::TRAIL_DESC pDesc{};
	pDesc.iMaxRect = 10;

	m_pBufferCom = CVIBuffer_Trail::Create(m_pDevice, m_pContext, pDesc);

	return S_OK;
}

HRESULT CTrail::Bind_ShaderResources()
{
	/* 부모의 행렬을 완전히 무시한 상황. */

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CCamera_Manager::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CCamera_Manager::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CTrail* CTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTrail* pInstance = new CTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CTrail::Clone(void* pArg)
{
	CTrail* pInstance = new CTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CTrail::Free()
{
	__super::Free();

	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
}
