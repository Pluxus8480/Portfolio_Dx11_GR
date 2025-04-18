#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Tick(_float fTimeDelta);
	HRESULT Render();

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	CGameInstance*				m_pGameInstance = { nullptr };
	class CPlayer_Manager*		m_pPlayerManager = { nullptr };
	class CObjPoolManager*		m_pObjPoolManager = { nullptr };
	class CGame_Manager*		m_pGameManager = { nullptr };
	class CEnemyManager*		m_pEnemyManager = { nullptr };

private:
	HRESULT Open_Level(LEVEL eLevelID);
	HRESULT Ready_Prototoype_Component_ForStatic();

public:
	static CMainApp* Create();
	virtual void Free() override;
};

END