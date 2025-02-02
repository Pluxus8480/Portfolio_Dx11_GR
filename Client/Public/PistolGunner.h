#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "PhysXCollider.h"

/*
Animation Index
0 : die
1 : die
2 : die
3 : Attack
4 : Idle
*/


BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CPistolGunner final : public CGameObject
{
public:
	enum PISTOLGUNNER_STATE { PISTOLGUNNER_IDLE, PISTOLGUNNER_SEARCH, PISTOLGUNNER_ATTACK, PISTOLGUNNER_DIE, PISTOLGUNNER_END };

private:
	CPistolGunner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPistolGunner(const CPistolGunner& rhs);
	virtual ~CPistolGunner() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Collision_Enter(CGameObject* pOpponent) override;
	virtual void Collision_RayHit() override;

	PISTOLGUNNER_STATE Get_State() { return m_ePistolGunnerState; }
	void Set_State(PISTOLGUNNER_STATE ePistolGunnerState) { m_ePistolGunnerState = ePistolGunnerState; }

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	HRESULT Model_Rendering(_int iMeshNum);

private:
	void Part_Manager(_float fTimeDelta);

private:
	void State_Machine(_float fTimeDelta);
	void State_Idle(_float fTimeDelta);
	void State_Search(_float fTimeDelta);
	void State_Attack(_float fTimeDelta);
	void State_Die(_float fTimeDelta);
	void Soft_Animation_Change(_uint _iAnimationNum, _bool _bLoop, _float _bSpeedMultiply = 1.f);
	void Hard_Animation_Change(_uint _iAnimationNum, _bool _bLoop, _float _bSpeedMultiply = 1.f);

	void Get_DeathCheck();

private:
	vector<CGameObject*> m_vecParts;

private:
	PISTOLGUNNER_STATE m_ePistolGunnerState = PISTOLGUNNER_IDLE;

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CPhysXCollider* m_pPhysXCollider = { nullptr };
	CGameObject* m_pTarget = { nullptr };

	_bool m_bDeathCheck = false;
	_bool m_bFirstAttack = true;
public:
	static CPistolGunner* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END