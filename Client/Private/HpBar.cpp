#include "HpBar.h"
#include "GameInstance.h"

CHpBar::CHpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUi{ pDevice, pContext }
{
}

CHpBar::CHpBar(const CHpBar& rhs)
	: CUi{ rhs }
{
}

HRESULT CHpBar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Initialize(nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHpBar::Initialize(void* pArg)
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

void CHpBar::PriorityTick(_float fTimeDelta)
{
}

void CHpBar::Tick(_float fTimeDelta)
{
}

void CHpBar::LateTick(_float fTimeDelta)
{
	if (Get_Active())
		m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_UI, this);
}

HRESULT CHpBar::Render()
{

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Float("g_fRate", m_pGameInstance->Random_Float(0.f,100.f))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(9)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	/*pBlendState->Release();
	pBlendState = nullptr;*/

	return S_OK;
}

void CHpBar::Initialize_Set_Scale_Pos_Rotation(void* pArg)
{

	m_fSizeX = 980.f;
	m_fSizeY = 40.f;

	Set_Pos({ 0,250.f });
	Set_Divide(1.0f);

	m_fScale = { m_fSizeX / Get_Divide() , m_fSizeY / Get_Divide(), 1.f };
	m_fRotation = { 0.f, 0.f, 0.f };

	m_pTransformCom->Set_Scale(m_fScale);
	m_pTransformCom->Rotation_XYZ(m_fRotation);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 250.f, 0.f, 1.f));

}

HRESULT CHpBar::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Hp_Bar"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUi* CHpBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHpBar* pInstance = new CHpBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CHpBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CUi* CHpBar::Clone(void* pArg)
{
	CHpBar* pInstance = new CHpBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CHpBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHpBar::Free()
{
	__super::Free();
}
