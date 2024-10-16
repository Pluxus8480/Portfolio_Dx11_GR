#include "Boss.h"
#include "Mesh.h"
#include "GameInstance.h"
#include "PlayerManager.h"
#include "Collision_Manager.h"
#include "ObjPoolManager.h"
#include "Pistol.h"
#include "Bullet.h"
#include "Spark.h"
#include "Katana.h"
#include "ShockWave.h"

CBoss::CBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CBoss::CBoss(const CBoss& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CBoss::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Set_MainTag("Monster");
	Set_SubTag("Boss");

	m_pTransformCom->Set_Scale({ 0.01f, 0.01f, 0.01f });
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

	m_vDashPos.push_back(_float3(488.081f, -205.369f, -262.999f));
	m_vDashPos.push_back(_float3(562.777f, -205.399f, -252.578f));
	m_vDashPos.push_back(_float3(571.391f, -205.369f, -202.888f));
	m_vDashPos.push_back(_float3(521.523f, -205.369f, -159.717f));

	return S_OK;
}

void CBoss::PriorityTick(_float fTimeDelta)
{
	for (auto& pParts : m_vecParts)
		pParts->PriorityTick(fTimeDelta);
}

void CBoss::Tick(_float fTimeDelta)
{


	if (GAMEINSTANCE->Get_CurrentLevelIndex() != LEVEL_TOOL) {
		/*if (GAMEINSTANCE->GetKeyDown(eKeyCode::Right))
			m_pModelCom->Set_AnimIndex(m_pModelCom->Get_CurrentAnimIndex() + 1, true);*/
		
		State_Machine(fTimeDelta);
		m_pPhysXCollider->Synchronize_Transform_Position(m_pTransformCom);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) - 1.5f));

		if (m_eBossState != BOSS_DIE && m_eBossState != BOSS_IDLE) {
			m_pTransformCom->LookAt(GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_POSITION));
			m_pTransformCom->PureTurn(XMVectorSet(0.f, 1.f, 0.f, 0.f), 3.141592f);
		}
	}

	if (m_bDeathCheck == false && m_eBossState == BOSS_DIE) {
		m_bDeathCheck = true;

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

		CTransform::TRANSFORM_DESC	TransformDesc{};
		XMStoreFloat4(&TransformDesc.vPosition, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) + 1.2f));

		m_pGameInstance->Add_Clone(GAMEINSTANCE->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Blood"), &TransformDesc);
	}

	m_pModelCom->Play_Animation(fTimeDelta);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	for (auto& pParts : m_vecParts)
		pParts->Tick(fTimeDelta);
}

void CBoss::LateTick(_float fTimeDelta)
{
	if (m_eBossState != BOSS_DIE) {
		m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);
	}
	else {
		m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_GLOW, this);
	}
	//Àç¹ÕÀ¸´Ï±î ÀÏ´Ü ³öµÎ¼À
	for (auto& pParts : m_vecParts)
		pParts->LateTick(fTimeDelta);

}

HRESULT CBoss::Render()
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

