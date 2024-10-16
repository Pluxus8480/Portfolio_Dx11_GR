#pragma once
#include "EnemyManager.h"
#include "Bullet.h"
#include "Beam.h"
#include "ShockWave.h"
#include "Layer.h"

IMPLEMENT_SINGLETON(CEnemyManager)

CEnemyManager::CEnemyManager()
{

}

void CEnemyManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pGameInstance = CGameInstance::Get_Instance();
    m_pDevice = pDevice;
    m_pContext = pContext;
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

void CEnemyManager::PriorityTick(_float fTimeDelta)
{

}

void CEnemyManager::Tick(_float fTimeDelta)
{
    EnemyCnt();
}

void CEnemyManager::LateTick(_float fTimeDelta)
{

}

void CEnemyManager::ClearEnemyList()
{
    for (auto& pEnemy : m_EnemyList)
    {
		Safe_Release(pEnemy);
	}

	m_EnemyList.clear();
}

void CEnemyManager::EnemyCnt()
{
    m_iEnemyCnt = 0;

    for (auto& pEnemy : m_EnemyList)
    {
        if(pEnemy->Get_TempDead() == false) m_iEnemyCnt++; 
    }

}

void CEnemyManager::Render()
{

}

void CEnemyManager::EnemyPooling(CGameObject* _pEnemy)
{
	m_EnemyList.push_back(_pEnemy);
}

void CEnemyManager::EnemyPooling(CLayer* _pLayer)
{
    _uint iEnemyCnt = 0;

    m_EnemyList.clear();

    while ((_pLayer->CLayer::Find_GameObject(iEnemyCnt) != nullptr))
    {
		m_EnemyList.push_back(_pLayer->CLayer::Find_GameObject(iEnemyCnt));
		++iEnemyCnt;
	}

    if(iEnemyCnt == 0)
		assert(false);
}

CGameObject* CEnemyManager::Get_NearEnemy(CGameObject* _pMonster, const _float& fMaxRange)
{
    _float fMinDistance = fMaxRange;
	CGameObject* pNearEnemy = nullptr;

    for (auto& pEnemy : m_EnemyList)
    {
        if(pEnemy == nullptr) continue;
        if(pEnemy == _pMonster) continue;
		_float fDistance = XMVectorGetX(XMVector4Length((pEnemy->Get_Transform()->Get_State(CTransform::STATE_POSITION)) - (_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION))));
        if (fDistance < fMinDistance)
        {
			fMinDistance = fDistance;
			pNearEnemy = pEnemy;
		}
    }

    if(pNearEnemy == nullptr)
		return nullptr;

    return pNearEnemy;
}

void CEnemyManager::Free()
{
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
}
