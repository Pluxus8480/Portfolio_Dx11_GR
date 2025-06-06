#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_OBB final : public CBounding
{
public:
	typedef struct obbDesc: public CBounding::BOUNDING_DESC
	{
		_float3		vExtents;
		_float3		vRotation; /* x축기준회전, y축기준회전, z축기준회전 */
	}BOUNDING_OBB_DESC;

	typedef struct
	{
		_float3		vCenter;
		_float3		vCenterDir[3];
		_float3		vAlignDir[3];
	}OBB_DESC;
private:
	CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_OBB() = default;

public:
	const BoundingOrientedBox* Get_Desc() const {
		return m_pBoundingDesc;
	}

public:
	virtual HRESULT Initialize(CBounding::BOUNDING_DESC* pBoundingDesc) override;
	virtual void Update(_fmatrix WorldMatrix) override;
	virtual _bool Intersect(CCollider::TYPE eColliderType, CBounding* pTargetBounding) override;
	virtual _bool RayIntersect(FXMVECTOR Origin, FXMVECTOR Direction, float& Dist) override;

#ifdef _DEBUG
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;
#endif
private:
	BoundingOrientedBox* m_pBoundingDesc_Original = { nullptr };
	BoundingOrientedBox* m_pBoundingDesc = { nullptr };

private:
	_bool Intersect_ToOBB(const CBounding_OBB* pTargetBounding);
	OBB_DESC Compute_OBB() const;

public:
	static CBounding_OBB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pBoundingDesc);
	virtual void Free() override;
};

END