void CBoss::Collision_Enter(CGameObject* pOpponent)
{
	if (pOpponent->Get_MainTag() == "Player" && PLAYER->Get_IsAttack() && m_eBossState == BOSS_PATTERN && m_eBossPattern == DASH_PATTERN && m_iPatternPhase == 2) {
		CSpark* Spark = dynamic_cast<CSpark*>(m_pGameInstance->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Spark")));
		Spark->Get_Transform()->Set_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVector4Normalize(GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_LOOK)));

		GAMEINSTANCE->Set_ShakePower(1.5f);
		GAMEINSTANCE->Set_ShakeTime(1.f);

		m_iHp -= 5;

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

		if (GAMEINSTANCE->Random_Bool()) {
			PLAYER->Get_PlayerArm()->Hard_Animation_Change(4, false, 1.5f);
			PLAYER->Set_PlayerAttack(false);
		}
		else {
			PLAYER->Get_PlayerArm()->Hard_Animation_Change(5, false, 1.5f);
			PLAYER->Set_PlayerAttack(false);
		}

		return;
	}

	if (pOpponent->Get_MainTag() == "Player" && PLAYER->Get_IsAttack() && m_eBossState == BOSS_PATTERN && m_eBossPattern == BULLET_PATTERN && m_iPatternPhase == 2) {
		GAMEINSTANCE->Set_ShakePower(1.5f);
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

		m_iHp -= 15;
		Hard_Animation_Change(23, false);
		m_eBossPattern = DASH_PATTERN;
		m_eBossState = BOSS_STUN;
		m_iPatternPhase = 0;
	}

	if (pOpponent->Get_MainTag() == "Player" && PLAYER->Get_IsAttack() && m_eBossState == BOSS_PATTERN && m_eBossPattern == SLASH_PATTERN && m_iPatternPhase == 2) {
		GAMEINSTANCE->Set_ShakePower(1.5f);
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

		m_iHp -= 15;
		Hard_Animation_Change(23, false);
		m_eBossPattern = DASH_PATTERN;
		m_eBossState = BOSS_STUN;
		m_iPatternPhase = 0;
	}


	/*if (pOpponent->Get_MainTag() == "Player" && PLAYER->Get_IsAttack() == false && m_eBossState == BOSS_DASH && m_eDashState == DASH_LOOP) {
		m_eBossState = BOSS_ATTACK;
		Hard_Animation_Change(21, false);
		PLAYER->Set_IsDie(true);
	}


	if (pOpponent->Get_MainTag() == "Player" && PLAYER->Get_IsAttack() && m_eBossState != BOSS_STUN && m_eBossState != BOSS_STUN && m_eBossState != BOSS_STUN) {
		if (m_eDashState == DASH_LOOP) {
			m_eBossState = BOSS_STUN;
			Hard_Animation_Change(22, false, 1.f);

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
			m_eBossState = BOSS_GUARD;
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
	}*/

}

HRESULT CBoss::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Boss"),
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
	m_pPhysXCollider->Get_Controller()->move(PxVec3(0.f, 1.f, 0.f), 0.03f, 0.034f, nullptr);
	return S_OK;
}

HRESULT CBoss::Bind_ShaderResources()
{
	return S_OK;
}

void CBoss::State_Machine(_float fTimeDelta)
{
	dynamic_cast<CKatana*>(m_vecParts[0])->Set_On(true);

	if (GAMEINSTANCE->GetKeyDown(eKeyCode::R)) {
		m_eBossState = BOSS_IDLE;
		Set_TempDead(false);
	}

	if(m_iHp <= 0)
		m_eBossState = BOSS_DIE;



	switch (m_eBossState)
	{
	case BOSS_IDLE:
		State_Idle(fTimeDelta);
		break;
	case BOSS_SEARCH:
		State_Search(fTimeDelta);
		break;
	case BOSS_PATTERN:
		State_Pattern(fTimeDelta);
		break;
	case BOSS_ATTACK:
		State_Attack(fTimeDelta);
		break;
	case BOSS_GUARD:
		State_Guard(fTimeDelta);
		break;
	case BOSS_DASH:
		State_Dash(fTimeDelta);
		break;
	case BOSS_STUN:
		State_Stun(fTimeDelta);
		break;
	case BOSS_DIE:
		State_Die(fTimeDelta);
		break;
	}
}

void CBoss::State_Idle(_float fTimeDelta)
{
	Soft_Animation_Change(15, true);
	if (PLAYER->Get_PlayerToTargetDistance(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) < 20.f && PLAYER->Get_PlayerState() != CPlayer_Manager::PLAYER_DIE) {
		Hard_Animation_Change(0, false);
		m_eBossState = BOSS_PATTERN;
		//if(m_eBossPattern == DASH_PATTERN) 
	}
}

void CBoss::State_Search(_float fTimeDelta)
{
	if (m_pModelCom->Get_Finished() == true) {
		m_eBossState = BOSS_DASH;
		m_eDashState = DASH_START;
	}
}

void CBoss::State_Attack(_float fTimeDelta)
{
}

