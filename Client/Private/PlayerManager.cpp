#pragma once
#include "PlayerManager.h"
#include "Camera_TPV.h"
#include "ShockWave.h"
#include "ObjPoolManager.h"

IMPLEMENT_SINGLETON(CPlayer_Manager)

CPlayer_Manager::CPlayer_Manager()
{

}

void CPlayer_Manager::PlayerCreate()
{

}

void CPlayer_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pGameInstance = CGameInstance::Get_Instance();
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);

}

void CPlayer_Manager::Tick(_float fTimeDelta)
{
	External_State_Dash(fTimeDelta);
	SlideControl(fTimeDelta);
	State_Machine(fTimeDelta);
		if (m_fVelocity_Multiplier > 0.f)
			m_fVelocity_Multiplier -= fTimeDelta / 2.f;

		if(m_fVelocity_Multiplier > m_fVelocity_MultiplierMax)
			m_fVelocity_Multiplier = m_fVelocity_MultiplierMax;

	KeyInput(fTimeDelta);

	Set_SkillGauge(m_fSkillGauge + fTimeDelta * 2.f);


}

void CPlayer_Manager::Render()
{
}

void CPlayer_Manager::State_Machine(_float fTimeDelta)
{
	if (GAMEINSTANCE->GetKeyDown(eKeyCode::Q)) {
		SkillActive(fTimeDelta);
		return;
	}

	if(m_bIsDie == true)
		m_ePlayerState = PLAYER_DIE;

	if(m_bIsAttack == true){
		switch (m_iRandAttackAnimation) {
			case 1:
				m_pPlayerArm->Hard_Animation_Change(7, false, 1.8f);
				break;
			case 2:
				m_pPlayerArm->Hard_Animation_Change(8, false, 1.8f);
				break;
			case 3:
				m_pPlayerArm->Hard_Animation_Change(9, false, 1.8f);
				break;
			case 4:
				m_pPlayerArm->Hard_Animation_Change(10, false, 1.8f);
				break;
			case 5:
				m_pPlayerArm->Hard_Animation_Change(11, false, 1.8f);
				break;
			case 6:
				m_pPlayerArm->Hard_Animation_Change(12, false, 1.8f);
				break;
			default:
				m_bIsAttack = false;
				break;
		}
	
		if (m_pPlayerArm->AnimationFinished()) {
			m_bIsAttack = false;
			m_ePlayerState = PLAYER_IDLE;
		}
		return;
	}



	switch (m_ePlayerState)
	{
	case PLAYER_IDLE:
		State_Idle(fTimeDelta);
		break;
	case PLAYER_MOVE:
		State_Move(fTimeDelta);
		break;
	case PLAYER_JUMP:
		State_Jump(fTimeDelta);
		break;
	case PLAYER_ATTACK:
		State_Attack(fTimeDelta);
		break;
	case PLAYER_WIREACTION:
		State_WireAction(fTimeDelta);
		break;
	case PLAYER_DIE:
		State_Die(fTimeDelta);
		break;
	case PLAYER_END:
		break;
	}
}

