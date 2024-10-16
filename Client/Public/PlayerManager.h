#pragma once
#include "Client_Defines.h"
#include "GameInstance.h"
#include "PhysXCollider.h"
#include "RigidBody.h"
#include "PlayerArm.h"
#include "RigidBody.h"
#include "Base.h"

BEGIN(Client)

class CPlayer_Manager : public CBase
{
	DECLARE_SINGLETON(CPlayer_Manager)

public:
	enum PLAYER_STATE { PLAYER_IDLE, PLAYER_MOVE, PLAYER_JUMP, PLAYER_ATTACK, PLAYER_GUARD, PLAYER_DIE, PLAYER_WIREACTION, PLAYER_END };
	enum JUMP_STATE { JUMP_START, JUMP_LOOP, JUMP_FINISH, JUMP_NO, JUMP_END};
	enum WIRE_STATE { WIRE_START, WIRE_LOOP, WIRE_ATTACH, WIRE_END};
	enum WALLRUN_STATE { WALLRUN_LEFT, WALLRUN_RIGHT, WALLRUN_NO, WALLRUN_END };
	enum SKILL_ENUM { SKILL_DASH, SKILL_SERGE, SKILL_MINDCONTROL, SKILL_END};
private:
	CPlayer_Manager();
	virtual ~CPlayer_Manager() = default;

public:
	void PlayerCreate();

public:
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Tick(_float fTimeDelta);
	void Render();

private:
	void State_Machine(_float fTimeDelta);
	void State_WireAction(_float fTimeDelta);
	void State_Idle(_float fTimeDelta);
	void State_Move(_float fTimeDelta);
	void State_Jump(_float fTimeDelta);
	void State_Attack(_float fTimeDelta);
	void State_Die(_float fTimeDelta);

	void Skill_Dash(_float fTimeDelta);
	void Skill_Serge(_float fTimeDelta);
	void Skill_MindControl(_float fTimeDelta);

	void External_State_Dash(_float fTimeDelta);

	void KeyInput(_float fTimeDelta);

	void SkillActive(_float fTimeDelta);
private:
	void SlideControl(_float fTimeDelta);

public:
	void GameRestart();

	void Set_PlayerState(PLAYER_STATE ePlayerState) {

		if (ePlayerState == PLAYER_JUMP) {
			Set_RandJumpAnimation();
		}

		if(ePlayerState == PLAYER_WIREACTION && (eWireState != WIRE_END))
		{

			if(eWireState == WIRE_START)
				m_ePlayerState = ePlayerState;

			return;
		}


		m_ePlayerState = ePlayerState; 
	}

	PLAYER_STATE Get_PlayerState() { return m_ePlayerState; }

	void Set_Velocity_Multiplier(_float fVelocity_Multiplier) { m_fVelocity_Multiplier = fVelocity_Multiplier; }
	void Add_Velocity_Multiplier(_float fVelocity_Multiplier) { m_fVelocity_Multiplier += fVelocity_Multiplier; }
	_float Get_Velocity_Multiplier() { return m_fVelocity_Multiplier; }

	void Set_IsJump(bool bIsJump) { m_bIsJump = bIsJump; }
	bool Get_IsJump() { return m_bIsJump; }

	void Set_IsAttack(bool bIsAttack) { 
		if(bIsAttack == m_bIsAttack)
			return;

		Set_RandAttackAnimation();
		m_bIsAttack = bIsAttack; }
	bool Get_IsAttack() { return m_bIsAttack; }

	void Set_IsSlide(bool bIsSlide) { m_bIsSlide = bIsSlide; }
	bool Get_IsSlide() { return m_bIsSlide; }