void CBoss::State_Die(_float fTimeDelta)
{
	Hard_Animation_Change(7, true);
	dynamic_cast<CKatana*>(m_vecParts[0])->Set_On(false);
	Set_TempDead(true);

	m_pPhysXCollider->Get_Controller()->move(PxVec3(0.f, 0.034f * 2.f, 0.f), 0.03f, 0.034f, nullptr);
}

void CBoss::State_Guard(_float fTimeDelta)
{
	PLAYER->Get_RigidBody()->Set_Velocity(m_vForwardPx * fTimeDelta * 2.f);
	if (m_pModelCom->Get_Finished() == true)
		m_eBossState = BOSS_IDLE;
}

void CBoss::State_Dash(_float fTimeDelta)
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
		}

		break;
	case DASH_LOOP: {
		m_pPhysXCollider->Get_Controller()->move(m_vForwardPx * 1.5f, 0.03f, fTimeDelta, nullptr);
		m_fDashTime -= fTimeDelta;
		if (m_fDashTime <= 0.f)
			m_eDashState = DASH_END;
	}
				  break;
	case DASH_END:
		m_eBossState = BOSS_IDLE;
		m_fDashTime = 0.f;
		break;
	}
}

void CBoss::State_Stun(_float fTimeDelta)
{
	Hard_Animation_Change(5, false);

	if (m_pModelCom->Get_Finished() == true)
		m_eBossState = BOSS_IDLE;
}

void CBoss::State_Pattern(_float fTimeDelta)
{
	switch (m_eBossPattern)
	{
	case DASH_PATTERN:
		Pattern_Dash(fTimeDelta);
		break;
	case BULLET_PATTERN:
		Pattern_Bullet(fTimeDelta);
		break;
	case SLASH_PATTERN:
		Pattern_Slash(fTimeDelta);
		break;
	case EXTEND_PATTERN:
		Pattern_Extend(fTimeDelta);
		break;
	default:
		break;
	}
}

void CBoss::State_AirStrike(_float fTimeDelta)
{
}

void CBoss::State_Fly(_float fTimeDelta)
{
}

void CBoss::Get_DeathCheck()
{
	//if (PLAYER->Get_PlayerToTargetDistance(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) < 2.5f && (PLAYER->Get_IsAttack() == true) && m_eSamuraiState != SAMURAI_DIE) {
	//	m_eSamuraiState = SAMURAI_DIE;
	//	//Hard_Animation_Change(1, false);
	//}
}

void CBoss::Soft_Animation_Change(_uint _iAnimationNum, _bool _bLoop, _float _bSpeedMultiply)
{
	if (m_pModelCom->Get_CurrentAnimIndex() == _iAnimationNum)
		return;

	if (m_pModelCom->Get_Finished() == true) {
		m_pModelCom->Set_AnimIndex(_iAnimationNum, _bLoop);
		m_pModelCom->Set_AnimSpeed(_bSpeedMultiply);
		m_pModelCom->Set_Finished(false);
	}
}

void CBoss::Hard_Animation_Change(_uint _iAnimationNum, _bool _bLoop, _float _bSpeedMultiply)
{
	if (m_pModelCom->Get_CurrentAnimIndex() == _iAnimationNum)
		return;

	m_pModelCom->Set_AnimIndex(_iAnimationNum, _bLoop);
	m_pModelCom->Set_AnimSpeed(_bSpeedMultiply);
	m_pModelCom->Set_Finished(false);
}

