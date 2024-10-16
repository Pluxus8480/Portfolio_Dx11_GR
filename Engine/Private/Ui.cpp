#include "Ui.h"
#include "GameInstance.h"

CUi::CUi(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CUi::CUi(const CUi& rhs)
	: CGameObject{ rhs }
	, m_fLifeTime(rhs.m_fLifeTime)
	, m_fActiveTime(rhs.m_fActiveTime)
{
	memcpy(&m_UiDesc, &rhs.m_UiDesc, sizeof(Ui_Pos_Size_Rotation));

}

HRESULT CUi::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUi::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUi::PriorityTick(_float fTimeDelta)
{
}

void CUi::Tick(_float fTimeDelta)
{
}

void CUi::LateTick(_float fTimeDelta)
{
}

HRESULT CUi::Render()
{
	return S_OK;
}

void CUi::Default_Set_LifeTime()
{
}

void CUi::Cal_Life_Blink(_float fTimeDelta)
{
	m_fLifeTime -= fTimeDelta;
	m_fBlink += fTimeDelta;

	if (m_fLifeTime < 0)
	{
		m_bDead = true;
	}

	if (m_fLifeTime < 0.5f)
	{
		m_bBlink = true;
	}
}

bool CUi::Cal_BlinkRender(_float BlinkGap)
{
	if (m_bBlink)
	{
		if (m_fBlink > BlinkGap)
		{
			m_fBlink = 0.f;
			return true;
		}
		return false;
	}

	return true;
}

void CUi::Initialize_Set_ActiveTime()
{
}

void CUi::Initialize_Set_Size()
{
}

void CUi::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}


CUi* CUi::Clone(void* pArg)
{
	return nullptr;
}
