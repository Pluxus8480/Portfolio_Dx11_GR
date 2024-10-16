#include "Bullet.h"
#include "Mesh.h"
#include "GameInstance.h"
#include "PlayerManager.h"
#include "Collision_Manager.h"
#include "Waver.h"
#include "PistolGunner.h"
#include "Spark.h"
#include "Uzi.h"

CBullet::CBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainedObject{ pDevice, pContext }
{
}

CBullet::CBullet(const CBullet& rhs)
	: CContainedObject{ rhs }
{
}

HRESULT CBullet::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBullet::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Set_MainTag("Bullet");
	Set_SubTag("PistolGunner");

	m_pTransformCom->Set_Scale({ 0.15f, 0.75f, 0.15f });
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

void CBullet::Activate(void* pArg)
{
	BULLET_DESC desc = *reinterpret_cast<BULLET_DESC*>(pArg);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, desc.vPosition);
	m_pTransformCom->Set_Speed(desc.fSpeed);
	m_vDirection = desc.vDirection;
	m_bColor = desc.bColor;
	m_fLifeTime = m_fMaxLifeTime;
	if(desc.bReflect == true)
		bReflect = true;
	else
		bReflect = false;

	if(desc.pTarget != nullptr)
		m_pTarget = desc.pTarget;
	else
		m_pTarget = GAMEINSTANCE->Get_CurCamera();


	m_vTargetPosition = m_pTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	//m_pTransformCom->LookAt(m_pTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION));
	m_vDirection = XMVectorSubtract(m_vDirection, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	//m_vDirection = XMVectorSubtract(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_vDirection);
	m_vNormalDirection = XMVector4Normalize(m_vDirection);

	Set_ContainState(CONTAIN_ACTIVE);
	m_eBulletState = BULLET_FIRE;
	Set_Directional_Rotate();
	m_pTransformCom->Set_Scale({ 0.15f, 1.25f, 0.15f });
}

void CBullet::PriorityTick(_float fTimeDelta)
{

}

void CBullet::Tick(_float fTimeDelta)
{

	//Set_Directional_Rotate();
	//Get_DeathCheck();
	//if (GAMEINSTANCE->GetKeyDown(eKeyCode::Right))
	//	m_pModelCom->Set_AnimIndex(m_pModelCom->Get_CurrentAnimIndex() + 1, true);

	//m_pPhysXCollider->Synchronize_Transform_Position(m_pTransformCom);
	//m_pModelCom->Play_Animation(fTimeDelta);
	if (BULLET_SLEEP != m_eBulletState) {
		State_Machine(fTimeDelta);

		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	}
}

void CBullet::LateTick(_float fTimeDelta)
{
	if(BULLET_SLEEP != m_eBulletState)
		m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_GLOW, this);

}

HRESULT CBullet::Render()
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
		if(m_bColor == false)
			m_pShaderCom->Begin(0);
		else
			m_pShaderCom->Begin(2);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

void CBullet::DeActivate()
{
	m_eBulletState = BULLET_SLEEP;
	m_eContainState = CONTAIN_SLEEP;
	m_bColor = false;
}

