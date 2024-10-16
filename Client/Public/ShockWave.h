#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "ContainedOBJ.h"
#include "PhysXCollider.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CShockWave final : public CContainedObject
{
public:
	enum SHOCKWAVE_STATE { SHOCKWAVE_FIRE, SHOCKWAVE_HIT, SHOCKWAVE_SLEEP, SHOCKWAVE_END };

	typedef struct ShockDesc {
		_float4x4 matWorld = {};
		_vector vPosition = {};
		_vector vDirection = {};
		_float fSpeed = 0.f;
	}SHOCKWAVE_DESC;

private:
	CShockWave(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShockWave(const CShockWave& rhs);
	virtual ~CShockWave() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Activate(void* pArg = nullptr) override;
	virtual void DeActivate() override;
public:
	virtual void Collision_Enter(CGameObject* pOpponent) override;

	void Set_Directional_Rotate();

	void State_Machine(_float fTimeDelta);
	void State_Fire(_float fTimeDelta);
	void State_Hit(_float fTimeDelta);
	void State_Sleep(_float fTimeDelta);

	void Set_Target(CGameObject* pTarget) { m_pTarget = pTarget; }

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	void Get_DeathCheck();

public:
	void Set_State(SHOCKWAVE_STATE eBulletState) { m_eShockWaveState = eBulletState; }
	SHOCKWAVE_STATE Get_State() { return m_eShockWaveState; }

private:
	SHOCKWAVE_STATE m_eShockWaveState = SHOCKWAVE_END;

	bool bReflect = false;

	CGameObject* m_pTarget = { nullptr };
	_vector m_vDirection = {};
	_vector m_vNormalDirection = {};
	_vector m_vTargetPosition = {};

	_float m_fLifeTime = 0.f;
	_float m_fMaxLifeTime = 5.f;

	_float m_fSpeed = 5.f;
private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

public:
	static CShockWave* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END