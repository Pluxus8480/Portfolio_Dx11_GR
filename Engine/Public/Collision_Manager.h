#pragma once

#include "Base.h"
#include "Engine_Enum.h"

BEGIN(Engine)

class CCollision_Manager : public CBase
{
public:
	enum COLLIDER_TYPE { COLLISION_PLAYER, COLLISION_MONSTER, COLLISION_PROJECTILE, COLLISION_END };

private:
	CCollision_Manager();
	virtual ~CCollision_Manager() = default;


public:
	void Initialize();
	void Tick(_float fTimeDelta);
	void Clear();
	void Render();

	void CollisionClear();

	void CollisionDeleteCheck();

	void Collision_Check();

	void Add_Collider(COLLIDER_TYPE _colliderType, class CCollider* pCollider);

	void CollisionAccessBoth(COLLIDER_TYPE eDst, COLLIDER_TYPE eSrc);
	void CollisionAccess(COLLIDER_TYPE eDst, COLLIDER_TYPE eSrc);

	void CollisionDenyBoth(COLLIDER_TYPE eDst, COLLIDER_TYPE eSrc);
	void CollisionDeny(COLLIDER_TYPE eDst, COLLIDER_TYPE eSrc);

	void CollisionFlagClear();
public:
	static CCollision_Manager* Create();
	virtual void Free() override;

private:
	bool CollisionAccessFlag[_uint(COLLISION_END)][_uint(COLLISION_END)] = {};

	list<class CCollider*> m_ListCollider[_uint(COLLISION_END)];

};

END