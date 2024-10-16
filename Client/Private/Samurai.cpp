#include "Samurai.h"
#include "Mesh.h"
#include "GameInstance.h"
#include "PlayerManager.h"
#include "Collision_Manager.h"
#include "ObjPoolManager.h"
#include "Pistol.h"
#include "Bullet.h"
#include "Spark.h"
#include "Katana.h"

CSamurai::CSamurai(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CSamurai::CSamurai(const CSamurai& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CSamurai::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSamurai::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Set_MainTag("Monster");
	Set_SubTag("Samurai");

	m_pTransformCom->Set_Scale({ 0.015f, 0.015f, 0.015f });
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimIndex(0, false);

	CKatana::WEAPON_DESC TransformDesc = {};
	TransformDesc.pParentTransform = m_pTransformCom;
	TransformDesc.pBoneMatrix = m_pModelCom->Get_BoneMatrixPtr("Gun_r");
	TransformDesc.fSpeedPerSec = 0.f;
	TransformDesc.fRotationPerSec = 0.f;
	TransformDesc.pWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
	
	CPartObject* katana = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Katana"), &TransformDesc));

	if (katana == nullptr)
		return E_FAIL;

	m_vecParts.emplace_back(katana);

	m_vInitPos = m_pPhysXCollider->Get_Controller()->getPosition();

	return S_OK;
}

void CSamurai::PriorityTick(_float fTimeDelta)
{
	for (auto& pParts : m_vecParts)
		pParts->PriorityTick(fTimeDelta);
}

void CSamurai::Tick(_float fTimeDelta)
{
	if (GAMEINSTANCE->Get_CurrentLevelIndex() != LEVEL_TOOL) {
		if (GAMEINSTANCE->GetKeyDown(eKeyCode::Right))
			m_pModelCom->Set_AnimIndex(m_pModelCom->Get_CurrentAnimIndex() + 1, true);

		State_Machine(fTimeDelta);
		m_pPhysXCollider->Synchronize_Transform_Position(m_pTransformCom);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION),XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) - 1.5f));

		if (m_eSamuraiState != SAMURAI_DIE && m_eSamuraiState != SAMURAI_IDLE) {
			m_pTransformCom->LookAt(GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_POSITION));
			m_pTransformCom->PureTurn(XMVectorSet(0.f, 1.f, 0.f, 0.f), 3.141592f);
		}
	}

	if (m_bDeathCheck == false && m_eSamuraiState == SAMURAI_DIE) {
		m_bDeathCheck = true;

		CTransform::TRANSFORM_DESC	TransformDesc{};
		XMStoreFloat4(&TransformDesc.vPosition, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) + 1.2f));

		m_pGameInstance->Add_Clone(GAMEINSTANCE->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Blood"), &TransformDesc);
	}

	m_pModelCom->Play_Animation(fTimeDelta);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	for (auto& pParts : m_vecParts)
		pParts->Tick(fTimeDelta);
}

void CSamurai::LateTick(_float fTimeDelta)
{
	if (m_pColliderCom->IsPicked() && PLAYER->Get_PlayerDash() && PLAYER->Get_SkillUsable())
		m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_GLOW, this);
	else
		m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);

	for (auto& pParts : m_vecParts)
		pParts->LateTick(fTimeDelta);

}

HRESULT CSamurai::Render()
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

