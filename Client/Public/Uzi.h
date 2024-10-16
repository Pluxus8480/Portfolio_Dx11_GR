#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "PhysXCollider.h"

/*
Animation Index
0 : die
1 : Idle
2 : attack	
3 : reload
*/

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CUzi final : public CGameObject
{
public:
	enum UZI_STATE { UZI_IDLE, UZI_SEARCH, UZI_ATTACK, UZI_DIE, UZI_RELOAD ,PISTOLGUNNER_END };

private:
	CUzi(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUzi(const CUzi& rhs);
	virtual ~CUzi() = default;

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

	HRESULT Model_Rendering(_int iMeshNum);

private:
	void Part_Manager(_float fTimeDelta);

private:
	void State_Machine(_float fTimeDelta);
	void State_Idle(_float fTimeDelta);
	void State_Search(_float fTimeDelta);
	void State_Attack(_float fTimeDelta);
	void State_Reload(_float fTimeDelta);
	void State_Die(_float fTimeDelta);
	void Soft_Animation_Change(_uint _iAnimationNum, _bool _bLoop, _float _bSpeedMultiply = 1.f);
	void Hard_Animation_Change(_uint _iAnimationNum, _bool _bLoop, _float _bSpeedMultiply = 1.f);

	void Get_DeathCheck();

private:
	vector<CGameObject*> m_vecParts;

public:
	void Set_State(UZI_STATE eUziState) { m_eUziState = eUziState; }
	UZI_STATE Get_State() { return m_eUziState; }

private:
	UZI_STATE m_eUziState = UZI_IDLE;

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CPhysXCollider* m_pPhysXCollider = { nullptr };

	_bool m_bDeathCheck = false;
	_bool m_bFirstAttack = true;
	int m_iBulletCnt = 10;
	int m_iBulletMaxCnt = 10;
public:
	static CUzi* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END