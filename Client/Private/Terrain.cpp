//#include "..\Public\Terrain.h"
//
//#include "GameInstance.h"
//
//CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
//	: CGameObject{ pDevice, pContext }
//{
//}
//
//CTerrain::CTerrain(const CTerrain& rhs)
//	: CGameObject{ rhs }
//{
//}
//
//HRESULT CTerrain::Initialize_Prototype()
//{
//	if (FAILED(__super::Initialize_Prototype()))
//		return E_FAIL;
//
//	return S_OK;
//}
//
//HRESULT CTerrain::Initialize(void* pArg)
//{
//	CTransform::TRANSFORM_DESC			TransformDesc{};
//	TransformDesc.fSpeedPerSec = 1.f;
//	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
//
//	if (FAILED(__super::Initialize(&TransformDesc)))
//		return E_FAIL;
//
//	if (FAILED(Ready_Components()))
//		return E_FAIL;
//
//	return S_OK;
//}
//
//void CTerrain::PriorityTick(_float fTimeDelta)
//{
//}
//
//void CTerrain::Tick(_float fTimeDelta)
//{
//}
//
//void CTerrain::LateTick(_float fTimeDelta)
//{
//	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);
//}
//
//HRESULT CTerrain::Render()
//{
//	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
//		return E_FAIL;
//
//	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CCamera_Manager::D3DTS_VIEW))))
//		return E_FAIL;
//
//	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CCamera_Manager::D3DTS_PROJ))))
//		return E_FAIL;
//
//	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
//		return E_FAIL;
//
//	///*if (FAILED(m_pShaderCom->Bind_Float("g_fRandR", m_pGameInstance->Random_Float(0.f, 0.5f))))
//	//	return E_FAIL;
//
//	//if (FAILED(m_pShaderCom->Bind_Float("g_fRandG", m_pGameInstance->Random_Float(0.f, 0.5f))))
//	//	return E_FAIL;
//
//	//if (FAILED(m_pShaderCom->Bind_Float("g_fRandB", m_pGameInstance->Random_Float(0.f, 0.5f))))*/
//		//return E_FAIL;
//
//	if (FAILED(m_pShaderCom->Begin(0)))
//		return E_FAIL;
//
//	if (FAILED(m_pVIBufferCom->Render()))
//		return E_FAIL;
//
//	return S_OK;
//}
//
//HRESULT CTerrain::Ready_Components()
//{
//	/* For.Com_Shader */
//	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
//		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
//		return E_FAIL;
//
//	/* For.Com_Texture */
//	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
//		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
//		return E_FAIL;
//
//	/* For.Com_VIBuffer */
//	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
//		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
//		return E_FAIL;
//
//	return S_OK;
//}
//
//CTerrain* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
//{
//	CTerrain* pInstance = new CTerrain(pDevice, pContext);
//
//	if (FAILED(pInstance->Initialize_Prototype()))
//	{
//		MSG_BOX(TEXT("Failed to Created : CTerrain"));
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}
//
//
//CGameObject* CTerrain::Clone(void* pArg)
//{
//	CTerrain* pInstance = new CTerrain(*this);
//
//	if (FAILED(pInstance->Initialize(pArg)))
//	{
//		MSG_BOX(TEXT("Failed to Cloned : CTerrain"));
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}
//
//void CTerrain::Free()
//{
//	__super::Free();
//
//	Safe_Release(m_pShaderCom);
//	Safe_Release(m_pTextureCom);
//	Safe_Release(m_pVIBufferCom);
//}