void CPlayer_Manager::State_WireAction(_float fTimeDelta)
{
	switch(eWireState)
	{
		case WIRE_START: {
			m_pRigidBody->Set_Grab(CRigidbody::WIRE_START);
			m_fWireTime = m_fMaxWireTime;
			m_pRigidBody->Set_VelocityZ(XMVectorGetZ(m_vGrabTargetDir) * fabs(m_fGrabZDistance) * 0.04f);
			m_pRigidBody->Set_VelocityX(XMVectorGetX(m_vGrabTargetDir) * fabs(m_fGrabXDistance) * 0.04f);
			m_pRigidBody->Set_VelocityY(XMVectorGetY(m_vGrabTargetDir) * fabs(m_fGrabYDistance) * 0.1f);
			m_pRigidBody->Set_Ground(false);
			eWireState = WIRE_LOOP;
			break;
		}
		case WIRE_LOOP: {
			m_pRigidBody->Set_Grab(CRigidbody::WIRE_LOOP);
		
			m_fWireTime -= fTimeDelta;
		
			if (m_fWireTime <= 0.f || fabs(Get_PlayerToTargetXZDistance(m_pGrabTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION))) < 8.f) {
				m_fWireTime = 0.f;
				m_pRigidBody->Set_Grab(CRigidbody::WIRE_ATTACH);
				m_pRigidBody->Set_VelocityY(0.f);
				eWireState = WIRE_ATTACH;
			}
			break;
		}
		case WIRE_ATTACH: {
			m_pRigidBody->Set_Grab(CRigidbody::WIRE_ATTACH);

			if (m_pRigidBody->IsGround() == true) {
				eWireState = WIRE_END;
				m_pRigidBody->Set_Grab(CRigidbody::WIRE_END);
			}
			break;
		}
		case WIRE_END: {
			m_pRigidBody->Set_Grab(CRigidbody::WIRE_END);
			m_pRigidBody->Set_VelocityZ(0.f);
			m_pRigidBody->Set_VelocityX(0.f);
			m_pRigidBody->Set_VelocityY(0.f);
			m_fGrabZDistance = 0.f;
			m_fGrabXDistance = 0.f;
			m_fGrabYDistance = 0.f;
			m_vGrabTargetDir = XMVectorSet(0.f, 0.f, 0.f,0.f);
			m_ePlayerState = PLAYER_IDLE;
			break;
		}
	}

}

void CPlayer_Manager::State_Idle(_float fTimeDelta)
{

	if (m_pPlayerArm != nullptr && m_pPlayerArm->AnimationFinished())
		m_pPlayerArm->Soft_Animation_Change(14, true);
	
	if(m_pRigidBody != nullptr)
		m_pRigidBody->Set_Grab(CRigidbody::WIRE_END);
}

