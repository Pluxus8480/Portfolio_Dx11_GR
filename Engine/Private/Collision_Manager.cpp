#pragma once
#include "Collision_Manager.h"
#include "GameInstance.h"
#include "Collider.h"
#include "GameObject.h"

CCollision_Manager::CCollision_Manager()
{

}

void CCollision_Manager::Initialize()
{
	CollisionFlagClear();

	CollisionAccessBoth(COLLISION_PLAYER, COLLISION_MONSTER);
	CollisionAccessBoth(COLLISION_PLAYER, COLLISION_PROJECTILE);
	CollisionAccessBoth(COLLISION_MONSTER, COLLISION_PROJECTILE);
}

void CCollision_Manager::Tick(_float fTimeDelta)
{
	Collision_Check();
}

void CCollision_Manager::Clear()
{

}

void CCollision_Manager::Render()
{

}

void CCollision_Manager::CollisionClear()
{

}

void CCollision_Manager::CollisionDeleteCheck()
{
	for (_uint i = 0; i < _uint(COLLISION_END); ++i)
	{
		for (auto iter = m_ListCollider[i].begin(); iter != m_ListCollider[i].end();)
		{
			if ((*iter)->Get_Owner()->Get_IsDead() == true || (*iter)->Get_Owner() == nullptr)
			{
				Safe_Release(*iter);
				iter = m_ListCollider[i].erase(iter);
			}
			else
				++iter;
		}
	}
}

void CCollision_Manager::Collision_Check()
{
	_float RayDistance = 0.f;
	for (_uint i = 0; i < _uint(COLLISION_END); ++i)
	{
		if(m_ListCollider[i].empty())
			continue;

		for (auto& pDst : m_ListCollider[i])
		{
			if (GAMEINSTANCE->Get_CurCamera_Transform() != nullptr) {
					if (pDst->RayIntersect(GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_POSITION), GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_LOOK), RayDistance))
					{
						pDst->Get_Owner()->Collision_RayHit();
					}
			}
		}


		for (_uint j = i + 1; j < _uint(COLLISION_END); ++j)
		{
			if (CollisionAccessFlag[i][j])
			{
				for (auto& pDst : m_ListCollider[i])
				{
					for (auto& pSrc : m_ListCollider[j])
					{
						if (pDst->Get_Owner() == nullptr || pSrc->Get_Owner() == nullptr)
							continue;

						if (pDst->Get_Owner() == pSrc->Get_Owner())
							continue;

							if (pSrc->Intersect(pDst))
								pDst->Get_Owner()->Collision_Enter(pSrc->Get_Owner());

							if (pDst->Intersect(pSrc))
								pSrc->Get_Owner()->Collision_Enter(pDst->Get_Owner());


					}
				}
			}
		}
	}
}

void CCollision_Manager::Add_Collider(COLLIDER_TYPE _colliderType, CCollider* pCollider)
{
	m_ListCollider[_uint(_colliderType)].emplace_back(pCollider);
}

void CCollision_Manager::CollisionAccessBoth(COLLIDER_TYPE eDst, COLLIDER_TYPE eSrc)
{
	CollisionAccessFlag[eDst][eSrc] = true;
	CollisionAccessFlag[eSrc][eDst] = true;
}

void CCollision_Manager::CollisionAccess(COLLIDER_TYPE eDst, COLLIDER_TYPE eSrc)
{
	CollisionAccessFlag[eDst][eSrc] = true;
}

void CCollision_Manager::CollisionDenyBoth(COLLIDER_TYPE eDst, COLLIDER_TYPE eSrc)
{
	CollisionAccessFlag[eDst][eSrc] = false;
	CollisionAccessFlag[eSrc][eDst] = false;
}

void CCollision_Manager::CollisionDeny(COLLIDER_TYPE eDst, COLLIDER_TYPE eSrc)
{
	CollisionAccessFlag[eDst][eSrc] = false;
}

void CCollision_Manager::CollisionFlagClear()
{
	std::fill(&CollisionAccessFlag[0][0], &CollisionAccessFlag[COLLISION_END][COLLISION_END], false);
}

CCollision_Manager* CCollision_Manager::Create()
{
	CCollision_Manager* pInstance = new CCollision_Manager();

	pInstance->Initialize();

	return pInstance;
}

void CCollision_Manager::Free()
{
	__super::Free();
}
