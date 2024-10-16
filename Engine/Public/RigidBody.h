#pragma once

#include "Engine_Defines.h"
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRigidbody : public CComponent
{
public:
	enum GRAB_STATE { WIRE_START, WIRE_LOOP, WIRE_ATTACH, WIRE_END };
protected:
	CRigidbody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRigidbody(const CRigidbody& rhs);
	virtual ~CRigidbody() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Update(_float fTimeDelta);

public:
	void Set_Velocity(const PxVec3& fVelocity)
	{
		m_vVelocity = fVelocity;
	}
	void Add_Velocity(const PxVec3& fVelocity)
	{
		m_vVelocity += fVelocity;
	}
	const PxVec3 Get_Velocity() const { return m_vVelocity; }

	void Set_VelocityX(const _float fX) { m_vVelocity.x = fX; }
	void Set_VelocityY(const _float fY) { m_vVelocity.y = fY; }
	void Set_VelocityZ(const _float fZ) { m_vVelocity.z = fZ; }
	void Set_GroundVelocity(const PxVec3& v) { m_vVelocity.x = v.x; m_vVelocity.z = v.z; }

	void Add_VelocityX(const _float fX) { m_vVelocity.x += fX; }
	void Add_VelocityY(const _float fY) { m_vVelocity.y += fY; }
	void Add_VelocityZ(const _float fZ) { m_vVelocity.z += fZ; }
	void Add_GroundVelocity(const PxVec3& v) { m_vVelocity.x += v.x; m_vVelocity.z += v.z; }

	void Set_OnGround();
	void Set_Ground(_bool bGround) { m_bGround = bGround; }
	_bool IsGround() { return m_bGround; }

	void Set_Gravity(const PxVec3& _vGravity) { m_vGravity = _vGravity; }
	void Set_UseGravity(_bool b) { m_bUseGravity = b; }
	_bool Is_UseGravity() { return m_bUseGravity; }

	void Set_Friction(_float fFriction) { m_fFriction = fFriction; }
	
	void Add_Multiplyer(_float fMultiplier) { m_fMultiplier += fMultiplier; }
	void Clear_Multiplyer() { m_fMultiplier = 1.f; }
	//void Set_Mass(_float fMass) { m_fMass = fMass; }
	//_float Get_Mass() { return m_fMass; }

	//void Add_Force(const _float3& _fForce) { m_vForce = _fForce; }

	void Set_Grab(GRAB_STATE bGrab) { m_bGrab = bGrab; }
	GRAB_STATE Get_Grab() { return m_bGrab; }

private:
	CTransform* m_pOwnerTransform = { nullptr };



	PxVec3		m_vVelocity = { 0.f, 0.f, 0.f };
	_float      m_fVelocityLimit = 0.145f;

	PxVec3		m_vGravity = { 0.f, -0.2f, 0.f };

	_bool		m_bGround = { false };
	_bool		m_bUseGravity = true;

	GRAB_STATE		m_bGrab = WIRE_END;

	//_float		m_fMass;
	//_float3		m_vAccel;
	//_float3		m_vForce;

	_float		m_fFriction = 4.f;
	_float		m_fMultiplier = 1.f;

public:
	static CRigidbody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END