void CBoss::Pattern_Dash(_float fTimeDelta)
{
	if (m_bDash) {
		if (m_iPatternPhase == 0) {
			Hard_Animation_Change(6, true);
			m_iPatternPhase++;
		}

		if (m_iPatternPhase == 1) {
			XMVECTOR vDest = GAMEINSTANCE->Get_CurCamera()->Get_Transform()->Get_State(CTransform::STATE_POSITION);
			XMVECTOR vDirOrigin = vDest - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			XMVECTOR vDir = XMVector3Normalize(vDirOrigin) * 0.3f;
			m_vForwardPx = PxVec3(XMVectorGetX(vDir), 0.f, XMVectorGetZ(vDir));

			m_pPhysXCollider->Get_Controller()->move(PxVec3(XMVectorGetX(vDir), XMVectorGetY(vDir), XMVectorGetZ(vDir)), 0.03f, fTimeDelta, nullptr);

			if (XMVectorGetX(XMVector3Length(vDirOrigin)) < 3.f) {
				m_pPhysXCollider->Get_Controller()->setPosition(PxExtendedVec3(XMVectorGetX(m_pTransformCom->Get_State(CTransform::STATE_POSITION)), XMVectorGetY(vDest), XMVectorGetZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION))));
				m_iPatternPhase++;
			}
		}

		if (m_iPatternPhase == 2) {
			Hard_Animation_Change(16, false, GAMEINSTANCE->Random_Float(1.f,2.f));
		}

		if (m_iPatternPhase == 2 && m_pModelCom->Get_Finished() == true) {
			m_iPatternPhase++;
		}

		if (m_iPatternPhase == 3) {
			Hard_Animation_Change(17, false);
		}

		if (m_iPatternPhase == 3 && m_pModelCom->Get_Finished() == true) {
			PLAYER->Get_RigidBody()->Set_Velocity(m_vForwardPx * 10000.f);
			GAMEINSTANCE->Set_ShakePower(1.5f);
			GAMEINSTANCE->Set_ShakeTime(1.f);

			m_eBossState = BOSS_IDLE;
			m_iPatternPhase = 0;
			m_bDash = !m_bDash;
			m_eBossPattern = SLASH_PATTERN;
		}
	}
	else {
		if (m_iPatternPhase == 0) {
			Hard_Animation_Change(6, true);
			m_iPatternPhase++;
		}

		if (m_iPatternPhase == 1) {
			XMVECTOR vDest = GAMEINSTANCE->Get_CurCamera()->Get_Transform()->Get_State(CTransform::STATE_POSITION);
			XMVECTOR vDirOrigin = vDest - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			XMVECTOR vDir = XMVector3Normalize(vDirOrigin) * 0.3f;
			m_vForwardPx = PxVec3(XMVectorGetX(vDir), 0.f, XMVectorGetZ(vDir));

			m_pPhysXCollider->Get_Controller()->move(PxVec3(XMVectorGetX(vDir), XMVectorGetY(vDir), XMVectorGetZ(vDir)), 0.03f, fTimeDelta, nullptr);

			if (XMVectorGetX(XMVector3Length(vDirOrigin)) < 3.f) {
				m_pPhysXCollider->Get_Controller()->setPosition(PxExtendedVec3(XMVectorGetX(m_pTransformCom->Get_State(CTransform::STATE_POSITION)), XMVectorGetY(vDest), XMVectorGetZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION))));
				m_iPatternPhase++;
			}
		}

		if (m_iPatternPhase == 2) {
			Hard_Animation_Change(18,false, GAMEINSTANCE->Random_Float(1.f, 2.f));
		}

		if (m_iPatternPhase == 2 && m_pModelCom->Get_Finished() == true) {
			m_iPatternPhase++;
		}

		if (m_iPatternPhase == 3) {
			Hard_Animation_Change(19, false);
		}

		if (m_iPatternPhase == 3 && m_pModelCom->Get_Finished() == true) {

			m_iHp -= 10;
			
			PLAYER->Get_RigidBody()->Set_Velocity(m_vForwardPx * 10000.f);
			GAMEINSTANCE->Set_ShakePower(1.5f);
			GAMEINSTANCE->Set_ShakeTime(1.f);

			m_eBossState = BOSS_IDLE;
			m_iPatternPhase = 0;
			m_bDash = !m_bDash;
			m_eBossPattern = BULLET_PATTERN;
		}
	}

}

