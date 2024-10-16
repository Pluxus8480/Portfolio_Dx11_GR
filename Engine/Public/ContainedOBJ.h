#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CContainedObject abstract : public CGameObject
{
public:
	enum CONTAIN_STATE { CONTAIN_SLEEP, CONTAIN_ACTIVE, CONTAIN_END};

protected:
	CContainedObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CContainedObject(const CContainedObject& rhs);
	virtual ~CContainedObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Activate(void* pArg = nullptr) PURE;
	virtual void DeActivate() PURE;

	CONTAIN_STATE Get_ContainState() const { return m_eContainState; }
	void Set_ContainState(CONTAIN_STATE eState) { m_eContainState = eState; }

protected:
	CONTAIN_STATE m_eContainState = CONTAIN_SLEEP;

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END