#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "PhysXCollider.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CSamurai final : public CGameObject
{
public:
	enum SAMURAI_STATE { SAMURAI_IDLE, SAMURAI_SEARCH, SAMURAI_GUARD, SAMURAI_DASH, SAMURAI_STUN, SAMURAI_ATTACK, SAMURAI_DIE, SAMURAI_END };
	enum DASH_STATE { DASH_START, DASH_LOOP, DASH_END };

private:
	CSamurai(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSamurai(const CSamurai& rhs);
	virtual ~CSamurai() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Collision_Enter(CGameObject* pOpponent) override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	void State_Machine(_float fTimeDelta);
	void State_Idle(_float fTimeDelta);
	void State_Search(_float fTimeDelta);
	void State_Attack(_float fTimeDelta);
	void State_Die(_float fTimeDelta);
	void State_Guard(_float fTimeDelta);
	void State_Dash(_float fTimeDelta);
	void State_Stun(_float fTimeDelta);
	void Soft_Animation_Change(_uint _iAnimationNum, _bool _bLoop, _float _bSpeedMultiply = 1.f);
	void Hard_Animation_Change(_uint _iAnimationNum, _bool _bLoop, _float _bSpeedMultiply = 1.f);

	void Get_DeathCheck();

private:
	vector<CGameObject*> m_vecParts;

public:
	void Set_State(SAMURAI_STATE eSAMURAIState) { m_eSamuraiState = eSAMURAIState; }
	SAMURAI_STATE Get_State() { return m_eSamuraiState; }

private:
	SAMURAI_STATE	m_eSamuraiState = SAMURAI_IDLE;
	DASH_STATE		m_eDashState = DASH_END;

	PxVec3 m_vForwardPx = {};

	_float m_fDashTime = 0.f;
	_float m_fDashTimeMax = 0.7f;

	_float m_fParryTime = 0.f;
	_float m_fParryTimeMax = 0.5f;

	_bool m_bDeathCheck = false;
	_bool m_bFirstAttack = true;

	PxExtendedVec3 m_vInitPos = {};
private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CPhysXCollider* m_pPhysXCollider = { nullptr };

public:
	static CSamurai* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END