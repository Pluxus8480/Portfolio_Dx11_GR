#include "Wire.h"
#include "GameInstance.h"
#include "VIBuffer_TrailRect.h"

CWire::CWire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CWire::CWire(const CWire& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CWire::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWire::Initialize(void* pArg)
{
	WIRE_DESC* pDesc = static_cast<WIRE_DESC*>(pArg);

	m_pBoneMatrix = const_cast<_float4x4*>(pDesc->pBoneMatrix);
	m_pTarget = const_cast<_float4x4*>(pDesc->pTarget);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pBufferCom->SetTarget(m_pTarget);

	return S_OK;
}

void CWire::PriorityTick(_float fTimeDelta)
{
	if (m_pBoneMatrix2 != nullptr && m_bOn && m_bStart == false) {
		m_pBufferCom->Start(m_pBoneMatrix, m_pBoneMatrix2);
		m_bStart = true;
	}
	
	if (m_bOn == false && m_bStart == true)
	{
		m_pBufferCom->End();
		m_bStart = false;
	}

}

void CWire::Tick(_float fTimeDelta)
{
	m_pBufferCom->Tick(fTimeDelta);
	/*_matrix		BoneMatrix = XMLoadFloat4x4(m_pBoneMatrix);

	for (size_t i = 0; i < 3; i++)
		BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix() * BoneMatrix);*/

	//m_pModelCom->Play_Animation(fTimeDelta);
}

void CWire::LateTick(_float fTimeDelta)
{
	if (m_bOn) m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_GLOW, this);
}

HRESULT CWire::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(10);

	m_pBufferCom->Bind_Buffers();

	m_pBufferCom->Render();

	return S_OK;
}

HRESULT CWire::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	CVIBuffer_TrailRect::TRAIL_DESC pDesc{};
	pDesc.iMaxRect = 1;

	m_pBufferCom = CVIBuffer_TrailRect::Create(m_pDevice, m_pContext, pDesc);

	return S_OK;
}

HRESULT CWire::Bind_ShaderResources()
{
	/* 부모의 행렬을 완전히 무시한 상황. */

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CCamera_Manager::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CCamera_Manager::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CWire* CWire::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWire* pInstance = new CWire(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWire"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CWire::Clone(void* pArg)
{
	CWire* pInstance = new CWire(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWire"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CWire::Free()
{
	__super::Free();

	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
}
