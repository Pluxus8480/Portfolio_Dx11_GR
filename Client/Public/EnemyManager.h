#pragma once
#include "Client_Defines.h"
#include "GameInstance.h"
#include "Base.h"

BEGIN(Client)

class CEnemyManager : public CBase
{
	DECLARE_SINGLETON(CEnemyManager)

private:
	CEnemyManager();
	virtual ~CEnemyManager() = default;

public:
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void PriorityTick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);
	void ClearEnemyList();
	void EnemyCnt();
	void Render();

	void EnemyPooling(CGameObject* _pEnemy);
	void EnemyPooling(class CLayer* _pLayer);

	CGameObject* Get_NearEnemy(CGameObject* _pMonster, const _float& fMaxRange);

	_uint Get_EnemyCnt() { return m_iEnemyCnt; }

private:
	list<CGameObject*> m_EnemyList;
	_uint m_iEnemyCnt = 0;

private:
	CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	virtual void Free() override;
};

END