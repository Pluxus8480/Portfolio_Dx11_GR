#pragma once

#include "Base.h"

BEGIN(Engine)

class CGameObject;

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComponent(const CComponent& rhs);
	virtual ~CComponent() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render() { return S_OK; }
	
	HRESULT Set_Owner(CGameObject* pOwner) { 
		
		if(pOwner == nullptr)
			return E_FAIL;

		m_pOwner = pOwner; 
		return S_OK; 
	}


	CGameObject* Get_Owner() { 
		if(m_pOwner == nullptr)
			return nullptr;

		return m_pOwner; 
	}

protected:	
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	_bool						m_isCloned = { false };
	CGameObject*				m_pOwner = nullptr;
	class CGameInstance*		m_pGameInstance = { nullptr };

public:	
	virtual CComponent* Clone(void* pArg) = 0;	
	virtual void Free() override;
};

END