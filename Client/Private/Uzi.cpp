#include "Uzi.h"
#include "Mesh.h"
#include "GameInstance.h"
#include "PlayerManager.h"
#include "Collision_Manager.h"
#include "Pistol.h"
#include "Bullet.h"
#include "ObjPoolManager.h"

CUzi::CUzi(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CUzi::CUzi(const CUzi& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CUzi::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUzi::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Set_MainTag("Monster");
	Set_SubTag("Uzi");

	m_pTransformCom->Set_Scale({ 0.01f, 0.01f, 0.01f });
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimIndex(0, true);


	/*CPistol::WEAPON_DESC TransformDesc = {};
	TransformDesc.pParentTransform = m_pTransformCom;
	TransformDesc.pBoneMatrix = m_pModelCom->Get_BoneMatrixPtr("Gun_r");
	TransformDesc.fSpeedPerSec = 0.f;
	TransformDesc.fRotationPerSec = 0.f;

	CPartObject* pistol = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_PistolGunnerWeapon"), &TransformDesc));

	if (pistol == nullptr)
		return E_FAIL;

	m_vecParts.emplace_back(pistol);*/

	return S_OK;
}

void CUzi::PriorityTick(_float fTimeDelta)
{
	for (auto& pParts : m_vecParts)
		pParts->PriorityTick(fTimeDelta);

	if(GAMEINSTANCE->GetKeyDown(eKeyCode::Right))
		m_pModelCom->Set_AnimIndex(m_pModelCom->Get_CurrentAnimIndex() + 1, true);

}

void CUzi::Tick(_float fTimeDelta)
{
	if (m_bDeathCheck == false && m_eUziState == UZI_DIE) {
		m_bDeathCheck = true;

		CTransform::TRANSFORM_DESC	TransformDesc{};
		XMStoreFloat4(&TransformDesc.vPosition, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) + 1.2f));

		m_pGameInstance->Add_Clone(GAMEINSTANCE->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Blood"), &TransformDesc);
	}


	if (GAMEINSTANCE->Get_CurrentLevelIndex() != LEVEL_TOOL) {
		State_Machine(fTimeDelta);
		//Get_DeathCheck();
		//if (GAMEINSTANCE->GetKeyDown(eKeyCode::Right))
		//	m_pModelCom->Set_AnimIndex(m_pModelCom->Get_CurrentAnimIndex() + 1, true);

		m_pPhysXCollider->Synchronize_Transform_Position(m_pTransformCom);

		if (m_eUziState == UZI_ATTACK) {
			m_pTransformCom->LookAt(GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_POSITION));
			m_pTransformCom->PureTurn(XMVectorSet(0.f, 1.f, 0.f, 0.f), 3.141592f);
		}
	}
		m_pModelCom->Play_Animation(fTimeDelta);

		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

		for (auto& pParts : m_vecParts)
			pParts->Tick(fTimeDelta);
}

void CUzi::LateTick(_float fTimeDelta)
{
	if (m_pColliderCom->IsPicked() && PLAYER->Get_PlayerDash() && PLAYER->Get_SkillUsable())
		m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_GLOW, this);
	else
		m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);

	for (auto& pParts : m_vecParts)
		pParts->LateTick(fTimeDelta);

}

HRESULT CUzi::Render()
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

void CUzi::Collision_Enter(CGameObject* pOpponent)
{
	if (pOpponent->Get_MainTag() == "Player" && PLAYER->Get_IsAttack() && m_eUziState != UZI_DIE) {
		m_eUziState = UZI_DIE;
		Hard_Animation_Change(2, false); 
		PLAYER->Set_SkillGauge(PLAYER->Get_SkillGauge() + 30.f);

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

		GAMEINSTANCE->Set_ShakePower(1.f);
		GAMEINSTANCE->Set_ShakeTime(1.f);
	}

}

HRESULT CUzi::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Uzi"),
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
		string Uzi = "UziGunner";
		string tmp = to_string(i);

		Uzi += tmp;

	}

	_float3 m_vPosition = {};
	XMStoreFloat3(&m_vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pPhysXCollider->createCharacterController(CGameInstance::Get_Instance()->Get_PhysX_ControllerManager(), PxVec3(m_vPosition.x, m_vPosition.y, m_vPosition.z));

	return S_OK;
}

HRESULT CUzi::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CUzi::Model_Rendering(_int iMeshNum)
{
	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", (_uint)iMeshNum, aiTextureType_DIFFUSE)))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pModelCom->Render((_uint)iMeshNum);

	return S_OK;

}

