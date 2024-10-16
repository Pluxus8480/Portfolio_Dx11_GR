#include "Core_Camera.h"
#include "GameInstance.h"

CCoreCamera::CCoreCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CCoreCamera::CCoreCamera(const CCoreCamera& rhs)
	: CGameObject{ rhs }
{

}

HRESULT CCoreCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCoreCamera::Initialize(void* pArg)
{
	CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(pArg);

	m_CameraDesc.fFovy = pDesc->fFovy;
	m_CameraDesc.fAspect = pDesc->fAspect;
	m_CameraDesc.fNear = pDesc->fNear;
	m_CameraDesc.fFar = pDesc->fFar;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pDesc->vEye));
	m_pTransformCom->LookAt(XMLoadFloat4(&pDesc->vAt));

	return S_OK;
}

void CCoreCamera::PriorityTick(_float fTimeDelta)
{
	/* 카메라 월드행렬의 역행렬을 뷰행렬로서 파이파이라인에 저장해주낟. */
	//m_pGameInstance->Set_Transform(CCamera_Manager::D3DTS_VIEW, XMMatrixInverse(nullptr, m_pTransformCom->Get_WorldMatrix()));
	//m_pGameInstance->Set_Transform(CCamera_Manager::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_CameraDesc.fFovy, m_CameraDesc.fAspect, m_CameraDesc.fNear, m_CameraDesc.fFar));
}

void CCoreCamera::Tick(_float fTimeDelta)
{

}

void CCoreCamera::LateTick(_float fTimeDelta)
{
}

void CCoreCamera::Free()
{
	__super::Free();
}