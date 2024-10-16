#include "Camera_TPV.h"
#include "GameInstance.h"
#include "PlayerManager.h"

CCamera_TPV::CCamera_TPV(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCoreCamera{ pDevice, pContext }
{
}

CCamera_TPV::CCamera_TPV(const CCamera_TPV& rhs)
	: CCoreCamera{ rhs }
{
}

HRESULT CCamera_TPV::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_TPV::Initialize(void* pArg)
{
	CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(pArg);
	m_fMouseSensor = pDesc->fMouseSensor;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Set_MainTag("Player");

	/* For PhysX Collider*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysX"), reinterpret_cast<CComponent**>(&m_pPhysXCollider))))
		return E_FAIL;
	
	/* For RigidBody*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidbody),m_pTransformCom)))
		return E_FAIL;

	PLAYER->Set_RigidBody(m_pRigidbody);

	_float3 m_vPosition = {};
	XMStoreFloat3(&m_vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pPhysXCollider->createCharacterController(CGameInstance::Get_Instance()->Get_PhysX_ControllerManager(), PxVec3(m_vPosition.x, m_vPosition.y, m_vPosition.z));

	PLAYER->Set_PhysXController(m_pPhysXCollider);

	///* For.Com_Collider */
	//CBounding_Sphere::BOUNDING_SPHERE_DESC	BoundingDesc{};

	//BoundingDesc.fRadius = 1.0f;
	//BoundingDesc.vCenter = _float3(0.f, BoundingDesc.fRadius, 0.f);

	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
	//	TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
	//	return E_FAIL;

	//m_pColliderCom->Set_Owner(this);
	//GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_PLAYER, m_pColliderCom);

	return S_OK;
}

void CCamera_TPV::PriorityTick(_float fTimeDelta)
{
	//Camera_HeadTilt_Reset(fTimeDelta);
	Camera_Roll(fTimeDelta);
}

void CCamera_TPV::Tick(_float fTimeDelta)
{
	m_pRigidbody->Update(fTimeDelta);
	Key_Input(fTimeDelta);

	/*m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());*/
}

void CCamera_TPV::LateTick(_float fTimeDelta)
{
	m_pPhysXCollider->Synchronize_Transform_Position(m_pTransformCom);
	//이 이후에 카메라 포지션 기반 이펙트 넣는건 오케이(피젝스 포지션으로 다시 돌아가서 그냥 막 넣어도 됨)
	if(PLAYER->Get_IsSlide() == true)
		m_pTransformCom->Go_Down(PLAYER->Get_SlideOffset());

	Fall_Check();
	Camera_Effect(fTimeDelta);

//#ifdef _DEBUG
//	m_pColliderCom->Render();
//#endif
}

HRESULT CCamera_TPV::Render()
{

	return S_OK;
}

void CCamera_TPV::GetPickingRay(XMVECTOR& rayOrigin, XMVECTOR& rayDir)
{
}

void CCamera_TPV::Set_Real_Position(_float3 fPos)
{
	m_pPhysXCollider->Get_Controller()->setPosition(PxExtendedVec3(fPos.x, fPos.y, fPos.z));
}

void CCamera_TPV::Camera_Effect(_float fTimeDelta)
{
	if (GAMEINSTANCE->GetKeyDown(eKeyCode::E))
	{
		m_fShakePower = { 0.1f, 0.1f };
		m_fShakeTime = 1.5f;
	}


	//Camera_HeadTilt(fTimeDelta);
	Camera_Shake(fTimeDelta);

}

void CCamera_TPV::Camera_Roll(_float fTimeDelta)
{
	POINT		ptMouse;
	POINT		ptWindow = { g_iWinSizeX >> 1, g_iWinSizeY >> 1 };


	HWND		focus_hWnd = GetFocus();
	if (focus_hWnd != NULL) {
		GetCursorPos(&ptMouse);

		ScreenToClient(g_hWnd, &ptMouse);

		_int	iMouseMove = { 0 };

		if (iMouseMove = ptMouse.x - ptWindow.x)
		{
			if (fabs(iMouseMove) > m_MoveSensitivity)
				m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 1.f), fTimeDelta * iMouseMove * m_fMouseSensor);
		}

		if (iMouseMove = ptMouse.y - ptWindow.y)
		{
			if (fabs(iMouseMove) > m_MoveSensitivity && fabs(iMouseMove) < 400.f) {

				if (m_fVerticalAngle <= m_fVerticalAngleLimit && iMouseMove > 0.f) {
					m_fVerticalAngle += iMouseMove * m_fMouseSensor;
					m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * iMouseMove * m_fMouseSensor);
				}
				else if (m_fVerticalAngle >= -m_fVerticalAngleLimit && iMouseMove < 0.f) {
					m_fVerticalAngle += iMouseMove * m_fMouseSensor;
					m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * iMouseMove * m_fMouseSensor);
				}
			}
		}
		ClientToScreen(g_hWnd, &ptWindow);

		SetCursorPos(ptWindow.x, ptWindow.y);
	}
}