void CUzi::State_Machine(_float fTimeDelta)
{

	//if (GAMEINSTANCE->GetKeyDown(eKeyCode::T)) {
	//	/*CTransform::TRANSFORM_DESC			TransformDesc{};
	//	XMStoreFloat4(&TransformDesc.vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	//	_float fTempY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	//	fTempY += 1.5f;
	//	TransformDesc.vPosition.y = fTempY;

	//	TransformDesc.fSpeedPerSec = 14.f;
	//	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	//	CBullet* Bullet = dynamic_cast<CBullet*>(m_pGameInstance->Add_Clone_Return(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Bullet"), &TransformDesc));
	//	Bullet->Set_State(CBullet::BULLET_FIRE);*/

	//	CBullet::BULLET_DESC TransformDesc = {};
	//	TransformDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	//	_float fTempY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	//	fTempY += 1.5f;
	//	XMVectorSetY(TransformDesc.vPosition, fTempY);
	//	TransformDesc.fSpeed = 21.f;
	//	TransformDesc.vDirection = GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_POSITION);
	//	CONTAINER->BulletAwake(&TransformDesc);
	//}

	if (GAMEINSTANCE->GetKeyDown(eKeyCode::R)) {
		m_eUziState = UZI_IDLE;
		Set_TempDead(false);
		m_bDeathCheck = false;
	}

	switch (m_eUziState)
	{
	case UZI_IDLE:
		State_Idle(fTimeDelta);
		break;
	case UZI_SEARCH:
		//State_Search(fTimeDelta);
		break;
	case UZI_ATTACK:
		State_Attack(fTimeDelta);
		break;
	case UZI_RELOAD:
		State_Reload(fTimeDelta);
		break;
	case UZI_DIE:
		State_Die(fTimeDelta);
		break;
	}
}

