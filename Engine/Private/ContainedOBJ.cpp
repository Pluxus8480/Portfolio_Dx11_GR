#include "ContainedOBJ.h"


CContainedObject::CContainedObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CContainedObject::CContainedObject(const CContainedObject& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CContainedObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CContainedObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CContainedObject::PriorityTick(_float fTimeDelta)
{
}

void CContainedObject::Tick(_float fTimeDelta)
{
}

void CContainedObject::LateTick(_float fTimeDelta)
{
}

HRESULT CContainedObject::Render()
{
	return S_OK;
}

void CContainedObject::Free()
{
	__super::Free();
}