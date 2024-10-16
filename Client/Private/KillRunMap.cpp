#include "KillRunMap.h"
#include "Mesh.h"
#include "GameInstance.h"

CKillRunMap::CKillRunMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CKillRunMap::CKillRunMap(const CKillRunMap& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CKillRunMap::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CKillRunMap::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	//m_pTransformCom->Go_Up(10.f);
	//m_pTransformCom->Set_Scale({ 0.01f, 0.01f, 0.01f });

	if (FAILED(Ready_Components()))
		return E_FAIL;



	return S_OK;
}

void CKillRunMap::PriorityTick(_float fTimeDelta)
{
}

void CKillRunMap::Tick(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);
}

void CKillRunMap::LateTick(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CKillRunMap::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CCamera_Manager::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CCamera_Manager::D3DTS_PROJ))))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE)))
			continue;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", (_uint)i, aiTextureType_NORMALS)))
			return E_FAIL;

		if(FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pModelCom->Render((_uint)i);
	}

	///*if (FAILED(m_pShaderCom->Bind_Float("g_fRandR", m_pGameInstance->Random_Float(0.f, 0.5f))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_Float("g_fRandG", m_pGameInstance->Random_Float(0.f, 0.5f))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_Float("g_fRandB", m_pGameInstance->Random_Float(0.f, 0.5f))))*/
		//return E_FAIL;

	return S_OK;
}

HRESULT CKillRunMap::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_KillRun_1_Map"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	///* For.Com_VIBuffer */
	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_PistolGunner"),
	//	TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	//	return E_FAIL;

	/* For PhysX Collider*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysX"), reinterpret_cast<CComponent**>(&m_pPhysXCollider))))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();



	for (size_t i = 0; i < iNumMeshes; i++)
	{
		string PistolGunner = "KillRUnMap";
		string tmp = to_string(i);

		PistolGunner += tmp;
		m_pPhysXCollider->createStaticMeshActor(m_pModelCom->Get_Mesh((_uint)i)->Get_TriangleMesh(), PistolGunner.c_str());
	}

	return S_OK;
}

HRESULT CKillRunMap::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CKillRunMap::Model_Rendering(_int iMeshNum)
{
	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", (_uint)iMeshNum, aiTextureType_DIFFUSE)))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pModelCom->Render((_uint)iMeshNum);

	return S_OK;

}

CKillRunMap* CKillRunMap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CKillRunMap* pInstance = new CKillRunMap(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CKillRunMap"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CKillRunMap::Clone(void* pArg)
{
	CKillRunMap* pInstance = new CKillRunMap(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CKillRunMap"));
		Safe_Release(pInstance);
	}

	GAMEINSTANCE->Imgui_SetSelectObject(pInstance);

	return pInstance;
}

void CKillRunMap::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pPhysXCollider);

}
