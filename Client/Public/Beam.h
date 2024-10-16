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

class CBeam final : public CContainedObject
{
public:
	enum BULLET_STATE { BULLET_FIRE, BULLET_HIT, BULLET_SLEEP, BULLET_END };

	typedef struct BulletDesc {
		_vector vPosition = {};
		_vector vDirection = {};
		_float fSpeed = 0.f;
	}BULLET_DESC;

private:
	CBeam(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBeam(const CBeam& rhs);
	virtual ~CBeam() = default;

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

	_float m_fLifeTime = 0.f;
	_float m_fMaxLifeTime = 3.f;

	_float m_fSpeed = 5.f;
private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	/*CPhysXCollider* m_pPhysXCollider = { nullptr };*/

public:
	static CBeam* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END