void CSamurai::Collision_Enter(CGameObject* pOpponent)
{
	if (pOpponent->Get_MainTag() == "Player" && PLAYER->Get_IsAttack() && m_eSamuraiState == SAMURAI_STUN && m_eSamuraiState != SAMURAI_DIE) {
		m_eSamuraiState = SAMURAI_DIE;
		m_eDashState = DASH_END;
		Hard_Animation_Change(13, false);
		PLAYER->Set_SkillGauge(PLAYER->Get_SkillGauge() + 35.f);

		switch (GAMEINSTANCE->Random_Int(1, 4))
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

		switch (GAMEINSTANCE->Random_Int(1, 10))
		{
		case 1:
			GAMEINSTANCE->Play(TEXT("Sound_Samurai_Death_01"), false);
			GAMEINSTANCE->SetVolume(TEXT("Sound_Samurai_Death_01"), 1.f);
			break;
		case 2:
			GAMEINSTANCE->Play(TEXT("Sound_Samurai_Death_02"), false);
			GAMEINSTANCE->SetVolume(TEXT("Sound_Samurai_Death_02"), 1.f);
			break;
		case 3:
			GAMEINSTANCE->Play(TEXT("Sound_Samurai_Death_03"), false);
			GAMEINSTANCE->SetVolume(TEXT("Sound_Samurai_Death_03"), 1.f);
			break;
		case 4:
			GAMEINSTANCE->Play(TEXT("Sound_Samurai_Death_04"), false);
			GAMEINSTANCE->SetVolume(TEXT("Sound_Samurai_Death_04"), 1.f);
			break;
		case 5:
			GAMEINSTANCE->Play(TEXT("Sound_Samurai_Death_05"), false);
			GAMEINSTANCE->SetVolume(TEXT("Sound_Samurai_Death_05"), 1.f);
			break;
		case 6:
			GAMEINSTANCE->Play(TEXT("Sound_Samurai_Death_06"), false);
			GAMEINSTANCE->SetVolume(TEXT("Sound_Samurai_Death_06"), 1.f);
			break;
		case 7:
			GAMEINSTANCE->Play(TEXT("Sound_Samurai_Death_07"), false);
			GAMEINSTANCE->SetVolume(TEXT("Sound_Samurai_Death_07"), 1.f);
			break;
		case 8:
			GAMEINSTANCE->Play(TEXT("Sound_Samurai_Death_08"), false);
			GAMEINSTANCE->SetVolume(TEXT("Sound_Samurai_Death_08"), 1.f);
			break;
		case 9:
			GAMEINSTANCE->Play(TEXT("Sound_Samurai_Death_09"), false);
			GAMEINSTANCE->SetVolume(TEXT("Sound_Samurai_Death_09"), 1.f);
			break;
		case 10:
			GAMEINSTANCE->Play(TEXT("Sound_Samurai_Death_10"), false);
			GAMEINSTANCE->SetVolume(TEXT("Sound_Samurai_Death_10"), 1.f);
			break;
		}

		GAMEINSTANCE->Set_ShakePower(1.f);
		GAMEINSTANCE->Set_ShakeTime(1.f);

		return;
	}

	if (pOpponent->Get_MainTag() == "Player" && PLAYER->Get_IsAttack() == false && m_eSamuraiState == SAMURAI_DASH && m_eDashState == DASH_LOOP) {
		m_eSamuraiState = SAMURAI_ATTACK;
		Hard_Animation_Change(21, false);

		switch (GAMEINSTANCE->Random_Int(1, 2))
		{
		case 1:
			GAMEINSTANCE->Play(TEXT("Sound_Samurai_Kill_01"), false);
			GAMEINSTANCE->SetVolume(TEXT("Sound_Samurai_Kill_01"), 1.f);
			break;
		case 2:
			GAMEINSTANCE->Play(TEXT("Sound_Samurai_Kill_02"), false);
			GAMEINSTANCE->SetVolume(TEXT("Sound_Samurai_Kill_02"), 1.f);
			break;
		}

		PLAYER->Set_IsDie(true);
	}


	if (pOpponent->Get_MainTag() == "Player" && PLAYER->Get_IsAttack() && m_eSamuraiState != SAMURAI_STUN && m_eSamuraiState != SAMURAI_GUARD && m_eSamuraiState != SAMURAI_DIE) {
		if (m_eDashState == DASH_LOOP) {
			m_eSamuraiState = SAMURAI_STUN;
			Hard_Animation_Change(22, false, 1.f);

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

			switch (GAMEINSTANCE->Random_Int(1, 4))
			{
			case 1:
				GAMEINSTANCE->Play(TEXT("Sound_Samurai_Block_01"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Samurai_Block_01"), 1.f);
				break;
			case 2:
				GAMEINSTANCE->Play(TEXT("Sound_Samurai_Block_02"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Samurai_Block_02"), 1.f);
				break;
			case 3:
				GAMEINSTANCE->Play(TEXT("Sound_Samurai_Block_03"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Samurai_Block_03"), 1.f);
				break;
			case 4:
				GAMEINSTANCE->Play(TEXT("Sound_Samurai_Block_04"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Samurai_Block_04"), 1.f);
				break;
			}

			CSpark* Spark = dynamic_cast<CSpark*>(m_pGameInstance->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Spark")));
			Spark->Get_Transform()->Set_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVector4Normalize(GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_LOOK)));

			GAMEINSTANCE->Set_ShakePower(1.5f);
			GAMEINSTANCE->Set_ShakeTime(1.f);

			if (GAMEINSTANCE->Random_Bool()) {
				PLAYER->Get_PlayerArm()->Hard_Animation_Change(4, false, 1.5f);
				PLAYER->Set_PlayerAttack(false);
			}
			else {
				PLAYER->Get_PlayerArm()->Hard_Animation_Change(5, false, 1.5f);
				PLAYER->Set_PlayerAttack(false);
			}
		}
		else {
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

			m_eSamuraiState = SAMURAI_GUARD;
			m_eDashState = DASH_END;
			Hard_Animation_Change(8, false);
			
			CSpark* Spark = dynamic_cast<CSpark*>(m_pGameInstance->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Spark")));
			Spark->Get_Transform()->Set_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVector4Normalize(GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_LOOK)));

			GAMEINSTANCE->Set_ShakePower(1.5f);
			GAMEINSTANCE->Set_ShakeTime(1.f);

			if (GAMEINSTANCE->Random_Bool()) {
				PLAYER->Get_PlayerArm()->Hard_Animation_Change(4, false, 1.5f);
				PLAYER->Set_PlayerAttack(false);
			}
			else {
				PLAYER->Get_PlayerArm()->Hard_Animation_Change(5, false, 1.5f);
				PLAYER->Set_PlayerAttack(false);
			}
		}
	}

}

HRESULT CSamurai::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Samurai"),
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
		string SAMURAI = "SAMURAI";
		string tmp = to_string(i);

		SAMURAI += tmp;

	}

	_float3 m_vPosition = {};
	XMStoreFloat3(&m_vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pPhysXCollider->createCharacterController(CGameInstance::Get_Instance()->Get_PhysX_ControllerManager(), PxVec3(m_vPosition.x, m_vPosition.y, m_vPosition.z));
	m_pPhysXCollider->Get_Controller()->move(PxVec3(0.f,1.f,0.f), 0.03f, 0.034f, nullptr);
	return S_OK;
}

HRESULT CSamurai::Bind_ShaderResources()
{
	return S_OK;
}

void CSamurai::State_Machine(_float fTimeDelta)
{
	dynamic_cast<CKatana*>(m_vecParts[0])->Set_On(true);

	if (GAMEINSTANCE->GetKeyDown(eKeyCode::R)) {
		m_pPhysXCollider->Get_Controller()->setPosition(m_vInitPos);
		m_eSamuraiState = SAMURAI_IDLE;
		m_eDashState = DASH_END;
		Set_TempDead(false);
	}


	switch (m_eSamuraiState)
	{
	case SAMURAI_IDLE:
		State_Idle(fTimeDelta);
		break;
	case SAMURAI_SEARCH:
		State_Search(fTimeDelta);
		break;
	case SAMURAI_ATTACK:
		State_Attack(fTimeDelta);
		break;
	case SAMURAI_GUARD:
		State_Guard(fTimeDelta);
		break;
	case SAMURAI_DASH:
		State_Dash(fTimeDelta);
		break;
	case SAMURAI_STUN:
		State_Stun(fTimeDelta);
		break;
	case SAMURAI_DIE:
		State_Die(fTimeDelta);
		break;
	}
}

void CSamurai::State_Idle(_float fTimeDelta)
{
	Soft_Animation_Change(1, true);
	if (PLAYER->Get_PlayerToTargetDistance(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) < 20.f && PLAYER->Get_PlayerState() != CPlayer_Manager::PLAYER_DIE) {
		if (m_bFirstAttack) {

			switch(GAMEINSTANCE->Random_Int(1, 3))
			{ 
			case 1:
				GAMEINSTANCE->Play(TEXT("Sound_Samurai_Alert_01"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Samurai_Alert_01"), 1.f);
				break;
			case 2:
				GAMEINSTANCE->Play(TEXT("Sound_Samurai_Alert_02"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Samurai_Alert_02"), 1.f);
				break;
			case 3:
				GAMEINSTANCE->Play(TEXT("Sound_Samurai_Alert_03"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Samurai_Alert_03"), 1.f);
				break;
			}

		}


		Hard_Animation_Change(0, false);
		m_eSamuraiState = SAMURAI_SEARCH;
	}
}

void CSamurai::State_Search(_float fTimeDelta)
{
	if (m_pModelCom->Get_Finished() == true) {
		m_eSamuraiState = SAMURAI_DASH;
		m_eDashState = DASH_START;
	}
}

void CSamurai::State_Attack(_float fTimeDelta)
{
}

void CSamurai::State_Die(_float fTimeDelta)
{
	Hard_Animation_Change(13, false);
	dynamic_cast<CKatana*>(m_vecParts[0])->Set_On(false);
	Set_TempDead(true);
}

void CSamurai::State_Guard(_float fTimeDelta)
{
	PLAYER->Get_RigidBody()->Set_Velocity(m_vForwardPx * fTimeDelta * 2.f);
	if (m_pModelCom->Get_Finished() == true)
		m_eSamuraiState = SAMURAI_IDLE;
}

void CSamurai::State_Dash(_float fTimeDelta)
{
	switch (m_eDashState)
	{
	case DASH_START:
		Hard_Animation_Change(19, false);
		if (m_pModelCom->Get_Finished() == true) { 
			_vector tempVec = GAMEINSTANCE->Get_CurCamera()->Get_Transform()->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			tempVec = XMVector3Normalize(tempVec);
			m_vForwardPx = PxVec3(XMVectorGetX(tempVec), 0.f, XMVectorGetZ(tempVec));
			m_fDashTime = m_fDashTimeMax;
			m_eDashState = DASH_LOOP; 

			switch (GAMEINSTANCE->Random_Int(1, 6))
			{
			case 1:
				GAMEINSTANCE->Play(TEXT("Sound_Samurai_Charge_01"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Samurai_Charge_01"), 1.f);
				break;
			case 2:
				GAMEINSTANCE->Play(TEXT("Sound_Samurai_Charge_02"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Samurai_Charge_02"), 1.f);
				break;
			case 3:
				GAMEINSTANCE->Play(TEXT("Sound_Samurai_Charge_03"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Samurai_Charge_03"), 1.f);
				break;
			case 4:
				GAMEINSTANCE->Play(TEXT("Sound_Samurai_Charge_04"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Samurai_Charge_04"), 1.f);
				break;
			case 5:
				GAMEINSTANCE->Play(TEXT("Sound_Samurai_Charge_05"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Samurai_Charge_05"), 1.f);
				break;
			case 6:
				GAMEINSTANCE->Play(TEXT("Sound_Samurai_Charge_06"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Samurai_Charge_06"), 1.f);
				break;
			}


		}

		break;
	case DASH_LOOP:{
		m_pPhysXCollider->Get_Controller()->move(m_vForwardPx * 1.5f, 0.03f, fTimeDelta, nullptr);
		m_fDashTime -= fTimeDelta;
		if (m_fDashTime <= 0.f)
			m_eDashState = DASH_END;
		}
		break;
	case DASH_END:
		m_eSamuraiState = SAMURAI_IDLE;
		m_fDashTime = 0.f;
		break;
	}
}

void CSamurai::State_Stun(_float fTimeDelta)
{
	PLAYER->Get_RigidBody()->Set_Velocity(m_vForwardPx * fTimeDelta * 2.f);
	m_pPhysXCollider->Get_Controller()->move(m_vForwardPx * -0.02f, 0.03f, fTimeDelta, nullptr);
	if(m_pModelCom->Get_Finished() == true)
		m_eSamuraiState = SAMURAI_IDLE;
}

void CSamurai::Get_DeathCheck()
{
	//if (PLAYER->Get_PlayerToTargetDistance(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) < 2.5f && (PLAYER->Get_IsAttack() == true) && m_eSamuraiState != SAMURAI_DIE) {
	//	m_eSamuraiState = SAMURAI_DIE;
	//	//Hard_Animation_Change(1, false);
	//}
}

void CSamurai::Soft_Animation_Change(_uint _iAnimationNum, _bool _bLoop, _float _bSpeedMultiply)
{
	if (m_pModelCom->Get_CurrentAnimIndex() == _iAnimationNum)
		return;

	if (m_pModelCom->Get_Finished() == true) {
		m_pModelCom->Set_AnimIndex(_iAnimationNum, _bLoop);
		m_pModelCom->Set_AnimSpeed(_bSpeedMultiply);
		m_pModelCom->Set_Finished(false);
	}
}

void CSamurai::Hard_Animation_Change(_uint _iAnimationNum, _bool _bLoop, _float _bSpeedMultiply)
{
	if (m_pModelCom->Get_CurrentAnimIndex() == _iAnimationNum)
		return;

	m_pModelCom->Set_AnimIndex(_iAnimationNum, _bLoop);
	m_pModelCom->Set_AnimSpeed(_bSpeedMultiply);
	m_pModelCom->Set_Finished(false);
}

CSamurai* CSamurai::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSamurai* pInstance = new CSamurai(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSamurai"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CSamurai::Clone(void* pArg)
{
	CSamurai* pInstance = new CSamurai(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSamurai"));
		Safe_Release(pInstance);
	}

	GAMEINSTANCE->Imgui_SetSelectObject(pInstance);

	return pInstance;
}

void CSamurai::Free()
{
	__super::Free();

	for (auto& pParts : m_vecParts)
		Safe_Release(pParts);


	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pPhysXCollider);
	Safe_Release(m_pColliderCom);

}
