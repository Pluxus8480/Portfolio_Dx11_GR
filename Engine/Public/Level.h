#pragma once

#include "Base.h"

/* 클라이언트에서 실제 사용하기위해 만든 레벨들의 부모가 되는 클래스이다. */

BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel() = default;

public:
	_uint Get_LevelID() const {
		return m_iLevelID;
	}

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };
	_uint						m_iLevelID = { 0 };

public:
	virtual void Free() override;
};

END