#include "PistolGunner.h"
#include "Mesh.h"
#include "GameInstance.h"
#include "PlayerManager.h"
#include "Collision_Manager.h"
#include "ObjPoolManager.h"
#include "Pistol.h"
#include "EnemyManager.h"
#include "Bullet.h"

CPistolGunner::CPistolGunner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CPistolGunner::CPistolGunner(const CPistolGunner& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CPistolGunner::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPistolGunner::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Set_MainTag("Monster");
	Set_SubTag("PistolGunner");

	m_pTransformCom->Set_Scale({ 0.01f, 0.01f, 0.01f });
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimIndex(4, true);

	CPistol::WEAPON_DESC TransformDesc = {};
	TransformDesc.pParentTransform = m_pTransformCom;
	TransformDesc.pBoneMatrix = m_pModelCom->Get_BoneMatrixPtr("Gun_r");
	TransformDesc.fSpeedPerSec = 0.f;
	TransformDesc.fRotationPerSec = 0.f;

	CPartObject* pistol = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_PistolGunnerWeapon"), &TransformDesc));
	
	if(pistol == nullptr)
		return E_FAIL;

	m_vecParts.emplace_back(pistol);

	return S_OK;
}

void CPistolGunner::PriorityTick(_float fTimeDelta)
{
	for(auto& pParts : m_vecParts)
		pParts->PriorityTick(fTimeDelta);
}

void CPistolGunner::Tick(_float fTimeDelta)
{
	if (m_bControlled == true) {
		if(m_pTarget == nullptr)
			m_pTarget = ENEMYMANAGER->Get_NearEnemy(this, 50.f);

	}

	if (m_bDeathCheck == false && m_ePistolGunnerState == PISTOLGUNNER_DIE) {
		m_bDeathCheck = true;

		CTransform::TRANSFORM_DESC	TransformDesc{};
		XMStoreFloat4(&TransformDesc.vPosition,XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) + 1.2f));

		m_pGameInstance->Add_Clone(GAMEINSTANCE->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Blood"), &TransformDesc);
	}


	if (GAMEINSTANCE->Get_CurrentLevelIndex() != LEVEL_TOOL) {
		State_Machine(fTimeDelta);
		//Get_DeathCheck();
		//if (GAMEINSTANCE->GetKeyDown(eKeyCode::Right))
		//	m_pModelCom->Set_AnimIndex(m_pModelCom->Get_CurrentAnimIndex() + 1, true);

		m_pPhysXCollider->Synchronize_Transform_Position(m_pTransformCom);

		if (m_ePistolGunnerState == PISTOLGUNNER_ATTACK) {
			m_pTransformCom->LookAt(GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_POSITION));
			m_pTransformCom->PureTurn(XMVectorSet(0.f, 1.f, 0.f, 0.f), 3.141592f);
		}
	}

	m_pModelCom->Play_Animation(fTimeDelta);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	for (auto& pParts : m_vecParts)
		pParts->Tick(fTimeDelta);
}

void CPistolGunner::LateTick(_float fTimeDelta)
{
	if(m_pColliderCom->IsPicked() && PLAYER->Get_PlayerDash() && PLAYER->Get_SkillUsable())
		m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_GLOW, this);
	else
		m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);

	for(auto& pParts : m_vecParts)
		pParts->LateTick(fTimeDelta);

}

HRESULT CPistolGunner::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CCamera_Manager::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CCamera_Manager::D3DTS_PROJ))))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE)))
			continue;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

