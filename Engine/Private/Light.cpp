#include "..\Public\Light.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "Transform.h"
#include "GameInstance.h"

CLight::CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

HRESULT CLight::Initialize(void* pArg)
{
	LIGHT_DESC* pLightDesc = reinterpret_cast<LIGHT_DESC*>(pArg);
	memcpy(&m_LightDesc, pLightDesc, sizeof(LIGHT_DESC));


	CTransform::TRANSFORM_DESC tTransformDesc{};
	tTransformDesc.vPosition = pLightDesc->vPosition;


	if (FAILED(__super::Initialize(&tTransformDesc)))
		return E_FAIL;

	Ready_Components();

	Set_MainTag("Light");
	Set_SubTag("Light");

	m_pTransformCom->Set_Scale({ 0.3f, 0.6f, 0.3f });
	//m_pTransformCom->Set_Scale({ 0.7f,0.2f,3.f });

	return S_OK;
}

HRESULT CLight::Lighting(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	_float4 color1 = { 0.27f, 0.84f, 0.89f, 1.0f };
	_float4 color2 = { 0.2f, 0.45f, 0.84f, 1.0f };
	_float4 color3 = { 0.11f, 0.92f, 0.7f, 1.0f };

	if (LIGHT_DESC::TYPE_DIRECTIONAL == m_LightDesc.eType && GAMEINSTANCE->Get_CurrentLevelIndex() == 3) {
		m_fTimeAcc += 0.02f;
		
		if (m_fTimeAcc > 2.f) {
			m_fTimeAcc = 0;
			m_iPattern++;
		}

		if ((m_iPattern & 3) == 0){
			m_LightDesc.vDiffuse.x += (color1.x - m_LightDesc.vDiffuse.x) * 0.015f;
			m_LightDesc.vDiffuse.y += (color1.y - m_LightDesc.vDiffuse.y) * 0.015f;
			m_LightDesc.vDiffuse.z += (color1.z - m_LightDesc.vDiffuse.z) * 0.015f;
			m_LightDesc.vDiffuse.w += (color1.w - m_LightDesc.vDiffuse.w) * 0.015f;
		}
	
		if ((m_iPattern & 3) == 1) {
			m_LightDesc.vDiffuse.x += (color2.x - m_LightDesc.vDiffuse.x) * 0.015f;
			m_LightDesc.vDiffuse.y += (color2.y - m_LightDesc.vDiffuse.y) * 0.015f;
			m_LightDesc.vDiffuse.z += (color2.z - m_LightDesc.vDiffuse.z) * 0.015f;
			m_LightDesc.vDiffuse.w += (color2.w - m_LightDesc.vDiffuse.w) * 0.015f;
		}

		if ((m_iPattern & 3) == 2) {
			m_LightDesc.vDiffuse.x += (color3.x - m_LightDesc.vDiffuse.x) * 0.015f;
			m_LightDesc.vDiffuse.y += (color3.y - m_LightDesc.vDiffuse.y) * 0.015f;
			m_LightDesc.vDiffuse.z += (color3.z - m_LightDesc.vDiffuse.z) * 0.015f;
			m_LightDesc.vDiffuse.w += (color3.w - m_LightDesc.vDiffuse.w) * 0.015f;
		}

		m_LightDesc.vSpecular = m_LightDesc.vDiffuse;
	}

	_uint		iPassIndex = { 0 };

	if (LIGHT_DESC::TYPE_DIRECTIONAL == m_LightDesc.eType)
	{
		iPassIndex = 1;

		if (FAILED(pShader->Bind_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4))))
			return E_FAIL;
	}
	else if (LIGHT_DESC::TYPE_POINT == m_LightDesc.eType)
	{
		iPassIndex = 2;

		if (FAILED(pShader->Bind_RawValue("g_vLightPos", &m_LightDesc.vPosition, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(pShader->Bind_RawValue("g_fLightRange", &m_LightDesc.fRange, sizeof(_float))))
			return E_FAIL;
	}
	else if (LIGHT_DESC::TYPE_SPOT == m_LightDesc.eType)
	{
		iPassIndex = 4;

		if (FAILED(pShader->Bind_RawValue("g_vLightPos", &m_pTransformCom->Get_State(CTransform::STATE_POSITION), sizeof(_float4))))
			return E_FAIL;

		if (FAILED(pShader->Bind_RawValue("g_vLightDir", &m_pTransformCom->Get_State(CTransform::STATE_LOOK), sizeof(_float4))))
			return E_FAIL;

		if (FAILED(pShader->Bind_RawValue("g_fLightRange", &m_LightDesc.fRange, sizeof(_float))))
			return E_FAIL;

		if (FAILED(pShader->Bind_Matrix("g_TrueViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CCamera_Manager::D3DTS_VIEW))))
			return E_FAIL;

		if (FAILED(pShader->Bind_Matrix("g_TrueProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CCamera_Manager::D3DTS_PROJ))))
			return E_FAIL;

		if (FAILED(pShader->Bind_RawValue("g_fSpotInnerConeAngle", &m_LightDesc.g_fSpotInnerConeAngle, sizeof(_float))))
			return E_FAIL;

		if (FAILED(pShader->Bind_RawValue("g_fSpotOuterConeAngle", &m_LightDesc.g_fSpotOuterConeAngle, sizeof(_float))))
			return E_FAIL;

		if (FAILED(pShader->Bind_RawValue("g_fLightFalloff", &m_LightDesc.g_fLightFalloff, sizeof(_float))))
			return E_FAIL;

	}


	if (FAILED(pShader->Bind_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_float4))))
		return E_FAIL;

	pShader->Begin(iPassIndex);

	pVIBuffer->Render();

	return S_OK;
}

HRESULT CLight::Ready_Components()
{
	if (FAILED(__super::Add_Component(0, TEXT("Prototype_Component_Shader_VtxBullet"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

		/* For.Com_Model */
	if (FAILED(__super::Add_Component(0, TEXT("Prototype_Component_Model_Bullet"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLight::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CCamera_Manager::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CCamera_Manager::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

void CLight::PriorityTick(_float fTimeDelta)
{
}

void CLight::Tick(_float fTimeDelta)
{
	XMStoreFloat4(&m_LightDesc.vPosition ,m_pTransformCom->Get_State(CTransform::STATE_POSITION));
}

void CLight::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_GLOW, this);
}

HRESULT CLight::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{

		if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
			return E_FAIL;

		m_pShaderCom->Begin(1);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

CLight* CLight::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,void* LightDesc)
{
	CLight* pInstance = new CLight( pDevice,pContext);

	if (FAILED(pInstance->Initialize(LightDesc)))
	{
		MSG_BOX(TEXT("Failed to Created : CLight"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

}
