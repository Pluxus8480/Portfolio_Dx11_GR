#pragma once

#include "Transform.h"	

/*클라이언트개발자가 실질적으로 생성해낸 객체타입(Player, Monster)의 부모가 되는 클래스이다. */

/* LPDIRECT3DDEVICE9 == ID3D11Device + ID3D11DeviceContext */

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	static const _wstring		m_strTransformTag;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

public:
	class CComponent* Find_Component(const wstring& strComponentTag);
	class CTransform* Get_Transform() { return m_pTransformCom; }
	
	void Set_WorldMatrix(const _float4x4& matWorld) { m_pTransformCom->Set_WorldMatrix(matWorld); }

	string Get_MainTag() { return m_strMaintag; }
	void Set_MainTag(const string& strMainTag) { m_strMaintag = strMainTag; }

	string Get_SubTag() { return m_strSubtag; }
	void Set_SubTag(const string& strSubTag) { m_strSubtag = strSubTag; }

	_bool Get_IsDead() { return m_bIsDead; }
	void Set_IsDead(_bool bIsDead) { m_bIsDead = bIsDead; }
	
	_bool Get_TempDead() { return m_bTempDead; }
	void Set_TempDead(_bool bTempDead) { m_bTempDead = bTempDead; }

	_bool Get_Controlled() { return m_bControlled; }
	void Set_Controlled(_bool bControlled) { m_bControlled = bControlled; }

public:
	virtual void Collision_Enter(CGameObject* pOpponent) {};
	virtual void Collision_RayHit() {};

protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };	

protected:
	string						m_strMaintag = {};
	string						m_strSubtag = {};

	_bool                       m_bControlled = false;

protected:
	class CTransform*			m_pTransformCom = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };
	

protected:
	map<const wstring, class CComponent*>		m_Components;

protected:
	HRESULT Add_Component(_uint iPrototoypeLevelIndex, const wstring& strPrototypeTag, const wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);

private:
	_bool m_bIsDead = false;
	_bool m_bTempDead = false;

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END