void CPistolGunner::Collision_Enter(CGameObject* pOpponent)
{
	if (pOpponent->Get_MainTag() == "Player" && PLAYER->Get_IsAttack() && m_ePistolGunnerState != PISTOLGUNNER_DIE) {
		m_ePistolGunnerState = PISTOLGUNNER_DIE;
		Hard_Animation_Change(1, false);
		PLAYER->Set_SkillGauge(PLAYER->Get_SkillGauge() + 23.f);

		switch (GAMEINSTANCE->Random_Int(1,4))
		{
		case 1:
			GAMEINSTANCE->Play(TEXT("Sound_Player_Katana_Attack_01"), false);
			GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Katana_Attack_01"), 1.f);
			break;
		case 2:
			GAMEINSTANCE->Play(TEXT("Sound_Player_Katana_Attack_02"), false);
			GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Katana_Attack_02"), 1.f);
			break;
		case 3:
			GAMEINSTANCE->Play(TEXT("Sound_Player_Katana_Attack_03"), false);
			GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Katana_Attack_03"), 1.f);
			break;
		case 4:
			GAMEINSTANCE->Play(TEXT("Sound_Player_Katana_Attack_04"), false);
			GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Katana_Attack_04"), 1.f);
			break;
		}

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

		GAMEINSTANCE->Set_ShakePower(1.f);
		GAMEINSTANCE->Set_ShakeTime(1.f);
	}

}

void CPistolGunner::Collision_RayHit()
{
	if (GAMEINSTANCE->GetKeyDown(eKeyCode::Q) && PLAYER->Get_Skill_Index() == CPlayer_Manager::SKILL_MINDCONTROL && PLAYER->Get_SkillUsable()) {
		m_pGameInstance->Play(TEXT("Sound_Player_Skill_Full"), false);
		m_pGameInstance->SetVolume(TEXT("Sound_Player_Skill_Full"), 1.f);
		Set_Controlled(true);
		PLAYER->Set_SkillGaugeZero();
	}
	/*if (GAMEINSTANCE->GetKeyDown(eKeyCode::Q) && PLAYER->Get_Skill_Index() == CPlayer_Manager::SKILL_DASH) {

	}*/
}

HRESULT CPistolGunner::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_PistolGunner"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For PhysX Collider*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysX"), reinterpret_cast<CComponent**>(&m_pPhysXCollider))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC	BoundingDesc{};

	BoundingDesc.fRadius = 100.0f;
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.fRadius, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	m_pColliderCom->Set_Owner(this);
	GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_MONSTER, m_pColliderCom);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		string PistolGunner = "PistolGunner";
		string tmp = to_string(i);
		
		PistolGunner += tmp;

	}
	
	_float3 m_vPosition = {};
	XMStoreFloat3(&m_vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pPhysXCollider->createCharacterController(CGameInstance::Get_Instance()->Get_PhysX_ControllerManager(), PxVec3(m_vPosition.x, m_vPosition.y, m_vPosition.z));

	return S_OK;
}

HRESULT CPistolGunner::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CPistolGunner::Model_Rendering(_int iMeshNum)
{
	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", (_uint)iMeshNum, aiTextureType_DIFFUSE)))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pModelCom->Render((_uint)iMeshNum);

	return S_OK;

}

void CPistolGunner::State_Machine(_float fTimeDelta)
{

	//if (GAMEINSTANCE->GetKeyDown(eKeyCode::T)) {
	//	/*CTransform::TRANSFORM_DESC			TransformDesc{};
	//	XMStoreFloat4(&TransformDesc.vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	//	_float fTempY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	//	fTempY += 1.5f;
	//	TransformDesc.vPosition.y = fTempY;

	//	TransformDesc.fSpeedPerSec = 8.f;
	//	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	//	CBullet* Bullet = dynamic_cast<CBullet*>(m_pGameInstance->Add_Clone_Return(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Bullet"), &TransformDesc));
	//	Bullet->Set_State(CBullet::BULLET_FIRE);*/

	//	CBullet::BULLET_DESC TransformDesc = {};
	//	TransformDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	//	_float fTempY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	//	fTempY += 1.5f;
	//	XMVectorSetY(TransformDesc.vPosition, fTempY);
	//	TransformDesc.fSpeed = 18.f;
	//	TransformDesc.vDirection = GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_POSITION);
	//	CONTAINER->BulletAwake(&TransformDesc);

	//}

	if (GAMEINSTANCE->GetKeyDown(eKeyCode::R)){
		m_ePistolGunnerState = PISTOLGUNNER_IDLE;
		m_pTarget = nullptr;
		Set_TempDead(false);
		m_bDeathCheck = false;
		m_bFirstAttack = true;
		Set_Controlled(false);
	}


 	switch (m_ePistolGunnerState)
	{
	case PISTOLGUNNER_IDLE:
		State_Idle(fTimeDelta);
		break;
	case PISTOLGUNNER_SEARCH:
		//State_Search(fTimeDelta);
		break;
	case PISTOLGUNNER_ATTACK:
		State_Attack(fTimeDelta);
		break;
	case PISTOLGUNNER_DIE:
		State_Die(fTimeDelta);
		break;
	}
}