	void Set_IsDie(bool bIsDie) { 
		if (m_bIsDie == false && bIsDie == true) {
			switch (GAMEINSTANCE->Random_Int(1, 5))
			{
			case 1:
				GAMEINSTANCE->Play(TEXT("Sound_Player_Die_01"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Die_01"), 1.f);
				break;
			case 2:
				GAMEINSTANCE->Play(TEXT("Sound_Player_Die_02"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Die_02"), 1.f);
				break;
			case 3:
				GAMEINSTANCE->Play(TEXT("Sound_Player_Die_03"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Die_03"), 1.f);
				break;
			case 4:
				GAMEINSTANCE->Play(TEXT("Sound_Player_Die_04"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Die_04"), 1.f);
				break;
			case 5:
				GAMEINSTANCE->Play(TEXT("Sound_Player_Die_05"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Die_05"), 1.f);
				break;
			}
		}
		m_bIsDie = bIsDie;
	
	}
	bool Get_IsDie() { return m_bIsDie; }

	void Set_IsWallRun(bool bIsWallRun) { m_bIsWallRun = bIsWallRun; }
	bool Get_IsWallRun() { return m_bIsWallRun; }

	void Set_PlayerArm(CPlayerArm* pPlayerArm) { m_pPlayerArm = pPlayerArm; }
	CPlayerArm* Get_PlayerArm() { return m_pPlayerArm; }

	_float Get_SlideOffset() { return m_fSlideOffset; }

	void Set_JumpState(JUMP_STATE eJumpState) {	m_eJumpState = eJumpState; }
	JUMP_STATE Get_JumpState() { return m_eJumpState; }

	void Set_WallRunState(WALLRUN_STATE eWallRunState) { m_eWallRunState = eWallRunState; }
	WALLRUN_STATE Get_WallRunState() { return m_eWallRunState; }

	void Set_RandJumpAnimation() { m_iRandJumpAnimation = GAMEINSTANCE->Random_Int(1, 4); }
	void Set_RandAttackAnimation() { m_iRandAttackAnimation = GAMEINSTANCE->Random_Int(1, 6); }
	void Set_RandDeathAnimation() { m_iRandDeathAnimation = GAMEINSTANCE->Random_Int(1, 3); }

	_float Get_PlayerToTargetDistance(_float3 vTargetPos);
	_float Get_PlayerToTargetDistance(_vector vTargetPos);

	_float Get_PlayerToTargetXZDistance(_float3 vTargetPos);
	_float Get_PlayerToTargetXZDistance(_vector vTargetPos);

	_float Get_PlayerToTargetDistanceY(_float3 vTargetPos);
	_float Get_PlayerToTargetDistanceY(_vector vTargetPos);

	_float Get_PlayerToTargetDistanceX(_float3 vTargetPos);
	_float Get_PlayerToTargetDistanceX(_vector vTargetPos);

	_float Get_PlayerToTargetDistanceZ(_float3 vTargetPos);
	_float Get_PlayerToTargetDistanceZ(_vector vTargetPos);

	void Set_GrabTarget(CGameObject* pGrabTarget) { 
		m_pGrabTarget = pGrabTarget;
		m_fGrabYDistance = Get_PlayerToTargetDistanceY(m_pGrabTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION));
		m_fGrabXDistance = Get_PlayerToTargetDistanceX(m_pGrabTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION));
		m_fGrabZDistance = Get_PlayerToTargetDistanceZ(m_pGrabTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION));
		m_vGrabTargetDir = XMVector3Normalize(m_pGrabTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION) - GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_POSITION));
	}
	CGameObject* Get_GrabTarget() { return m_pGrabTarget; }

	void Set_RigidBody(CRigidbody* pRigidBody) { m_pRigidBody = pRigidBody; }
	CRigidbody* Get_RigidBody() { return m_pRigidBody; }

	void Set_WireState(WIRE_STATE WireState) { eWireState = WireState; }
	WIRE_STATE Get_WireState() { return eWireState; }

	void Set_PlayerAttack( _bool bIsAttack) { m_bIsAttack = bIsAttack; }
	_bool Get_PlayerAttack() { return m_bIsAttack; }

	void Set_PlayerInvincible(_bool bIsInvincible) { m_bIsInvincible = bIsInvincible; }
	_bool Get_PlayerInvincible() { return m_bIsInvincible; }

	void Set_PlayerDash(_bool bDash) { m_bDash = bDash; }
	_bool Get_PlayerDash() { return m_bDash; }

	void Set_PlayerDashStart(_bool bDashStart) { m_bDashStart = bDashStart; }
	_bool Get_PlayerDashStart() { return m_bDashStart; }

	void Set_PhysXController(CPhysXCollider* pPhysXController) { m_pPhysXController = pPhysXController; }
	CPhysXCollider* Get_PhysXController() { return m_pPhysXController; }

	void Set_Real_Position(_float3 vPosition) { m_pPhysXController->Get_Controller()->setPosition(PxExtendedVec3(vPosition.x, vPosition.y, vPosition.z)); }

	void Set_SkillGauge(_float fSkillGauge) { 
		if (m_fSkillGauge == m_fSkillGaugeMax) return;

		m_fSkillGauge = fSkillGauge;
		if (m_fSkillGauge > m_fSkillGaugeMax) {

			m_pGameInstance->Play(TEXT("Sound_Player_Skill_MindHack"), false);
			m_pGameInstance->SetVolume(TEXT("Sound_Player_Skill_MindHack"), 0.7f);
			m_fSkillGauge = m_fSkillGaugeMax;
		}
	}
	void Set_SkillGaugeZero() { m_fSkillGauge = 0.f; }


	_float Get_SkillGauge() { return m_fSkillGauge; }

	_bool Get_SkillUsable() { return m_fSkillGauge >= m_fSkillGaugeMax; }

	void Set_Skill_Index(SKILL_ENUM eSkill) { m_eSkill = eSkill; }
	_int Get_Skill_Index() { return m_eSkill; }

	void Set_Ui_On();
	void Set_Ui_Off();

private:
	PLAYER_STATE m_ePlayerState = { PLAYER_IDLE };
	JUMP_STATE   m_eJumpState = { JUMP_NO };
	WALLRUN_STATE m_eWallRunState = { WALLRUN_NO };

	_uint		 m_iRandJumpAnimation = { 0 };
	_uint		 m_iRandDeathAnimation = { 0 };
	_uint		 m_iRandAttackAnimation = { 0 };

	_float       m_fVelocity_Multiplier = { 0.f };
	_float 		 m_fVelocity_MultiplierMax = { 0.7f };

	bool		 m_bIsJump = { false };
	bool		 m_bIsAttack = { false };
	bool		 m_bIsSlide = { false };
	bool		 m_bIsWallRun = { false };
	bool		 m_bIsDie = { false };
	
	bool		 m_bAnimationEnd = { false };

	CPlayerArm*  m_pPlayerArm = { nullptr };
	CRigidbody*  m_pRigidBody = { nullptr };
	CPhysXCollider* m_pPhysXController = { nullptr };

	CGameObject* m_pGrabTarget = { nullptr };
	_float 		 m_fGrabYDistance = { 0.f };
	_float		 m_fGrabXDistance = { 0.f };
	_float		 m_fGrabZDistance = { 0.f };
	_vector 	 m_vGrabTargetDir = { 0.f, 0.f, 0.f };

	_float		 m_fWireTime = { 0.f };
	_float		 m_fMaxWireTime = { 1.0f };

	WIRE_STATE   eWireState = { WIRE_END };

	_float		 m_fSlideOffset = { 0.f };
	_float		 m_fSlideOffsetMax = { 0.04f };
	
	 _bool		m_bIsInvincible = { false };

	 _bool		m_bDash = { false };
	 _bool		m_bDashStart = { false };
	 _bool		m_bDashEnd = { false };

	 _float		m_fDashTime = { 0.f };
	 _float     m_fDashTimeMax = { 3.f };
	 
	 _float     m_fSkillGauge = { 0.f };
	 _float     m_fSkillGaugeMax = { 83.f };

	 SKILL_ENUM m_eSkill = { SKILL_DASH };

	_float		m_fWalkSoundTime = { 0.f };
	_float		m_fWalkSoundTimeMax = { 0.25f };










private:
	CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	virtual void Free() override;
};

END