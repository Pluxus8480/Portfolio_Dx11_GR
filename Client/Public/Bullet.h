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

class CBullet final : public CContainedObject
{
public:
	enum BULLET_STATE { BULLET_FIRE, BULLET_HIT, BULLET_SLEEP, BULLET_END };

	typedef struct BulletDesc {
		_vector vPosition = {};
		_vector vDirection = {};
		CGameObject* pTarget = nullptr;
		_float fSpeed = 0.f;
		_bool  bReflect = false;
		_bool  bColor = false;
	}BULLET_DESC;

private:
	CBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBullet(const CBullet& rhs);
	virtual ~CBullet() = default;

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
	void Set_State(BULLET_STATE eBulletState) { m_eBulletState = eBulletState; }
	BULLET_STATE Get_State() { return m_eBulletState; }

private:
	BULLET_STATE m_eBulletState = BULLET_END;

	bool bReflect = false;

	CGameObject* m_pTarget = { nullptr };
	_vector m_vDirection = {};
	_vector m_vNormalDirection = {};
	_vector m_vTargetPosition = {};

	_float m_fLifeTime = 0.f;
	_float m_fMaxLifeTime = 3.f;

	_float m_fSpeed = 5.f;
	_bool  m_bColor = false;

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

public:
	static CBullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END