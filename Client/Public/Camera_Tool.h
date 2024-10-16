#pragma once

#include "Client_Defines.h"
#include "Core_Camera.h"
#include "PhysXCollider.h"
#include "RigidBody.h"

BEGIN(Client)

class CCamera_Tool final : public CCoreCamera
{
private:
	CCamera_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Tool(const CCamera_Tool& rhs);
	virtual ~CCamera_Tool() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	bool Get_Rigid_Grounded() { return m_pRigidbody->IsGround(); }
private:
	void Camera_Effect(_float fTimeDelta);

	void Camera_Roll(_float fTimeDelta);
	void Camera_HeadTilt(_float fTimeDelta);
	void Camera_HeadTilt_Reset(_float fTimeDelta);
	void Camera_Shake(_float fTimeDelta);

private:
	_bool m_bCameraRoll = false;

	_int   m_iRollDirection = 0;
	_float m_fRollAngle = 0.f;
	_float m_fRollAngleLimit = 0.3f;


	_float m_fShakeTime = 0.f;
	_float m_fShakePower = 0.f;

	virtual void Key_Input(_float fTimeDelta) override;
	void Fall_Check();

private:
	_float				m_fMouseSensor = {};

	_float m_fVerticalAngle = 0.f;
	_float m_fVerticalAngleLimit = 80.f;

	CPhysXCollider* m_pPhysXCollider = { nullptr };
	CRigidbody* m_pRigidbody = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

public:
	static CCamera_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END