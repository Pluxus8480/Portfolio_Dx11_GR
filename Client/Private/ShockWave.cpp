#include "ShockWave.h"
#include "Mesh.h"
#include "GameInstance.h"
#include "PlayerManager.h"
#include "Collision_Manager.h"
#include "Waver.h"
#include "PistolGunner.h"
#include "Uzi.h"

CShockWave::CShockWave(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainedObject{ pDevice, pContext }
{
}

CShockWave::CShockWave(const CShockWave& rhs)
	: CContainedObject{ rhs }
{
}

HRESULT CShockWave::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShockWave::Initialize(void* pArg)
{


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Set_MainTag("ShockWave");
	Set_SubTag("PistolGunner");

	m_pTransformCom->Set_Scale({ 1.f, 0.75f, 3.f });
	//m_pTarget = GAMEINSTANCE->Get_CurCamera();
	////m_pTransformCom->LookAt(m_pTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION));
	//m_vDirection = GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_POSITION);
	//m_vDirection = XMVectorSubtract(m_vDirection, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	//m_vNormalDirection = XMVector4Normalize(m_vDirection);
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	//Set_Directional_Rotate();
	return S_OK;
}

void CShockWave::Activate(void* pArg)
{
	SHOCKWAVE_DESC desc = *reinterpret_cast<SHOCKWAVE_DESC*>(pArg);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, desc.vPosition);
	m_pTransformCom->Set_Speed(desc.fSpeed);
	m_vDirection = desc.vDirection;

	m_fLifeTime = m_fMaxLifeTime;
	/*m_pTarget = GAMEINSTANCE->Get_CurCamera();
	m_vTargetPosition = m_pTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->LookAt(m_pTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION));
	m_vDirection = XMVectorSubtract(m_vDirection, m_pTransformCom->Get_State(CTransform::STATE_POSITION));*/
	m_pTransformCom->LookAt(m_vDirection);
	//m_vDirection = XMVectorSubtract(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_vDirection);
	m_vNormalDirection = XMVector4Normalize(m_vDirection);

	Set_ContainState(CONTAIN_ACTIVE);
	m_eShockWaveState = SHOCKWAVE_FIRE;
	bReflect = false;
	Set_Directional_Rotate();
	m_pTransformCom->PureTurn(m_vNormalDirection, GAMEINSTANCE->Random_Float(0.f, 3.14f));
	m_pTransformCom->Set_Scale({ 1.f, 0.15f, 4.5f });
	

}

void CShockWave::PriorityTick(_float fTimeDelta)
{

}

void CShockWave::Tick(_float fTimeDelta)
{
	if (SHOCKWAVE_SLEEP != m_eShockWaveState) {
		State_Machine(fTimeDelta);

		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	}
}

void CShockWave::LateTick(_float fTimeDelta)
{
	if (SHOCKWAVE_SLEEP != m_eShockWaveState)
		m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_GLOW, this);

}

HRESULT CShockWave::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		/*if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE)))
			continue;*/

		if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
			return E_FAIL;

		m_pShaderCom->Begin(1);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

void CShockWave::DeActivate()
{
	m_eShockWaveState = SHOCKWAVE_SLEEP;
	m_eContainState = CONTAIN_SLEEP;
}

void CShockWave::Collision_Enter(CGameObject* pOpponent)
{
	if (m_eContainState == CONTAIN_SLEEP)
		return;

	if (pOpponent->Get_MainTag() == "Monster") {
		if (pOpponent->Get_SubTag() == "PistolGunner") {
			dynamic_cast<CPistolGunner*>(pOpponent)->Set_State(CPistolGunner::PISTOLGUNNER_DIE);
			//DeActivate();
		}
		if (pOpponent->Get_SubTag() == "Uzi") {
			dynamic_cast<CUzi*>(pOpponent)->Set_State(CUzi::UZI_DIE);
			//DeActivate();
		}
		if (pOpponent->Get_SubTag() == "Waver") {
			dynamic_cast<CWaver*>(pOpponent)->Set_State(CWaver::PISTOLGUNNER_DIE);
			//DeActivate();
		}
	}

	/*if (pOpponent->Get_MainTag() == "Player") {
		if (PLAYER->Get_IsAttack() || PLAYER->Get_PlayerInvincible() == true) {
			m_vDirection = XMVectorNegate(m_vDirection);
			Set_Directional_Rotate();

			if (GAMEINSTANCE->Random_Bool()) {
				PLAYER->Get_PlayerArm()->Hard_Animation_Change(4, false, 1.5f);
				PLAYER->Set_PlayerAttack(false);
			}
			else {
				PLAYER->Get_PlayerArm()->Hard_Animation_Change(5, false, 1.5f);
				PLAYER->Set_PlayerAttack(false);
			}

			bReflect = true;
		}
		else {
			PLAYER->Set_IsDie(true);
		}
	}

	if (pOpponent->Get_MainTag() == "Monster" && (bReflect == true)) {
		if (pOpponent->Get_SubTag() == "PistolGunner") {
			dynamic_cast<CPistolGunner*>(pOpponent)->Set_State(CPistolGunner::PISTOLGUNNER_DIE);
			DeActivate();
		}
		if (pOpponent->Get_SubTag() == "Uzi") {
			dynamic_cast<CUzi*>(pOpponent)->Set_State(CUzi::UZI_DIE);
			DeActivate();
		}
		if (pOpponent->Get_SubTag() == "Waver") {
			dynamic_cast<CWaver*>(pOpponent)->Set_State(CWaver::PISTOLGUNNER_DIE);
			DeActivate();
		}
	}*/

}