void CCamera_TPV::Camera_HeadTilt(_float fTimeDelta)
{
	if (m_iRollDirection != 0)
	{
		if (m_fRollAngle < m_fRollAngleLimit && m_fRollAngle > -m_fRollAngleLimit) {
			m_fRollAngle += m_iRollDirection * fTimeDelta * 0.005f;
		}
		else {
			m_fRollAngle = m_iRollDirection * m_fRollAngleLimit;
		}

	

		m_bCameraRoll = true;
		m_pTransformCom->Head_Roll(m_fRollAngle*2.f);
	}
	
}

void CCamera_TPV::Camera_HeadTilt_Reset(_float fTimeDelta)
{
  if(m_iRollDirection != 0 && m_bCameraRoll == true)
	m_pTransformCom->Head_Roll(m_fRollAngle * -1.f);

  if (m_iRollDirection == 0 && m_bCameraRoll == true)
  {
	  m_pTransformCom->Head_Roll(m_fRollAngle * -1.f);
	  m_bCameraRoll = false;
  }
}

void CCamera_TPV::Camera_Shake(_float fTimeDelta)
{
	//RollBack Sequence
	/*if (m_fShakePowerRollBack.x != 0.f || m_fShakePowerRollBack.y != 0.f) {
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 1.f), m_fShakePowerRollBack.x);
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), m_fShakePowerRollBack.y);
		m_fShakePowerRollBack = { 0.f, 0.f };
	}*/

	//m_fShakeTime -= fTimeDelta;

	//if (m_fShakeTime < 0.f)
	//{
	//	m_fShakeTime = 0.f;
	//	m_fShakePowerRollBack = { m_fShakePower.x * -1.f , m_fShakePower.y * -1.f };
	//	m_fShakePower = { 0.f, 0.f };
	//}

	//if (m_fShakeTime == 0.f) return;

	//m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 1.f), m_fShakePower.x * GAMEINSTANCE->Random_Float(-1.f,1.f));
	//m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), m_fShakePower.y * GAMEINSTANCE->Random_Float(-1.f, 1.f));

	//m_fShakePowerRollBack = { m_fShakePower.x * -1.f , m_fShakePower.y * -1.f };




}

