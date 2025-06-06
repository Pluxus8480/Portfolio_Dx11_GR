#include "..\Public\Bounding_OBB.h"

#include "DebugDraw.h"

#include "Bounding_AABB.h"
#include "Bounding_Sphere.h"

CBounding_OBB::CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding{ pDevice, pContext }
{
}

HRESULT CBounding_OBB::Initialize(CBounding::BOUNDING_DESC* pBoundingDesc)
{
	if (nullptr == pBoundingDesc)
		return E_FAIL;

	BOUNDING_OBB_DESC* pDesc = static_cast<BOUNDING_OBB_DESC*>(pBoundingDesc);

	_float4			vRotation = {};
	XMStoreFloat4(&vRotation, XMQuaternionRotationRollPitchYaw(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z));

	m_pBoundingDesc_Original = new BoundingOrientedBox(pDesc->vCenter, pDesc->vExtents, vRotation);
	m_pBoundingDesc = new BoundingOrientedBox(*m_pBoundingDesc_Original);

	return S_OK;
}
void CBounding_OBB::Update(_fmatrix WorldMatrix)
{
	m_pBoundingDesc_Original->Transform(*m_pBoundingDesc, WorldMatrix);
}
_bool CBounding_OBB::Intersect(CCollider::TYPE eColliderType, CBounding* pTargetBounding)
{
	m_isCollision = false;

	switch (eColliderType)
	{
	case CCollider::TYPE_AABB:
		m_isCollision = m_pBoundingDesc->Intersects(*(dynamic_cast<CBounding_AABB*>(pTargetBounding)->Get_Desc()));
		break;
	case CCollider::TYPE_OBB:
		m_isCollision = m_pBoundingDesc->Intersects(*(dynamic_cast<CBounding_OBB*>(pTargetBounding)->Get_Desc()));
		break;
	case CCollider::TYPE_SPHERE:
		m_isCollision = m_pBoundingDesc->Intersects(*(dynamic_cast<CBounding_Sphere*>(pTargetBounding)->Get_Desc()));
		break;
	}

	return m_isCollision;
}
_bool CBounding_OBB::RayIntersect(FXMVECTOR Origin, FXMVECTOR Direction, float& Dist)
{
	m_isRayCast = false;

	m_isRayCast = m_pBoundingDesc->Intersects(Origin, Direction, Dist);

	return m_isRayCast;
}
#ifdef _DEBUG
HRESULT CBounding_OBB::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	DX::Draw(pBatch, *m_pBoundingDesc, false == m_isCollision ? XMVectorSet(0.f, 1.f, 0.f, 1.f) : XMVectorSet(1.f, 0.f, 0.f, 1.f));

	return S_OK;
}
#endif

_bool CBounding_OBB::Intersect_ToOBB(const CBounding_OBB* pTargetBounding)
{
	OBB_DESC		OBBDesc[2];

	m_isCollision = false;

	OBBDesc[0] = Compute_OBB();
	OBBDesc[1] = pTargetBounding->Compute_OBB();

	_float		fDistance[3] = {};

	for (size_t i = 0; i < 2; i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			/* 센터와 센터를 잇는 벡터를 비교판단하고자하는 축에 투영시킨 길이. */
			fDistance[0] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenter) - XMLoadFloat3(&OBBDesc[0].vCenter), XMLoadFloat3(&OBBDesc[i].vAlignDir[j]))));

			fDistance[1] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterDir[0]), XMLoadFloat3(&OBBDesc[i].vAlignDir[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterDir[1]), XMLoadFloat3(&OBBDesc[i].vAlignDir[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterDir[2]), XMLoadFloat3(&OBBDesc[i].vAlignDir[j]))));

			fDistance[2] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterDir[0]), XMLoadFloat3(&OBBDesc[i].vAlignDir[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterDir[1]), XMLoadFloat3(&OBBDesc[i].vAlignDir[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterDir[2]), XMLoadFloat3(&OBBDesc[i].vAlignDir[j]))));

			if (fDistance[0] > fDistance[1] + fDistance[2])
				return m_isCollision;
		}
	}

	return m_isCollision = true;
}

CBounding_OBB::OBB_DESC CBounding_OBB::Compute_OBB() const
{
	OBB_DESC			OBBDesc{};

	_float3				vCorners[8] = {};

	m_pBoundingDesc->GetCorners(vCorners);

	OBBDesc.vCenter = m_pBoundingDesc->Center;
	XMStoreFloat3(&OBBDesc.vCenterDir[0], (XMLoadFloat3(&vCorners[5]) - XMLoadFloat3(&vCorners[4])) * 0.5f);
	XMStoreFloat3(&OBBDesc.vCenterDir[1], (XMLoadFloat3(&vCorners[7]) - XMLoadFloat3(&vCorners[4])) * 0.5f);
	XMStoreFloat3(&OBBDesc.vCenterDir[2], (XMLoadFloat3(&vCorners[0]) - XMLoadFloat3(&vCorners[4])) * 0.5f);

	for (size_t i = 0; i < 3; i++)
		XMStoreFloat3(&OBBDesc.vAlignDir[i], XMVector3Normalize(XMLoadFloat3(&OBBDesc.vCenterDir[i])));

	return OBBDesc;
}

CBounding_OBB* CBounding_OBB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pBoundingDesc)
{
	CBounding_OBB* pInstance = new CBounding_OBB(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pBoundingDesc)))
	{
		MSG_BOX(TEXT("Failed to Created : CBounding_OBB"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_OBB::Free()
{
	__super::Free();

	Safe_Delete(m_pBoundingDesc_Original);
	Safe_Delete(m_pBoundingDesc);
}