void CBoss::Pattern_Bullet(_float fTimeDelta)
{

	if (m_iPatternPhase == 0) {
		Hard_Animation_Change(4, true);
		m_iPatternPhase++;
		m_iRandPos = GAMEINSTANCE->Random_Int(0, 3);
	}

	if (m_iPatternPhase == 1) {
		XMVECTOR vDest = XMLoadFloat3(&m_vDashPos[m_iRandPos]);
		XMVECTOR vDirOrigin = vDest - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		XMVECTOR vDir = XMVector3Normalize(vDirOrigin) * 0.45f;

		m_pPhysXCollider->Get_Controller()->move(PxVec3(XMVectorGetX(vDir), XMVectorGetY(vDir), XMVectorGetZ(vDir)), 0.03f, fTimeDelta, nullptr);

		if (XMVectorGetX(XMVector3Length(vDirOrigin)) < 1.f) {
			m_iPatternPhase++;
		}
	}

	if (m_iPatternPhase == 2) {
		Hard_Animation_Change(12, true);
		
		m_fShootTime -= fTimeDelta;

		if (m_fShootTime <= 0) {
			_float fTempY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			fTempY += 1.5f;

			CBullet::BULLET_DESC TransformDesc = {};
			TransformDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			TransformDesc.vPosition = XMVectorSetY(TransformDesc.vPosition, fTempY);
			TransformDesc.fSpeed = 36.f;
			TransformDesc.pTarget = GAMEINSTANCE->Get_CurCamera();
			TransformDesc.vDirection = GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_POSITION);
			TransformDesc.bReflect = false;
			CONTAINER->BulletAwake(&TransformDesc);
			m_fShootTime = m_fShootTimeMax;

		}



	}
}

void CBoss::Pattern_Slash(_float fTimeDelta)
{
	if (m_iPatternPhase == 0) {
		Hard_Animation_Change(4, true);
		m_iPatternPhase++;
		m_iRandPos = GAMEINSTANCE->Random_Int(0, 3);
	}

	if (m_iPatternPhase == 1) {
		XMVECTOR vDest = XMLoadFloat3(&m_vDashPos[m_iRandPos]);
		XMVECTOR vDirOrigin = vDest - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		XMVECTOR vDir = XMVector3Normalize(vDirOrigin) * 0.45f;

		m_pPhysXCollider->Get_Controller()->move(PxVec3(XMVectorGetX(vDir), XMVectorGetY(vDir), XMVectorGetZ(vDir)), 0.03f, fTimeDelta, nullptr);

		if (XMVectorGetX(XMVector3Length(vDirOrigin)) < 1.f) {
			m_iPatternPhase++;
		}
	}

	if (m_iPatternPhase == 2) {
		Hard_Animation_Change(12, true);

		m_fShootTime -= fTimeDelta;

		if (m_fShootTime <= 0) {
			_float fTempY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			fTempY += 1.5f;

			CShockWave::SHOCKWAVE_DESC TransformDesc = {};
			TransformDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			//_float fTempY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			//fTempY += 1.5f;
			//TransformDesc.vPosition = XMVectorSetY(TransformDesc.vPosition, fTempY);
			TransformDesc.fSpeed = 4500.f;

			_vector vSckDir = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			//GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_POSITION);
			
			for (int i = 0; i < 3; ++i) {

				TransformDesc.vDirection = XMVectorSetX(vSckDir, XMVectorGetX(vSckDir) + GAMEINSTANCE->Random_Float(-5.f,5.f));
				TransformDesc.vDirection = XMVectorSetZ(TransformDesc.vDirection, XMVectorGetZ(vSckDir) + GAMEINSTANCE->Random_Float(-5.f, 5.f));
				CONTAINER->ShockWaveAwake(&TransformDesc);
			}

			m_fShootTime = m_fShootTimeMax;

		}



	}


}

void CBoss::Pattern_Extend(_float fTimeDelta)
{
}

CBoss* CBoss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBoss* pInstance = new CBoss(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBoss"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CBoss::Clone(void* pArg)
{
	CBoss* pInstance = new CBoss(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBoss"));
		Safe_Release(pInstance);
	}

	GAMEINSTANCE->Imgui_SetSelectObject(pInstance);

	return pInstance;
}

void CBoss::Free()
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