void CCamera_TPV::Key_Input(_float fTimeDelta)
{
	if (m_pGameInstance->GetKey(eKeyCode::R))
	{
		if(GAMEINSTANCE->Get_CurrentLevelIndex() == LEVEL_GAMEPLAY)
			m_pPhysXCollider->Get_Controller()->setPosition(PxExtendedVec3(60.f, 20.f, -55.f));

		if (GAMEINSTANCE->Get_CurrentLevelIndex() == LEVEL_LAB)
			m_pPhysXCollider->Get_Controller()->setPosition(PxExtendedVec3(1049.83f, 17.67f, -321.63f));

		m_pPhysXCollider->Synchronize_Transform_Position(m_pTransformCom);

		PLAYER->GameRestart();
	}

	if (PLAYER->Get_IsDie() != true) {

		m_pRigidbody->Set_Friction(8.f);

		PxVec3 velocity = { 0.f, 0.f, 0.f };
		m_pRigidbody->Set_Gravity({ 0.f, -0.3f, 0.f });

		if (PLAYER->Get_PlayerState() != CPlayer_Manager::PLAYER_WIREACTION) {
			if (m_pGameInstance->GetKey(eKeyCode::W) && PLAYER->Get_PlayerDash() != true)
			{
				m_pRigidbody->Add_GroundVelocity(GAMEINSTANCE->Add_PxVec3toXMVECTOR(velocity, m_pTransformCom->Get_PureStraight() * fTimeDelta * 2.f));
				if (PLAYER->Get_PlayerState() == CPlayer_Manager::PLAYER_IDLE) PLAYER->Set_PlayerState(CPlayer_Manager::PLAYER_MOVE);
			}

			if (m_pGameInstance->GetKey(eKeyCode::S) && PLAYER->Get_PlayerDash() != true)
			{
				m_pRigidbody->Add_GroundVelocity(GAMEINSTANCE->Add_PxVec3toXMVECTOR(velocity, m_pTransformCom->Get_PureStraight() * -fTimeDelta * 2.f));
				if (PLAYER->Get_PlayerState() == CPlayer_Manager::PLAYER_IDLE) PLAYER->Set_PlayerState(CPlayer_Manager::PLAYER_MOVE);
			}

			if (m_pGameInstance->GetKey(eKeyCode::A))
			{
				if(PLAYER->Get_PlayerDash())
					m_pRigidbody->Add_GroundVelocity(GAMEINSTANCE->Add_PxVec3toXMVECTOR(velocity, m_pTransformCom->Get_PureRight() * -fTimeDelta * 1.f));
				else
					m_pRigidbody->Add_GroundVelocity(GAMEINSTANCE->Add_PxVec3toXMVECTOR(velocity, m_pTransformCom->Get_PureRight() * -fTimeDelta * 2.f));

				if (PLAYER->Get_PlayerState() == CPlayer_Manager::PLAYER_IDLE) PLAYER->Set_PlayerState(CPlayer_Manager::PLAYER_MOVE);
			}

			if (m_pGameInstance->GetKey(eKeyCode::D))
			{
				if (PLAYER->Get_PlayerDash())
					m_pRigidbody->Add_GroundVelocity(GAMEINSTANCE->Add_PxVec3toXMVECTOR(velocity, m_pTransformCom->Get_PureRight() * fTimeDelta * 1.f));
				else
					m_pRigidbody->Add_GroundVelocity(GAMEINSTANCE->Add_PxVec3toXMVECTOR(velocity, m_pTransformCom->Get_PureRight() * fTimeDelta * 2.f));

				if (PLAYER->Get_PlayerState() == CPlayer_Manager::PLAYER_IDLE) PLAYER->Set_PlayerState(CPlayer_Manager::PLAYER_MOVE);
			}
		}
		//if (GAMEINSTANCE->GetKeyDown(eKeyCode::RButton))
		//{
		//	//
		//}

		if (m_pGameInstance->GetKeyDown(eKeyCode::LShift))
			PLAYER->Set_PlayerDashStart(true);

		if (m_pGameInstance->GetKeyDown(eKeyCode::J))
			m_fVerticalAngle = 0.f;


		if (m_pGameInstance->GetKeyUp(eKeyCode::LShift)) {
			m_bDash = true;
		}
		
		if (m_bDash == true) {
			_float3 vForward;
			XMStoreFloat3(&vForward, XMVector3Normalize(GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_LOOK)));
			PxVec3 vForwardPx = PxVec3(vForward.x, vForward.y, vForward.z) * fTimeDelta * 20.f;

			m_pPhysXCollider->Get_Controller()->move(vForwardPx, 0.01f, fTimeDelta, PxControllerFilters());

			m_fDashTime+= fTimeDelta;
			if (m_fDashTime > m_fDashTimeMax)
			{
				m_bDash = false;
				m_fDashTime = 0.f;
			}
		}

		if (m_pGameInstance->GetKeyDown(eKeyCode::LCtrl) && m_pRigidbody->IsGround())
		{
			GAMEINSTANCE->Play(TEXT("Sound_Player_Slide"), false);
			GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Slide"), 1.f);
			
			PLAYER->Set_IsSlide(true);
			PLAYER->Add_Velocity_Multiplier(0.35f);
		}

		if (m_pGameInstance->GetKeyUp(eKeyCode::LCtrl))
		{
			GAMEINSTANCE->Stop(TEXT("Sound_Player_Slide"));
			PLAYER->Set_IsSlide(false);
		}

		if (GAMEINSTANCE->GetKeyDown(eKeyCode::Enter)) {
			float x = static_cast<float>(m_pPhysXCollider->Get_Controller()->getPosition().x);
			float y = static_cast<float>(m_pPhysXCollider->Get_Controller()->getPosition().y);
			float z = static_cast<float>(m_pPhysXCollider->Get_Controller()->getPosition().z);
			int i = 0;
		}

		if (GAMEINSTANCE->GetKeyDown(eKeyCode::LButton))
		{
			if (PLAYER->Get_IsAttack() == false) {
				switch (GAMEINSTANCE->Random_Int(1, 4))
				{
				case 1:
					GAMEINSTANCE->Play(TEXT("Sound_Player_Katana_01"), false);
					GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Katana_01"), 1.f);
					break;
				case 2:
					GAMEINSTANCE->Play(TEXT("Sound_Player_Katana_02"), false);
					GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Katana_02"), 1.f);
					break;
				case 3:
					GAMEINSTANCE->Play(TEXT("Sound_Player_Katana_03"), false);
					GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Katana_03"), 1.f);
					break;
				case 4:
					GAMEINSTANCE->Play(TEXT("Sound_Player_Katana_04"), false);
					GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Katana_04"), 1.f);
					break;
				}
			}
			PLAYER->Set_IsAttack(true);
		}

		if (m_pGameInstance->GetKeyDown(eKeyCode::Space) && m_pRigidbody->IsGround())
		{
			_int iRand = GAMEINSTANCE->Random_Int(1, 3);

			switch (iRand)
			{
			case 1:
				GAMEINSTANCE->Play(TEXT("Sound_Player_Jump_01"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Jump_01"), 1.f);
				break;
			case 2:
				GAMEINSTANCE->Play(TEXT("Sound_Player_Jump_02"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Jump_02"), 1.f);
				break;
			case 3:
				GAMEINSTANCE->Play(TEXT("Sound_Player_Jump_03"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Player_Jump_03"), 1.f);
				break;


			}

			m_pRigidbody->Set_VelocityY(0.15f);
			PLAYER->Add_Velocity_Multiplier(0.25f);
			m_pRigidbody->Set_Friction(2.f);
			m_pRigidbody->Set_Ground(false);
			PLAYER->Set_IsJump(true);
			PLAYER->Set_PlayerState(CPlayer_Manager::PLAYER_JUMP);
			PLAYER->Set_JumpState(CPlayer_Manager::JUMP_START);
		}



		m_pPhysXCollider->checkCollisionFlags(m_pPhysXCollider->Get_Controller()->move(m_pRigidbody->Get_Velocity() * (PLAYER->Get_Velocity_Multiplier() + 1.f), 0.01f, fTimeDelta, PxControllerFilters()));

		if (true == m_pPhysXCollider->Get_CollisionDown()) {
			m_pRigidbody->Set_Ground(true);
			if (PLAYER->Get_IsJump() == true) {
				PLAYER->Set_IsJump(false);
				PLAYER->Get_PlayerArm()->Hard_Animation_Change(17, false, 1.5f);
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
			PLAYER->Set_JumpState(CPlayer_Manager::JUMP_FINISH);
			m_pRigidbody->Set_Friction(6.f);
		}

		if (true == m_pPhysXCollider->Get_CollisionRight()) {
			m_pRigidbody->Add_GroundVelocity(GAMEINSTANCE->Add_PxVec3toXMVECTOR(velocity, m_pTransformCom->Get_PureRight() * fTimeDelta * 0.4f));
			m_pRigidbody->Set_Ground(true);
			PLAYER->Set_IsJump(false);
			PLAYER->Set_IsWallRun(true);
			m_pRigidbody->Set_VelocityY(0.015f);
		}

		if (true == m_pPhysXCollider->Get_CollisionSides() && GAMEINSTANCE->GetKey(eKeyCode::A)) {

			m_iRollDirection = -1;

			m_pRigidbody->Set_Grab(CRigidbody::WIRE_END);
			PLAYER->Set_WireState(CPlayer_Manager::WIRE_END);

			if (m_bCameraRoll == true) {
				m_pTransformCom->Head_Roll(-1.f * m_fRollAngle * m_iRollDirection);
			}

			if (m_fRollAngle < m_fRollAngleLimit)
				m_fRollAngle += fTimeDelta * 3.f;
			else
				m_fRollAngle = m_fRollAngleLimit;

			m_pTransformCom->Head_Roll(m_fRollAngle * m_iRollDirection);
			PLAYER->Set_PlayerState(CPlayer_Manager::PLAYER_MOVE);
			PLAYER->Set_WallRunState(CPlayer_Manager::WALLRUN_LEFT);
			m_bCameraRoll = true;
		}

		if (true == m_pPhysXCollider->Get_CollisionSides() && GAMEINSTANCE->GetKey(eKeyCode::D)) {
			m_iRollDirection = 1;

			m_pRigidbody->Set_Grab(CRigidbody::WIRE_END);
			PLAYER->Set_WireState(CPlayer_Manager::WIRE_END);
			if (m_bCameraRoll == true) {
				m_pTransformCom->Head_Roll(-1.f * m_fRollAngle * m_iRollDirection);
			}

			if (m_fRollAngle < m_fRollAngleLimit)
				m_fRollAngle += fTimeDelta * 3.f;
			else
				m_fRollAngle = m_fRollAngleLimit;

			m_pTransformCom->Head_Roll(m_fRollAngle * m_iRollDirection);
			PLAYER->Set_PlayerState(CPlayer_Manager::PLAYER_MOVE);
			PLAYER->Set_WallRunState(CPlayer_Manager::WALLRUN_RIGHT);
			m_bCameraRoll = true;
		}




		if (true == m_pPhysXCollider->Get_CollisionLeft()) {
			m_pRigidbody->Add_GroundVelocity(GAMEINSTANCE->Add_PxVec3toXMVECTOR(velocity, m_pTransformCom->Get_PureRight() * -fTimeDelta * 0.4f));
			m_pRigidbody->Set_Ground(true);
			PLAYER->Set_IsJump(false);
			PLAYER->Set_IsWallRun(true);
			m_pRigidbody->Set_VelocityY(0.015f);
		}

		if (false == m_pPhysXCollider->Get_CollisionSides())
		{
			if (PLAYER->Get_IsWallRun() == true) {
				PLAYER->Set_IsWallRun(false);
				if (PLAYER->Get_IsJump() == false) {
					m_pRigidbody->Set_VelocityY(0.15f);
					PLAYER->Add_Velocity_Multiplier(0.2f);
					m_pRigidbody->Set_Friction(2.f);
					m_pRigidbody->Set_Ground(false);

					m_pTransformCom->Set_State(CTransform::STATE_UP, XMVectorSet(0.f, 1.f, 0.f, 0.f));
					m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(XMVector3Cross(m_pTransformCom->Get_State(CTransform::STATE_UP), m_pTransformCom->Get_State(CTransform::STATE_LOOK))));
					m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3Normalize(XMVector3Cross(m_pTransformCom->Get_State(CTransform::STATE_LOOK), m_pTransformCom->Get_State(CTransform::STATE_RIGHT))));

					m_bCameraRoll = false;
					m_fRollAngle = 0.f;
					PLAYER->Set_PlayerState(CPlayer_Manager::PLAYER_JUMP);
					PLAYER->Set_JumpState(CPlayer_Manager::JUMP_START);
					PLAYER->Set_WallRunState(CPlayer_Manager::WALLRUN_NO);
					PLAYER->Set_IsJump(true);
				}
			}
		}

	}
}

void CCamera_TPV::Fall_Check()
{
	if (GAMEINSTANCE->Get_CurrentLevelIndex() == LEVEL_GAMEPLAY && m_pPhysXCollider->Get_Controller()->getPosition().y < -60.f)
	{
		PLAYER->Set_IsDie(true);
	}
}

CCamera_TPV* CCamera_TPV::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_TPV* pInstance = new CCamera_TPV(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCamera_TPV"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CCamera_TPV::Clone(void* pArg)
{
	CCamera_TPV* pInstance = new CCamera_TPV(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCamera_TPV"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CCamera_TPV::Free()
{
	__super::Free();
	Safe_Release(m_pPhysXCollider);
	Safe_Release(m_pRigidbody);

}