void CUzi::State_Idle(_float fTimeDelta)
{
	Soft_Animation_Change(1, true);
	if (PLAYER->Get_PlayerToTargetDistance(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) < 20.f && PLAYER->Get_PlayerState() != CPlayer_Manager::PLAYER_DIE) {
		
		if (m_bFirstAttack) {

			switch (GAMEINSTANCE->Random_Int(1, 10))
			{
			case 1:
				m_pGameInstance->Play(TEXT("Sound_Uzi_Alert_01"), false);
				m_pGameInstance->SetVolume(TEXT("Sound_Uzi_Alert_01"), 1.f);
				break;
			case 2:
				m_pGameInstance->Play(TEXT("Sound_Uzi_Alert_02"), false);
				m_pGameInstance->SetVolume(TEXT("Sound_Uzi_Alert_02"), 1.f);
				break;
			case 3:
				m_pGameInstance->Play(TEXT("Sound_Uzi_Alert_03"), false);
				m_pGameInstance->SetVolume(TEXT("Sound_Uzi_Alert_03"), 1.f);
				break;
			case 4:
				m_pGameInstance->Play(TEXT("Sound_Uzi_Alert_04"), false);
				m_pGameInstance->SetVolume(TEXT("Sound_Uzi_Alert_04"), 1.f);
				break;
			case 5:
				m_pGameInstance->Play(TEXT("Sound_Uzi_Alert_05"), false);
				m_pGameInstance->SetVolume(TEXT("Sound_Uzi_Alert_05"), 1.f);
				break;
			case 6:
				m_pGameInstance->Play(TEXT("Sound_Uzi_Alert_06"), false);
				m_pGameInstance->SetVolume(TEXT("Sound_Uzi_Alert_06"), 1.f);
				break;
			case 7:
				m_pGameInstance->Play(TEXT("Sound_Uzi_Alert_07"), false);
				m_pGameInstance->SetVolume(TEXT("Sound_Uzi_Alert_07"), 1.f);
				break;
			case 8:
				m_pGameInstance->Play(TEXT("Sound_Uzi_Alert_08"), false);
				m_pGameInstance->SetVolume(TEXT("Sound_Uzi_Alert_08"), 1.f);
				break;
			case 9:
				m_pGameInstance->Play(TEXT("Sound_Uzi_Alert_09"), false);
				m_pGameInstance->SetVolume(TEXT("Sound_Uzi_Alert_09"), 1.f);
				break;
			case 10:
				m_pGameInstance->Play(TEXT("Sound_Uzi_Alert_10"), false);
				m_pGameInstance->SetVolume(TEXT("Sound_Uzi_Alert_10"), 1.f);
				break;
			}
			m_bFirstAttack = false;
		}

		if (m_iBulletCnt > 0) {

			switch (GAMEINSTANCE->Random_Int(1, 4))
			{
			case 1:
				m_pGameInstance->Play(TEXT("Sound_Uzi_Shot_01"), false);
				m_pGameInstance->SetVolume(TEXT("Sound_Uzi_Shot_01"), 1.f);
				break;
			case 2:
				m_pGameInstance->Play(TEXT("Sound_Uzi_Shot_02"), false);
				m_pGameInstance->SetVolume(TEXT("Sound_Uzi_Shot_02"), 1.f);
				break;
			case 3:
				m_pGameInstance->Play(TEXT("Sound_Uzi_Shot_03"), false);
				m_pGameInstance->SetVolume(TEXT("Sound_Uzi_Shot_03"), 1.f);
				break;
			case 4:
				m_pGameInstance->Play(TEXT("Sound_Uzi_Shot_04"), false);
				m_pGameInstance->SetVolume(TEXT("Sound_Uzi_Shot_04"), 1.f);
				break;
			}

			CBullet::BULLET_DESC TransformDesc = {};
			TransformDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			_float fTempY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			fTempY += 1.5f;
			TransformDesc.vPosition = XMVectorSetY(TransformDesc.vPosition, fTempY);
			TransformDesc.fSpeed = 42.f;
			TransformDesc.vDirection = GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_POSITION);
			TransformDesc.bColor = true;
			CONTAINER->BulletAwake(&TransformDesc);


			m_eUziState = UZI_ATTACK;
			m_iBulletCnt--;
		}
		else {
			switch (GAMEINSTANCE->Random_Int(1, 5))
			{
			case 1:
				GAMEINSTANCE->Play(TEXT("Sound_Uzi_Reload_01"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Uzi_Reload_01"), 1.f);
				break;
			case 2:
				GAMEINSTANCE->Play(TEXT("Sound_Uzi_Reload_02"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Uzi_Reload_02"), 1.f);
				break;
			case 3:
				GAMEINSTANCE->Play(TEXT("Sound_Uzi_Reload_03"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Uzi_Reload_03"), 1.f);
				break;
			case 4:
				GAMEINSTANCE->Play(TEXT("Sound_Uzi_Reload_04"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Uzi_Reload_04"), 1.f);
				break;
			case 5:
				GAMEINSTANCE->Play(TEXT("Sound_Uzi_Reload_05"), false);
				GAMEINSTANCE->SetVolume(TEXT("Sound_Uzi_Reload_05"), 1.f);
				break;
			}

			m_eUziState = UZI_RELOAD;
		}
	}
}

void CUzi::State_Attack(_float fTimeDelta)
{
	Hard_Animation_Change(2, false, 3.f);
	if (m_pModelCom->Get_Finished() == true)
		m_eUziState = UZI_IDLE;
}

void CUzi::State_Reload(_float fTimeDelta)
{
	Hard_Animation_Change(3, false);
	if (m_pModelCom->Get_Finished() == true) {
		m_iBulletCnt = m_iBulletMaxCnt;
		m_eUziState = UZI_IDLE;
	}
}

void CUzi::State_Die(_float fTimeDelta)
{
	Set_TempDead(true);
	Hard_Animation_Change(0, false);
}

void CUzi::Get_DeathCheck()
{
	//if (PLAYER->Get_PlayerToTargetDistance(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) < 2.5f && (PLAYER->Get_IsAttack() == true) && m_ePistolGunnerState != PISTOLGUNNER_DIE) {
	//	m_ePistolGunnerState = PISTOLGUNNER_DIE;
	//	//Hard_Animation_Change(1, false);
	//}
}

void CUzi::Soft_Animation_Change(_uint _iAnimationNum, _bool _bLoop, _float _bSpeedMultiply)
{
	if (m_pModelCom->Get_CurrentAnimIndex() == _iAnimationNum)
		return;

	if (m_pModelCom->Get_Finished() == true) {
		m_pModelCom->Set_AnimIndex(_iAnimationNum, _bLoop);
		m_pModelCom->Set_AnimSpeed(_bSpeedMultiply);
		m_pModelCom->Set_Finished(false);
	}
}

void CUzi::Hard_Animation_Change(_uint _iAnimationNum, _bool _bLoop, _float _bSpeedMultiply)
{
	if (m_pModelCom->Get_CurrentAnimIndex() == _iAnimationNum)
		return;

	m_pModelCom->Set_AnimIndex(_iAnimationNum, _bLoop);
	m_pModelCom->Set_AnimSpeed(_bSpeedMultiply);
	m_pModelCom->Set_Finished(false);
}

CUzi* CUzi::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUzi* pInstance = new CUzi(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUzi"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CUzi::Clone(void* pArg)
{
	CUzi* pInstance = new CUzi(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUzi"));
		Safe_Release(pInstance);
	}

	GAMEINSTANCE->Imgui_SetSelectObject(pInstance);

	return pInstance;
}

void CUzi::Free()
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