void CBullet::Collision_Enter(CGameObject* pOpponent)
{
	if (m_eContainState == CONTAIN_SLEEP)
		return;

	if (pOpponent->Get_MainTag() == "Player") {
		if (PLAYER->Get_IsAttack() || PLAYER->Get_PlayerInvincible() == true) {

			CSpark* Spark = dynamic_cast<CSpark*>(m_pGameInstance->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Spark")));
			Spark->Get_Transform()->Set_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVector4Normalize(GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_LOOK)));
			/*_float3 vPivot;
			XMStoreFloat3(&vPivot, XMVectorSetY(XMVectorNegate(XMVector4Normalize(GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_LOOK) * 0.01f)),0.f));
			Spark->Set_Pivot(vPivot);*/
			GAMEINSTANCE->Set_ShakePower(1.f);
			GAMEINSTANCE->Set_ShakeTime(1.f);

			switch (GAMEINSTANCE->Random_Int(1, 7)) {
			case 1:
				m_pGameInstance->Play(TEXT("Sound_Player_Katana_Parry_01"), false);
				m_pGameInstance->SetVolume(TEXT("Sound_Player_Katana_Parry_01"), 0.25f);
				break;
			case 2:
				m_pGameInstance->Play(TEXT("Sound_Player_Katana_Parry_02"), false);
				m_pGameInstance->SetVolume(TEXT("Sound_Player_Katana_Parry_02"), 0.25f);
				break;
			case 3:
				m_pGameInstance->Play(TEXT("Sound_Player_Katana_Parry_03"), false);
				m_pGameInstance->SetVolume(TEXT("Sound_Player_Katana_Parry_03"), 0.25f);
				break;
			case 4:
				m_pGameInstance->Play(TEXT("Sound_Player_Katana_Parry_04"), false);
				m_pGameInstance->SetVolume(TEXT("Sound_Player_Katana_Parry_04"), 0.25f);
				break;
			case 5:
				m_pGameInstance->Play(TEXT("Sound_Player_Katana_Parry_05"), false);
				m_pGameInstance->SetVolume(TEXT("Sound_Player_Katana_Parry_05"), 0.25f);
				break;
			case 6:
				m_pGameInstance->Play(TEXT("Sound_Player_Katana_Parry_06"), false);
				m_pGameInstance->SetVolume(TEXT("Sound_Player_Katana_Parry_06"), 0.25f);
				break;
			case 7:
				m_pGameInstance->Play(TEXT("Sound_Player_Katana_Parry_07"), false);
				m_pGameInstance->SetVolume(TEXT("Sound_Player_Katana_Parry_07"), 0.25f);
				break;
			}



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
			switch (GAMEINSTANCE->Random_Int(1, 5))
			{
			case 1:
				GAMEINSTANCE->Play(TEXT("Sound_PistolGunner_Death_01"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_PistolGunner_Death_01"), 1.f);
				break;
			case 2:
				GAMEINSTANCE->Play(TEXT("Sound_PistolGunner_Death_02"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_PistolGunner_Death_02"), 1.f);
				break;
			case 3:
				GAMEINSTANCE->Play(TEXT("Sound_PistolGunner_Death_03"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_PistolGunner_Death_03"), 1.f);
				break;
			case 4:
				GAMEINSTANCE->Play(TEXT("Sound_PistolGunner_Death_04"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_PistolGunner_Death_04"), 1.f);
				break;
			case 5:
				GAMEINSTANCE->Play(TEXT("Sound_PistolGunner_Death_05"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_PistolGunner_Death_05"), 1.f);
				break;
			}
			DeActivate();
		}
		if (pOpponent->Get_SubTag() == "Uzi") {
			dynamic_cast<CUzi*>(pOpponent)->Set_State(CUzi::UZI_DIE);
			switch (GAMEINSTANCE->Random_Int(1, 5))
			{
			case 1:
				GAMEINSTANCE->Play(TEXT("Sound_Uzi_Death_01"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Uzi_Death_01"), 1.f);
				break;
			case 2:
				GAMEINSTANCE->Play(TEXT("Sound_Uzi_Death_02"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Uzi_Death_02"), 1.f);
				break;
			case 3:
				GAMEINSTANCE->Play(TEXT("Sound_Uzi_Death_03"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Uzi_Death_03"), 1.f);
				break;
			case 4:
				GAMEINSTANCE->Play(TEXT("Sound_Uzi_Death_04"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Uzi_Death_04"), 1.f);
				break;
			case 5:
				GAMEINSTANCE->Play(TEXT("Sound_Uzi_Death_05"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Uzi_Death_05"), 1.f);
				break;
			}
			DeActivate();
		}
		if (pOpponent->Get_SubTag() == "Waver") {
			dynamic_cast<CWaver*>(pOpponent)->Set_State(CWaver::PISTOLGUNNER_DIE);
			DeActivate();
		}
	}

}

void CBullet::Set_Directional_Rotate()
{
	// 현재 꼬리 방향 (0, 1, 0)
	XMVECTOR currentTailDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	// 타겟 방향 벡터 계산
	//XMVECTOR targetDirection = XMVectorSubtract(m_pTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	
	XMVECTOR targetDirection = XMVectorSubtract(m_vTargetPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	
	if(bReflect == true)
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

HRESULT CBullet::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Bullet"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC	BoundingDesc{};

	BoundingDesc.fRadius = 0.25f;
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

HRESULT CBullet::Bind_ShaderResources()
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

void CBullet::State_Machine(_float fTimeDelta)
{
	switch (m_eBulletState)
	{
	case BULLET_FIRE:
		State_Fire(fTimeDelta);
		break;
	case BULLET_HIT:
		//State_Search(fTimeDelta);
		break;
	case BULLET_SLEEP:
		//State_Attack(fTimeDelta);
		break;
	case BULLET_END:
		//State_Die(fTimeDelta);
		break;
	}
}

void CBullet::State_Fire(_float fTimeDelta)
{
	m_pTransformCom->Go_Vector(fTimeDelta, m_vDirection);
	
	if( bReflect== true )
		m_pTransformCom->Go_Vector(fTimeDelta, m_vDirection);
	//m_pPhysXCollider->Get_Controller()->move(PxVec3(vDirection.x, vDirection.y, vDirection.z), 0.1f, fTimeDelta, nullptr);
	m_fLifeTime -= fTimeDelta;

	if (m_fLifeTime < 0.f) {
		DeActivate();
	}
}

void CBullet::State_Hit(_float fTimeDelta)
{

}

void CBullet::State_Sleep(_float fTimeDelta)
{

}

//void CBullet::State_Idle(_float fTimeDelta)
//{
//	if (PLAYER->Get_PlayerToTargetDistance(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) < 10.f)
//}
//
//void CBullet::State_Attack(_float fTimeDelta)
//{
//}
//
//void CBullet::State_Die(_float fTimeDelta)
//{
//}

void CBullet::Get_DeathCheck()
{
	
}

CBullet* CBullet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBullet* pInstance = new CBullet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CBullet::Clone(void* pArg)
{
	CBullet* pInstance = new CBullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBullet::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	//Safe_Release(m_pPhysXCollider);
	Safe_Release(m_pColliderCom);

}
