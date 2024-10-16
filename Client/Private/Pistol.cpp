#include "Pistol.h"
#include "GameInstance.h"

CPistol::CPistol(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CPistol::CPistol(const CPistol& rhs)
	: CPartObject{ rhs }
{
}

HRESULT CPistol::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPistol::Initialize(void* pArg)
{
	WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);

	m_pBoneMatrix = pDesc->pBoneMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_float3(100.f, 100.f, 100.f));
	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-90.0f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.8f, 0.f, 0.f, 1.f));


	return S_OK;
}

void CPistol::PriorityTick(_float fTimeDelta)
{
}

void CPistol::Tick(_float fTimeDelta)
{
	_matrix		BoneMatrix = XMLoadFloat4x4(m_pBoneMatrix);

	for (size_t i = 0; i < 3; i++)
		BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix() * BoneMatrix);
}

void CPistol::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CPistol::Render()
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

HRESULT CPistol::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_PistolGunnerWeapon"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPistol::Bind_ShaderResources()
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

CPistol* CPistol::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPistol* pInstance = new CPistol(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPistol"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CPistol::Clone(void* pArg)
{
	CPistol* pInstance = new CPistol(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPistol"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CPistol::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
