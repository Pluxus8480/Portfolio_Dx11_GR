#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "PhysXCollider.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CBoss final : public CGameObject
{
public:
	enum BOSS_STATE { BOSS_IDLE, BOSS_SEARCH, BOSS_AIRSTRIKE , BOSS_FLY ,BOSS_GUARD, BOSS_DASH, BOSS_STUN, BOSS_PATTERN, BOSS_ATTACK, BOSS_DIE, BOSS_END };
	enum BOSS_PATTERN_STATE { DASH_PATTERN, BULLET_PATTERN, SLASH_PATTERN, EXTEND_PATTERN, PATTERN_END };
	enum DASH_STATE { DASH_START, DASH_LOOP, DASH_END };
	enum FLY_STATE { FLY_START, FLY_LOOP, FLY_END };

private:
	CBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss(const CBoss& rhs);
	virtual ~CBoss() = default;

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
	void State_Pattern(_float fTimeDelta);
	void State_AirStrike(_float fTimeDelta);
	void State_Fly(_float fTimeDelta);
	void Soft_Animation_Change(_uint _iAnimationNum, _bool _bLoop, _float _bSpeedMultiply = 1.f);
	void Hard_Animation_Change(_uint _iAnimationNum, _bool _bLoop, _float _bSpeedMultiply = 1.f);
	
	void Pattern_Dash(_float fTimeDelta);
	void Pattern_Bullet(_float fTimeDelta);
	void Pattern_Slash(_float fTimeDelta);
	void Pattern_Extend(_float fTimeDelta);


	void Get_DeathCheck();

private:
	vector<CGameObject*> m_vecParts;

public:
	void Set_State(BOSS_STATE eSAMURAIState) { m_eBossState = eSAMURAIState; }
	BOSS_STATE Get_State() { return m_eBossState; }

private:
	BOSS_STATE			m_eBossState = BOSS_IDLE;
	DASH_STATE			m_eDashState = DASH_END;
	BOSS_PATTERN_STATE	m_eBossPattern = DASH_PATTERN;
	
	PxVec3 m_vForwardPx = {};

	_float m_fDashTime = 0.f;
	_float m_fDashTimeMax = 0.7f;

	_float m_fParryTime = 0.f;
	_float m_fParryTimeMax = 0.5f;

	_bool m_bDeathCheck = false;

	_uint m_iPatternPhase = 0;

	_int m_iHp = 200;

	_float m_fShootTime = 1.f;
	_float m_fShootTimeMax = 1.f;

	_bool m_bDash = false;

	_uint m_iRandPos = 0;

	vector<_float3> m_vDashPos;
private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CPhysXCollider* m_pPhysXCollider = { nullptr };

public:
	static CBoss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END