void CPistolGunner::State_Idle(_float fTimeDelta)
{
	Soft_Animation_Change(4, true);
	if (PLAYER->Get_PlayerToTargetDistance(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) < 20.f && PLAYER->Get_PlayerState() != CPlayer_Manager::PLAYER_DIE) {

		if (m_bFirstAttack) {

			switch(GAMEINSTANCE->Random_Int(1, 10))
			{ 
			case 1:
				m_pGameInstance->Play(TEXT("Sound_PistolGunner_Alert_01"), false);
				m_pGameInstance->SetVolume(TEXT("Sound_PistolGunner_Alert_01"), 1.f);
				break;
			case 2:
				m_pGameInstance->Play(TEXT("Sound_PistolGunner_Alert_02"), false);
				m_pGameInstance->SetVolume(TEXT("Sound_PistolGunner_Alert_02"), 1.f);
				break;
			case 3:
				m_pGameInstance->Play(TEXT("Sound_PistolGunner_Alert_03"), false);
				m_pGameInstance->SetVolume(TEXT("Sound_PistolGunner_Alert_03"), 1.f);
				break;
			case 4:
				m_pGameInstance->Play(TEXT("Sound_PistolGunner_Alert_04"), false);
				m_pGameInstance->SetVolume(TEXT("Sound_PistolGunner_Alert_04"), 1.f);
				break;
			case 5:
				m_pGameInstance->Play(TEXT("Sound_PistolGunner_Alert_05"), false);
				m_pGameInstance->SetVolume(TEXT("Sound_PistolGunner_Alert_05"), 1.f);
				break;
			case 6:
				m_pGameInstance->Play(TEXT("Sound_PistolGunner_Alert_06"), false);
				m_pGameInstance->SetVolume(TEXT("Sound_PistolGunner_Alert_06"), 1.f);
				break;
			case 7:
				m_pGameInstance->Play(TEXT("Sound_PistolGunner_Alert_07"), false);
				m_pGameInstance->SetVolume(TEXT("Sound_PistolGunner_Alert_07"), 1.f);
				break;
			case 8:
				m_pGameInstance->Play(TEXT("Sound_PistolGunner_Alert_08"), false);
				m_pGameInstance->SetVolume(TEXT("Sound_PistolGunner_Alert_08"), 1.f);
				break;
			case 9:
				m_pGameInstance->Play(TEXT("Sound_PistolGunner_Alert_09"), false);
				m_pGameInstance->SetVolume(TEXT("Sound_PistolGunner_Alert_09"), 1.f);
				break;
			case 10:
				m_pGameInstance->Play(TEXT("Sound_PistolGunner_Alert_10"), false);
				m_pGameInstance->SetVolume(TEXT("Sound_PistolGunner_Alert_10"), 1.f);
				break;
			}
			m_bFirstAttack = false;
		}

		switch (GAMEINSTANCE->Random_Int(1, 4))
		{
		case 1:
			m_pGameInstance->Play(TEXT("Sound_PistolGunner_Shot_01"), false);
			m_pGameInstance->SetVolume(TEXT("Sound_PistolGunner_Shot_01"), 1.f);
			break;
		case 2:
			m_pGameInstance->Play(TEXT("Sound_PistolGunner_Shot_02"), false);
			m_pGameInstance->SetVolume(TEXT("Sound_PistolGunner_Shot_02"), 1.f);
			break;
		case 3:
			m_pGameInstance->Play(TEXT("Sound_PistolGunner_Shot_03"), false);
			m_pGameInstance->SetVolume(TEXT("Sound_PistolGunner_Shot_03"), 1.f);
			break;
		case 4:
			m_pGameInstance->Play(TEXT("Sound_PistolGunner_Shot_04"), false);
			m_pGameInstance->SetVolume(TEXT("Sound_PistolGunner_Shot_04"), 1.f);
			break;
		}

		CBullet::BULLET_DESC TransformDesc = {};
		TransformDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		_float fTempY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		fTempY += 1.5f;
		TransformDesc.vPosition = XMVectorSetY(TransformDesc.vPosition, fTempY);
		TransformDesc.fSpeed = 36.f;
		if (m_bControlled == false) {
			TransformDesc.pTarget = GAMEINSTANCE->Get_CurCamera();
			TransformDesc.vDirection = GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_POSITION);
			TransformDesc.bReflect = false;
		}
		else {
			if (m_pTarget == nullptr)
				m_pTarget = this;

			TransformDesc.pTarget = m_pTarget;
			TransformDesc.vDirection = XMVectorSetY(TransformDesc.pTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION),XMVectorGetY(TransformDesc.pTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION)) + 1.5f);
			TransformDesc.bReflect = true;
		}
		CONTAINER->BulletAwake(&TransformDesc);
		
		if (m_pTarget == this) {
			m_ePistolGunnerState = PISTOLGUNNER_DIE;
			return;
		}

		if(m_bControlled == true)
			m_pTarget = this;

		m_ePistolGunnerState = PISTOLGUNNER_ATTACK;
	}
}

