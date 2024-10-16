#pragma once
#include "Component.h"
#include "GameInstance.h"
#include "PhysX_Hitreport.h"

BEGIN(Engine)
class CTransform;
class CVIBuffer;

class ENGINE_DLL CPhysXCollider final : public CComponent
{

	enum COLLIDER_TYPE {
		CCT,
		RIGIDBODY,
		COLLIDER_NONE
	};

public:
	CPhysXCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPhysXCollider(const CPhysXCollider& rhs);

	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	//_vector	Get_Position();
	//_vector	Get_Quaternion();
	//_vector	Get_Velocity();

	//_vector	Get_AngularVelocity();
	//_vector	Get_LinearVelocity();
	//_matrix Get_WorldMatrix();

	/*_uint	Get_PColliderIndex() const { return m_iColliderIndex; }*/

	PHYSXCOLLIDERDESC Get_ColliderDesc() const { return m_PhysXColliderDesc; }


	//HRESULT Set_Position(_vector _vPos, _vector _vQuaternion);
	//HRESULT Set_Position(_vector _vPos);
	//void	Set_MaxLinearVelocity(_vector vMaxVelocity) { m_vMaxVelocity = vMaxVelocity; }
	//void	Set_Scale(_vector vScale);
	//void	Set_ActorFlag(PxActorFlag::Enum eFlag, _bool bState);

	//void	Set_PickState(_bool bState) { m_bPickState = bState; }
	//void	Set_Pickable(_bool bState) { m_bPickable = bState; }
	//void	Set_YFixed() { m_bYFixed = true; }

	//void	Set_ConvexMesh(_uint iIndex, PxConvexMesh* pMesh) { m_ConvexMeshes[iIndex] = pMesh; }

	//_bool	Is_Pickable() { return m_bPickable; }
	//_bool	Is_Picked() { return m_bPickState; }
	//_bool	Is_YFixed() { return m_bYFixed; }

	//void	Delete_Collider();
	//void	Create_Collider();

public:
	void	Synchronize_Transform(CTransform* pTransform, _fvector In_vOffset = { 0.f, 0.f, 0.f });
	void	Synchronize_Transform_Position(CTransform* pTransform);
	void	Synchronize_Transform_Rotation(CTransform* pTransform);

	void	checkCollisionFlags(PxU32 collisionFlags);
public:
	PxTriangleMesh* CreateTriangleMesh(PxVec3* vertices, PxU32* indices, PxU32 vertexCount, PxU32 triangleCount);
	PxRigidStatic* createStaticMeshActor(PxTriangleMesh* triangleMesh, const char* PhysXTag);
	PxRigidDynamic* createDynamicMeshActor(PxTriangleMesh* triangleMesh, const char* PhysXTag);

	PxRigidDynamic* createDynamicActor(const char* PhysXTag);

	PxController* createCharacterController(PxControllerManager* manager, const PxVec3& position);

	PxRigidDynamic* Get_Dynamic_RigidBody() { return m_pRigidDynamic; }
	PxController* Get_Controller() { return m_pController; }

    _bool Get_CollisionDown() { return m_bCollisionDown; }
	_bool Get_CollisionUp() { return m_bCollisionUp; }
	_bool Get_CollisionLeft() { return m_bCollisionLeft; }
	_bool Get_CollisionRight() { return m_bCollisionRight; }
	_bool Get_CollisionSides() { return m_bCollisionSides; }
	_bool Get_Collision() { return m_bCollision; }

	void Set_LookVector(PxVec3 vLook) { m_pHitReport->SetLookDirection(vLook); }

public:
	CPhysXCollider* CallBack_CollisionEnter;
	CPhysXCollider* CallBack_CollisionStay;
	CPhysXCollider* CallBack_CollisionExit;

private:
	static	_uint		m_iClonedColliderIndex;
	_uint				m_iColliderIndex;

	COLLIDER_TYPE		m_eColliderType = COLLIDER_TYPE::COLLIDER_NONE;

private:
	// 최대 속도는 XZ,Y 로 나뉘어 진다. XZ에 들어가있는 값은 X에 있는 값을 사용한다.
	_vector					m_vMaxVelocity;

private:


private:
	PHYSXCOLLIDERDESC		m_PhysXColliderDesc;

	PxRigidDynamic*			m_pRigidDynamic = nullptr;
	PxRigidStatic*			m_pRigidStatic = nullptr;
	PxController*			m_pController = nullptr;
	CharacterControllerHitReport* m_pHitReport = nullptr;

	vector<PxConvexMesh*>	m_ConvexMeshes;
	vector<PxTriangleMesh*>	m_TriangleMesh;

    CTransform*				m_pTransform = nullptr;
	
	PxPhysics*				m_pPhysics = nullptr;
	PxScene*				m_pCurrentScene = nullptr;
	
	_bool					m_bPickState = false;
	_bool					m_bPickable = true;
	_bool					m_bYFixed = false;


	_bool					m_bCollision = false;

	_bool					m_bCollisionDown = false;
	_bool					m_bCollisionUp = false;
	_bool					m_bCollisionLeft = false;
	_bool					m_bCollisionRight = false;
	_bool					m_bCollisionSides = false;
public:
	static CPhysXCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	void Free();
};

END