void CPlayer_Manager::State_Move(_float fTimeDelta)
{
	if (m_pPlayerArm != nullptr) {
		m_fWalkSoundTime += fTimeDelta;

		if (m_fWalkSoundTime >= m_fWalkSoundTimeMax) {
			m_fWalkSoundTime = 0;

			switch (GAMEINSTANCE->Random_Int(1, 4)) {
			case 1:
				GAMEINSTANCE->Play(TEXT("Sound_Player_Walk_01"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Walk_01"), 1.3f);
				break;
			case 2:
				GAMEINSTANCE->Play(TEXT("Sound_Player_Walk_02"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Walk_02"), 1.3f);
				break;
			case 3:
				GAMEINSTANCE->Play(TEXT("Sound_Player_Walk_03"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Walk_03"), 1.3f);
				break;
			case 4:
				GAMEINSTANCE->Play(TEXT("Sound_Player_Walk_04"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Walk_04"), 1.3f);
				break;
			}
		}

		switch(m_eWallRunState)
		{
			case WALLRUN_LEFT:
				m_pPlayerArm->Hard_Animation_Change(19, true,2.f);
				break;
			case WALLRUN_RIGHT:
				m_pPlayerArm->Hard_Animation_Change(20, true, 2.f);
				break;
			case WALLRUN_NO:
				m_pPlayerArm->Hard_Animation_Change(18, true, 2.f);
				break;
			default:
				m_pPlayerArm->Hard_Animation_Change(18, true);
				break;
		}
		//m_pPlayerArm->Hard_Animation_Change(12, true);

		if(m_pPlayerArm->AnimationFinished())
			m_ePlayerState = PLAYER_IDLE;

		if (GAMEINSTANCE->GetKeyNone(eKeyCode::W) && GAMEINSTANCE->GetKeyNone(eKeyCode::A) && GAMEINSTANCE->GetKeyNone(eKeyCode::S) && GAMEINSTANCE->GetKeyNone(eKeyCode::D)) {
			m_pPlayerArm->Hard_Animation_Change(14, true);
			m_ePlayerState = PLAYER_IDLE;
		}
	}
}

void CPlayer_Manager::State_Jump(_float fTimeDelta)
{
	if (m_pPlayerArm != nullptr) {
		switch (m_eJumpState)
		{
			case JUMP_START: {
				switch (m_iRandJumpAnimation) {
					case 1:
						m_pPlayerArm->Hard_Animation_Change(0, false, 1.3f);
						break;
					case 2:
						m_pPlayerArm->Hard_Animation_Change(1, false, 1.3f);
						break;
					case 3:
						m_pPlayerArm->Hard_Animation_Change(2, false, 1.8f);
						break;
					case 4:
						m_pPlayerArm->Hard_Animation_Change(3, false, 1.8f);
						break;
					default:
						m_pPlayerArm->Hard_Animation_Change(3, false);
						break;
				}
				if (m_pPlayerArm->AnimationFinished()) {
					m_eJumpState = JUMP_LOOP;
				}
				break;
			}
			case JUMP_LOOP: {
				m_pPlayerArm->Soft_Animation_Change(16, true);
				m_pRigidBody->Set_Grab(CRigidbody::WIRE_END);
				if (dynamic_cast<CCamera_TPV*>(GAMEINSTANCE->Get_CurCamera())->Get_Rigid_Grounded() == true) {
					m_eJumpState = JUMP_FINISH;
					m_pPlayerArm->Hard_Animation_Change(17, false, 1.5f);
					switch (GAMEINSTANCE->Random_Int(1, 3)) {
					case 1:
						GAMEINSTANCE->Play(TEXT("Sound_Player_Land_01"), false);
						GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Land_01"), 1.3f);
						break;
					case 2:
						GAMEINSTANCE->Play(TEXT("Sound_Player_Land_02"), false);
						GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Land_02"), 1.3f);
						break;
					case 3:
						GAMEINSTANCE->Play(TEXT("Sound_Player_Land_03"), false);
						GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Land_03"), 1.3f);
						break;
					}
				}
				break;
			}
			case JUMP_FINISH: {
				if (m_pPlayerArm->AnimationFinished()) {
					m_eJumpState = JUMP_NO;
					m_ePlayerState = PLAYER_IDLE;
				}
				break;
			}
		}
	}

}

void CPlayer_Manager::State_Attack(_float fTimeDelta)
{
	if (m_pPlayerArm != nullptr) {
		m_pPlayerArm->Hard_Animation_Change(2, false);
		if(m_pPlayerArm->AnimationFinished())
			m_ePlayerState = PLAYER_IDLE;
	}

}

void CPlayer_Manager::State_Die(_float fTimeDelta)
{
	Set_Ui_Off();
	GAMEINSTANCE->Set_UseGlitchLens(true);
	GAMEINSTANCE->Set_UseRedBlockGlitch(true);

	GAMEINSTANCE->Set_GlitchPower(GAMEINSTANCE->Get_GlitchPower() * pow(1.5f, fTimeDelta));
	GAMEINSTANCE->Set_GlitchRate(GAMEINSTANCE->Get_GlitchRate() * pow(1.5f, fTimeDelta));
	GAMEINSTANCE->Set_GlitchSpeed(GAMEINSTANCE->Get_GlitchSpeed() * pow(2.f, fTimeDelta));
	GAMEINSTANCE->Set_GlitchColorRate(GAMEINSTANCE->Get_GlitchColorRate() * pow(1.5f, fTimeDelta));

	if(GAMEINSTANCE->Get_RedBlockMixRate() < 0.2f)
		GAMEINSTANCE->Set_RedBlockMixRate(GAMEINSTANCE->Get_RedBlockMixRate() * pow(5.f, fTimeDelta));

	if(GAMEINSTANCE->Get_RedBlockMixRate() > 0.2f)
		GAMEINSTANCE->Set_BlackScreen(true);

	if (GAMEINSTANCE->Get_BlackScreen())
		GAMEINSTANCE->Set_BlackScreenRate(GAMEINSTANCE->Get_BlackScreenRate() + (fTimeDelta * 0.33f));


	if (m_pPlayerArm != nullptr)
		m_pPlayerArm->Hard_Animation_Change(21, false);
}

void CPlayer_Manager::Skill_Dash(_float fTimeDelta)
{
}

void CPlayer_Manager::Skill_Serge(_float fTimeDelta)
{
	if (Get_SkillGauge() != m_fSkillGaugeMax) return;
	int iRand = GAMEINSTANCE->Random_Int(7, 12);
	
	switch (GAMEINSTANCE->Random_Int(1, 2))
	{
		case 1:
			GAMEINSTANCE->Play(TEXT("Sound_Player_Skill_Nami_01"), false);
			GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Skill_Nami_01"), 1.f);
			break;
		case 2:
			GAMEINSTANCE->Play(TEXT("Sound_Player_Skill_Nami_02"), false);
			GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Skill_Nami_02"), 1.f);
			break;
	}

	m_pPlayerArm->Hard_Animation_Change(iRand, false, 2.5f);

	CShockWave::SHOCKWAVE_DESC TransformDesc = {};
	TransformDesc.vPosition = GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_POSITION);

	//_float fTempY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	//fTempY += 1.5f;
	//TransformDesc.vPosition = XMVectorSetY(TransformDesc.vPosition, fTempY);
	TransformDesc.fSpeed = 36.f;
	TransformDesc.vDirection = GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_LOOK);
	CONTAINER->ShockWaveAwake(&TransformDesc);

	Set_SkillGaugeZero();
}

void CPlayer_Manager::Skill_MindControl(_float fTimeDelta)
{

}

void CPlayer_Manager::External_State_Dash(_float fTimeDelta)
{
	if (m_bDashStart == true) {
		m_bDashStart = false;
		m_bDash = true;

		eWireState = WIRE_END;
		m_ePlayerState = PLAYER_IDLE;
		m_pRigidBody->Set_Grab(CRigidbody::WIRE_END);

		GAMEINSTANCE->Set_TimeMultiplier(0.4f);
		m_pRigidBody->Set_VelocityX(0.f);
		m_pRigidBody->Set_VelocityZ(0.f);
		GAMEINSTANCE->Set_BlueScreen(true);
		GAMEINSTANCE->Set_BlurScreen(true);
		GAMEINSTANCE->Set_BlurPower(5.f);
		GAMEINSTANCE->Set_UseGlitchLens(true);

		switch (GAMEINSTANCE->Random_Int(1, 2))
		{
		case 1:
			GAMEINSTANCE->Play(TEXT("Sound_Player_Dash_Charge_01"), false);
			GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Dash_Charge_01"), 1.f);
			break;
		case 2:
			GAMEINSTANCE->Play(TEXT("Sound_Player_Dash_Charge_01"), false);
			GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Dash_Charge_01"), 1.f);
			break;
		}
		GAMEINSTANCE->Play(TEXT("Sound_Player_Dash_Charge_loop"), true);
		GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Dash_Charge_loop"), 1.f);


	}


	if (m_bDash == true)
	{

		m_pRigidBody->Set_VelocityY(-0.01f);

		if(GAMEINSTANCE->Get_TimeMultiplier() > 0.15f)
			GAMEINSTANCE->Set_TimeMultiplier(GAMEINSTANCE->Get_TimeMultiplier() - 0.016f/2.f);

		if(GAMEINSTANCE->Get_Fov() < 95.f)
			GAMEINSTANCE->Set_Fov(GAMEINSTANCE->Get_Fov() + ( 0.016f * 1.5f));

		if(GAMEINSTANCE->Get_BlueScreenRate() < 0.2f)
			GAMEINSTANCE->Set_BlueScreenRate(GAMEINSTANCE->Get_BlueScreenRate() + 0.016f / 5.f );
	
		GAMEINSTANCE->Set_BlurPower(GAMEINSTANCE->Get_BlurPower() + 0.016f * 5.f);
		
		m_fDashTime += 0.016f;

		if (m_fDashTime > m_fDashTimeMax) {
			switch (GAMEINSTANCE->Random_Int(1, 2))
			{
			case 1:
				GAMEINSTANCE->Play(TEXT("Sound_Player_Dash_01"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Dash_01"), 1.f);
				break;
			case 2:
				GAMEINSTANCE->Play(TEXT("Sound_Player_Dash_02"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Dash_02"), 1.f);
				break;
			}

			GAMEINSTANCE->Stop(TEXT("Sound_Player_Dash_Charge_loop"));

			m_bDash = false;
			m_bDashEnd = true;
		}

		if (GAMEINSTANCE->GetKeyUp(eKeyCode::LShift)) {

			switch (GAMEINSTANCE->Random_Int(1, 2))
			{
				case 1:
					GAMEINSTANCE->Play(TEXT("Sound_Player_Dash_01"), false);
					GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Dash_01"), 1.f);
					break;
				case 2:
					GAMEINSTANCE->Play(TEXT("Sound_Player_Dash_02"), false);
					GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Dash_02"), 1.f);
					break;
			}

			GAMEINSTANCE->Stop(TEXT("Sound_Player_Dash_Charge_loop"));

			m_bDash = false;
			m_bDashEnd = true;
		}


	}

	if (m_bDashEnd == true) {
		m_fDashTime = 0.f;
		m_bDashEnd = false;
		GAMEINSTANCE->Set_TimeMultiplier(1.f);
		GAMEINSTANCE->Set_Fov(90.f);
		GAMEINSTANCE->Set_BlueScreenRate(0.f);
		GAMEINSTANCE->Set_BlurPower(0.f);
		GAMEINSTANCE->Set_BlurScreen(false);
		GAMEINSTANCE->Set_BlueScreen(false);
		GAMEINSTANCE->Set_UseGlitchLens(false);
	
	}

}

void CPlayer_Manager::KeyInput(_float fTimeDelta)
{
	if (GAMEINSTANCE->GetKeyDown(eKeyCode::Y))
		Set_PlayerInvincible(!Get_PlayerInvincible());

	if (GAMEINSTANCE->GetKey(eKeyCode::Left)) {
		if (Get_SkillGauge() > 0)
			Set_SkillGauge(Get_SkillGauge() - 15.f * fTimeDelta);
		if (Get_SkillGauge() <= 0)
			Set_SkillGauge(0);
	}

	if (GAMEINSTANCE->GetKey(eKeyCode::Right)) {
		if (Get_SkillGauge() < m_fSkillGaugeMax)
			Set_SkillGauge(Get_SkillGauge() + 15.f * fTimeDelta);
		if (Get_SkillGauge() >= m_fSkillGaugeMax)
			Set_SkillGauge(m_fSkillGaugeMax);
	}

	if (GAMEINSTANCE->GetKeyDown(eKeyCode::One)) {
		m_pGameInstance->Play(TEXT("Sound_Player_Skill_Switch"), false);
		m_pGameInstance->SetVolume(TEXT("Sound_Player_Skill_Switch"), 1.f);
		Set_Skill_Index(SKILL_DASH);
	}

	if (GAMEINSTANCE->GetKeyDown(eKeyCode::Two)) {
		m_pGameInstance->Play(TEXT("Sound_Player_Skill_Switch"), false);
		m_pGameInstance->SetVolume(TEXT("Sound_Player_Skill_Switch"), 1.f);
		Set_Skill_Index(SKILL_SERGE);
	}

	if (GAMEINSTANCE->GetKeyDown(eKeyCode::Three)) {
		m_pGameInstance->Play(TEXT("Sound_Player_Skill_Switch"), false);
		m_pGameInstance->SetVolume(TEXT("Sound_Player_Skill_Switch"), 1.f);
		Set_Skill_Index(SKILL_MINDCONTROL);
	}
}

void CPlayer_Manager::SkillActive(_float fTimeDelta)
{
	switch (m_eSkill)
	{
	case SKILL_DASH:
		Skill_Dash(fTimeDelta);
		break;
	case SKILL_SERGE:
		Skill_Serge(fTimeDelta);
		break;
	case SKILL_MINDCONTROL:
		Skill_MindControl(fTimeDelta);
		break;
	}
}

void CPlayer_Manager::SlideControl(_float fTimeDelta)
{
	if (m_bIsSlide == true)
	{
		if (m_fSlideOffset != m_fSlideOffsetMax) {
			if (m_fSlideOffset <= m_fSlideOffsetMax) {
				m_fSlideOffset += fTimeDelta / 2.f;
			}
			else {
				m_fSlideOffset = m_fSlideOffsetMax;
			}
		}
	}

	else
	{
		if (m_fSlideOffset != 0.f) {
			if (m_fSlideOffset >= 0.f)
				m_fSlideOffset -= fTimeDelta / 2.f;
			else
				m_fSlideOffset = 0.f;
		}
	}
}

void CPlayer_Manager::GameRestart()
{
	GAMEINSTANCE->Set_UseGlitchLens(false);
	GAMEINSTANCE->Set_UseRedBlockGlitch(false);

	GAMEINSTANCE->Set_GlitchPower(0.03f);
	GAMEINSTANCE->Set_GlitchRate(0.1f);
	GAMEINSTANCE->Set_GlitchSpeed(5.f);
	GAMEINSTANCE->Set_GlitchColorRate(0.01f);
	GAMEINSTANCE->Set_RedBlockMixRate(0.01f);
	GAMEINSTANCE->Set_BlackScreen(false);
	GAMEINSTANCE->Set_BlackScreenRate(0.f);

	Set_SkillGaugeZero();
	Set_IsDie(false);
	Set_PlayerState(CPlayer_Manager::PLAYER_IDLE);
	Set_SkillGauge(0.f);
	Set_Ui_On();
}

_float CPlayer_Manager::Get_PlayerToTargetDistance(_float3 vTargetPos)
{
	_vector vPlayerPos = GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_POSITION);
	
	_float3 vPlayerPos3 = { XMVectorGetX(vPlayerPos), XMVectorGetY(vPlayerPos), XMVectorGetZ(vPlayerPos) };
	_float3 vTargetPos3 = { vTargetPos.x, vTargetPos.y, vTargetPos.z };

	float fDistance = sqrt((vPlayerPos3.x - vTargetPos3.x) * (vPlayerPos3.x - vTargetPos3.x) +
		(vPlayerPos3.y - vTargetPos3.y) * (vPlayerPos3.y - vTargetPos3.y) +
		(vPlayerPos3.z - vTargetPos3.z) * (vPlayerPos3.z - vTargetPos3.z));

	return fDistance;
}

_float CPlayer_Manager::Get_PlayerToTargetDistance(_vector vTargetPos)
{
	_vector vPlayerPos = GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_POSITION);
	
	// 두 벡터의 차이를 구합니다.
	XMVECTOR difference = XMVectorSubtract(vPlayerPos, vTargetPos);

	// 벡터의 길이를 계산합니다.
	XMVECTOR length = XMVector3Length(difference);

	// 벡터의 길이 값을 추출합니다.
	float distance;
	XMStoreFloat(&distance, length);

	return distance;
}

_float CPlayer_Manager::Get_PlayerToTargetXZDistance(_float3 vTargetPos)
{
	_vector vPlayerPos = GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_POSITION);

	_float3 vPlayerPos3 = { XMVectorGetX(vPlayerPos), 0.f, XMVectorGetZ(vPlayerPos) };
	_float3 vTargetPos3 = { vTargetPos.x, 0.f, vTargetPos.z };

	float fDistance = sqrt((vPlayerPos3.x - vTargetPos3.x) * (vPlayerPos3.x - vTargetPos3.x) +
		(vPlayerPos3.y - vTargetPos3.y) * (vPlayerPos3.y - vTargetPos3.y) +
		(vPlayerPos3.z - vTargetPos3.z) * (vPlayerPos3.z - vTargetPos3.z));

	return fDistance;
}

_float CPlayer_Manager::Get_PlayerToTargetXZDistance(_vector vTargetPos)
{
	_vector vPlayerPos = GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_POSITION);

	_float3 vPlayerPos3 = { XMVectorGetX(vPlayerPos), 0.f, XMVectorGetZ(vPlayerPos) };
	_float3 vTargetPos3 = { XMVectorGetX(vTargetPos), 0.f, XMVectorGetZ(vTargetPos) };

	float fDistance = sqrt((vPlayerPos3.x - vTargetPos3.x) * (vPlayerPos3.x - vTargetPos3.x) +
		(vPlayerPos3.y - vTargetPos3.y) * (vPlayerPos3.y - vTargetPos3.y) +
		(vPlayerPos3.z - vTargetPos3.z) * (vPlayerPos3.z - vTargetPos3.z));

	return fDistance;
}

_float CPlayer_Manager::Get_PlayerToTargetDistanceY(_float3 vTargetPos)
{
	_vector vPlayerPos = GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_POSITION);

	return vTargetPos.y - XMVectorGetY(vPlayerPos);
}

_float CPlayer_Manager::Get_PlayerToTargetDistanceY(_vector vTargetPos)
{
	_vector vPlayerPos = GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_POSITION);

	return XMVectorGetY(vTargetPos) - XMVectorGetY(vPlayerPos);
}

_float CPlayer_Manager::Get_PlayerToTargetDistanceX(_float3 vTargetPos)
{
	_vector vPlayerPos = GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_POSITION);

	return vTargetPos.x - XMVectorGetX(vPlayerPos);
}

_float CPlayer_Manager::Get_PlayerToTargetDistanceX(_vector vTargetPos)
{
	_vector vPlayerPos = GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_POSITION);

	return XMVectorGetX(vTargetPos) - XMVectorGetX(vPlayerPos);
}

_float CPlayer_Manager::Get_PlayerToTargetDistanceZ(_float3 vTargetPos)
{
	_vector vPlayerPos = GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_POSITION);

	return vTargetPos.z - XMVectorGetZ(vPlayerPos);
}

_float CPlayer_Manager::Get_PlayerToTargetDistanceZ(_vector vTargetPos)
{
	_vector vPlayerPos = GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_POSITION);

	return XMVectorGetZ(vTargetPos) - XMVectorGetZ(vPlayerPos);
}

void CPlayer_Manager::Set_Ui_On()
{
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_Main_Skill"));
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_Main_Border"));
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_Main_Gauge"));
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_Main_Icon"));
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_IconBack1"));
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_Icon1"));
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_IconBack2"));
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_Icon2"));
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_IconBack3"));
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_Icon3"));
	m_pGameInstance->Set_Ui_ActiveState(TEXT("Aim"));
}

void CPlayer_Manager::Set_Ui_Off()
{
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_Main_Skill"), false);
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_Main_Border"), false);
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_Main_Gauge"), false);
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_Main_Icon"), false);
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_IconBack1"), false);
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_Icon1"),false);
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_IconBack2"), false);
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_Icon2"), false);
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_IconBack3"), false);
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_Icon3"), false);
	m_pGameInstance->Set_Ui_ActiveState(TEXT("Aim"), false);
}

void CPlayer_Manager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}