void CPistolGunner::State_Attack(_float fTimeDelta)
{
	Hard_Animation_Change(3, false);
	if (m_pModelCom->Get_Finished() == true)
		m_ePistolGunnerState = PISTOLGUNNER_IDLE;
}

void CPistolGunner::State_Die(_float fTimeDelta)
{
	Hard_Animation_Change(1, false);
	Set_TempDead(true);
}

void CPistolGunner::Get_DeathCheck()
{
	//if (PLAYER->Get_PlayerToTargetDistance(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) < 2.5f && (PLAYER->Get_IsAttack() == true) && m_ePistolGunnerState != PISTOLGUNNER_DIE) {
	//	m_ePistolGunnerState = PISTOLGUNNER_DIE;
	//	//Hard_Animation_Change(1, false);
	//}
}

void CPistolGunner::Soft_Animation_Change(_uint _iAnimationNum, _bool _bLoop, _float _bSpeedMultiply)
{
	if (m_pModelCom->Get_CurrentAnimIndex() == _iAnimationNum)
		return;

	if (m_pModelCom->Get_Finished() == true) {
		m_pModelCom->Set_AnimIndex(_iAnimationNum, _bLoop);
		m_pModelCom->Set_AnimSpeed(_bSpeedMultiply);
		m_pModelCom->Set_Finished(false);
	}
}

void CPistolGunner::Hard_Animation_Change(_uint _iAnimationNum, _bool _bLoop, _float _bSpeedMultiply)
{
	if (m_pModelCom->Get_CurrentAnimIndex() == _iAnimationNum)
		return;

	m_pModelCom->Set_AnimIndex(_iAnimationNum, _bLoop);
	m_pModelCom->Set_AnimSpeed(_bSpeedMultiply);
	m_pModelCom->Set_Finished(false);
}

CPistolGunner* CPistolGunner::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPistolGunner* pInstance = new CPistolGunner(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPistolGunner"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CPistolGunner::Clone(void* pArg)
{
	CPistolGunner* pInstance = new CPistolGunner(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPistolGunner"));
		Safe_Release(pInstance);
	}

	GAMEINSTANCE->Imgui_SetSelectObject(pInstance);

	return pInstance;
}

void CPistolGunner::Free()
{
	__super::Free();

	for(auto& pParts : m_vecParts)
		Safe_Release(pParts);


	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pPhysXCollider);
	Safe_Release(m_pColliderCom);

}
