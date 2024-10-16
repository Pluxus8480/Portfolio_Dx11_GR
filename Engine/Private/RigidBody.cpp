#include "Rigidbody.h"
#include "Transform.h"

CRigidbody::CRigidbody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent { pDevice,pContext }
{
}

CRigidbody::CRigidbody(const CRigidbody& rhs)
	: CComponent{ rhs }
{
}

HRESULT CRigidbody::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRigidbody::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;
	
	m_pOwnerTransform = (CTransform*)pArg;

	Safe_AddRef(m_pOwnerTransform);

	return S_OK;
}

void CRigidbody::Update(_float fTimeDelta)
{
	if(m_bGrab == WIRE_END){
		if (false == m_bGround && true == m_bUseGravity)
			m_vVelocity += m_vGravity * fTimeDelta;

		if (m_fFriction != 0.f)
		{
			PxVec3 vInvVelocity = m_vVelocity;
			vInvVelocity.y = 0.f;
			//D3DXVec3Normalize(&vInvVelocity, &vInvVelocity);
			vInvVelocity *= m_fFriction * fTimeDelta;

			PxVec3 vXZVelocity = m_vVelocity;
			vXZVelocity.y = 0.f;

			if (vXZVelocity.magnitude() < vInvVelocity.magnitude())
			{
				m_vVelocity.x = 0.f;
				m_vVelocity.z = 0.f;
			}
			else
			{
				m_vVelocity -= vInvVelocity;
			}
		}

		if (m_vVelocity.x >= m_fVelocityLimit)
			m_vVelocity.x = m_fVelocityLimit;

		if (m_vVelocity.z >= m_fVelocityLimit)
			m_vVelocity.z = m_fVelocityLimit;

		if (m_vVelocity.x <= -1.f * m_fVelocityLimit)
			m_vVelocity.x = -1.f * m_fVelocityLimit;

		if (m_vVelocity.z <= -1.f * m_fVelocityLimit)
			m_vVelocity.z = -1.f * m_fVelocityLimit;
	}
	else if (m_bGrab == WIRE_LOOP) {
		/*m_vVelocity.x *= (float)pow(1e-5, 1.0f / (4.f / fTimeDelta));
		m_vVelocity.z *= (float)pow(1e-5, 1.0f / (4.f / fTimeDelta));*/
	}
	else if(m_bGrab == WIRE_ATTACH){
		m_vVelocity.y += m_vGravity.y * m_fFriction * 0.3f * fTimeDelta;
		/*m_vVelocity.x *= (float)pow(1e-5, 1.0f / (2.f / fTimeDelta));
		m_vVelocity.z *= (float)pow(1e-5, 1.0f / (2.f / fTimeDelta));*/
	}

	/*_float3 vOwnerPosf = {};
	XMStoreFloat3(&vOwnerPosf, m_pOwnerTransform->Get_State(CTransform::STATE_POSITION));
	
	vOwnerPosf.x += m_vVelocity.x;
	vOwnerPosf.y += m_vVelocity.y;
	vOwnerPosf.z += m_vVelocity.z;

	m_pOwnerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(vOwnerPosf.x, vOwnerPosf.y, vOwnerPosf.z, 1.f));*/
}


void CRigidbody::Set_OnGround()
{
	m_bGround = true;
	m_vVelocity.y = 0.f;
}

CRigidbody* CRigidbody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRigidbody* pInstance = new CRigidbody(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CRigidbody"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent* CRigidbody::Clone(void* pArg)
{
	CRigidbody* pInstance = new CRigidbody(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CRigidbody"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRigidbody::Free()
{
	__super::Free();

	Safe_Release(m_pOwnerTransform);
}