void CShockWave::Set_Directional_Rotate()
{
	// 현재 꼬리 방향 (0, 1, 0)
	XMVECTOR currentTailDirection = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);

	// 타겟 방향 벡터 계산
	//XMVECTOR targetDirection = XMVectorSubtract(m_pTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	XMVECTOR targetDirection = m_vNormalDirection;//XMVectorSubtract(m_vTargetPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (bReflect == true)
		targetDirection = XMVectorNegate(targetDirection);

	targetDirection = XMVector3Normalize(targetDirection);

	// 타겟 반대 방향 벡터 계산
	XMVECTOR oppositeDirection = XMVectorNegate(targetDirection);

	XMVECTOR rotationQuaternion = GAMEINSTANCE->XMQuaternionRotationBetweenVectors(currentTailDirection, oppositeDirection);

	// 회전 행렬 계산
	XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(rotationQuaternion);

	//// 두 벡터 사이의 회전 축 계산
	//XMVECTOR rotationAxis = XMVector3Cross(currentTailDirection, oppositeDirection);
	//rotationAxis = XMVector3Normalize(rotationAxis);

	//// 두 벡터 사이의 각도 계산
	//float angle = acosf(XMVectorGetX(XMVector3Dot(currentTailDirection, oppositeDirection)));

	XMVECTOR scale, rot, trans;
	XMMatrixDecompose(&scale, &rot, &trans, m_pTransformCom->Get_WorldMatrix());
	XMMATRIX translationMatrix = XMMatrixTranslationFromVector(trans);

	_float4x4 mat = {};
	XMStoreFloat4x4(&mat, XMMatrixMultiply(rotationMatrix, translationMatrix));
	m_pTransformCom->Set_WorldMatrix(mat);
}

HRESULT CShockWave::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxBullet"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For PhysX Collider*/
	/*if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysX"), reinterpret_cast<CComponent**>(&m_pPhysXCollider))))
		return E_FAIL;*/

		/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_ShockWave"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC	BoundingDesc{};

	BoundingDesc.fRadius = 1.f;
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.fRadius, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	m_pColliderCom->Set_Owner(this);
	GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_PROJECTILE, m_pColliderCom);

	/*_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		string PistolGunner = "Bullet";
		string tmp = to_string(i);

		PistolGunner += tmp;

	}

	_float3 m_vPosition = {};
	XMStoreFloat3(&m_vPosition, m_vDirection);
	m_pPhysXCollider->createCharacterController(CGameInstance::Get_Instance()->Get_PhysX_ControllerManager(), PxVec3(m_vPosition.x, m_vPosition.y, m_vPosition.z));*/

	return S_OK;
}

HRESULT CShockWave::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CCamera_Manager::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CCamera_Manager::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

void CShockWave::State_Machine(_float fTimeDelta)
{
	switch (m_eShockWaveState)
	{
	case SHOCKWAVE_FIRE:
		State_Fire(fTimeDelta);
		break;
	case SHOCKWAVE_HIT:
		//State_Search(fTimeDelta);
		break;
	case SHOCKWAVE_SLEEP:
		//State_Attack(fTimeDelta);
		break;
	case SHOCKWAVE_END:
		//State_Die(fTimeDelta);
		break;
	}
}

void CShockWave::State_Fire(_float fTimeDelta)
{
	//m_pTransformCom->Go_Straight(fTimeDelta);
	m_pTransformCom->Go_Vector(fTimeDelta, m_vNormalDirection);
	//if (bReflect == true)
		//m_pTransformCom->Go_Vector(fTimeDelta, m_vDirection);
	//m_pPhysXCollider->Get_Controller()->move(PxVec3(vDirection.x, vDirection.y, vDirection.z), 0.1f, fTimeDelta, nullptr);
	m_fLifeTime -= fTimeDelta;

	if (m_fLifeTime < 0.f) {
		DeActivate();
	}
}

void CShockWave::State_Hit(_float fTimeDelta)
{

}

void CShockWave::State_Sleep(_float fTimeDelta)
{

}

//void CShockWave::State_Idle(_float fTimeDelta)
//{
//	if (PLAYER->Get_PlayerToTargetDistance(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) < 10.f)
//}
//
//void CShockWave::State_Attack(_float fTimeDelta)
//{
//}
//
//void CShockWave::State_Die(_float fTimeDelta)
//{
//}

void CShockWave::Get_DeathCheck()
{

}

CShockWave* CShockWave::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShockWave* pInstance = new CShockWave(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CShockWave"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CShockWave::Clone(void* pArg)
{
	CShockWave* pInstance = new CShockWave(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CShockWave"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShockWave::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	//Safe_Release(m_pPhysXCollider);
	Safe_Release(m_pColliderCom);

}
