#include "Waver.h"
#include "Mesh.h"
#include "GameInstance.h"
#include "PlayerManager.h"
#include "Collision_Manager.h"
#include "ObjPoolManager.h"
#include "Pistol.h"
#include "Bullet.h"
#include "Beam.h"

CWaver::CWaver(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CWaver::CWaver(const CWaver& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CWaver::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWaver::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Set_MainTag("Monster");
	Set_SubTag("Waver");

	m_pTransformCom->Set_Scale({ 0.02f, 0.02f, 0.02f });
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

void CWaver::PriorityTick(_float fTimeDelta)
{
	for (auto& pParts : m_vecParts)
		pParts->PriorityTick(fTimeDelta);
}

void CWaver::Tick(_float fTimeDelta)
{
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

void CWaver::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);

	for (auto& pParts : m_vecParts)
		pParts->LateTick(fTimeDelta);

}

HRESULT CWaver::Render()
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

void CWaver::Collision_Enter(CGameObject* pOpponent)
{
	if (pOpponent->Get_MainTag() == "Player" && PLAYER->Get_IsAttack() && m_ePistolGunnerState != PISTOLGUNNER_DIE) {
		m_ePistolGunnerState = PISTOLGUNNER_DIE;

		switch (GAMEINSTANCE->Random_Int(1, 4))
		{
		case 1:
			m_pGameInstance->Play(TEXT("Sound_Waver_Death_01"), false);
			m_pGameInstance->SetVolume(TEXT("Sound_Waver_Death_01"), 0.5f);
			break;
		case 2:
			m_pGameInstance->Play(TEXT("Sound_Waver_Death_02"), false);
			m_pGameInstance->SetVolume(TEXT("Sound_Waver_Death_02"), 0.5f);
			break;
		case 3:
			m_pGameInstance->Play(TEXT("Sound_Waver_Death_03"), false);
			m_pGameInstance->SetVolume(TEXT("Sound_Waver_Death_03"), 0.5f);
			break;
		case 4:
			m_pGameInstance->Play(TEXT("Sound_Waver_Death_04"), false);
			m_pGameInstance->SetVolume(TEXT("Sound_Waver_Death_04"), 0.5f);
			break;
		}

		Hard_Animation_Change(2, false);
		PLAYER->Set_SkillGauge(PLAYER->Get_SkillGauge() + 35.f);
	}

}

HRESULT CWaver::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Waver"),
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

HRESULT CWaver::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CWaver::Model_Rendering(_int iMeshNum)
{
	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", (_uint)iMeshNum, aiTextureType_DIFFUSE)))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pModelCom->Render((_uint)iMeshNum);

	return S_OK;

}

void CWaver::State_Machine(_float fTimeDelta)
{

	if (GAMEINSTANCE->GetKeyDown(eKeyCode::T)) {

		CBullet::BULLET_DESC TransformDesc = {};
		TransformDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		_float fTempY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		fTempY += 1.5f;
		XMVectorSetY(TransformDesc.vPosition, fTempY);
		TransformDesc.fSpeed = 18.f;
		TransformDesc.vDirection = GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_POSITION);
		CONTAINER->BulletAwake(&TransformDesc);

	}

	if (GAMEINSTANCE->GetKeyDown(eKeyCode::R)) {
		m_ePistolGunnerState = PISTOLGUNNER_IDLE;
		Set_TempDead(false);
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

void CWaver::State_Idle(_float fTimeDelta)
{
	Soft_Animation_Change(3, true);
	if (PLAYER->Get_PlayerToTargetDistance(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) < 20.f && PLAYER->Get_PlayerState() != CPlayer_Manager::PLAYER_DIE) {

		if (m_bFirstAttack) {

			m_pGameInstance->Play(TEXT("Sound_Waver_Alert"), false);
			m_pGameInstance->SetVolume(TEXT("Sound_Waver_Alert"), 1.f);
			m_bFirstAttack = false;
		}

		switch (GAMEINSTANCE->Random_Int(1, 4))
		{
		case 1:
			m_pGameInstance->Play(TEXT("Sound_Waver_Shot_01"), false);
			m_pGameInstance->SetVolume(TEXT("Sound_Waver_Shot_01"), 1.f);
			break;
		case 2:
			m_pGameInstance->Play(TEXT("Sound_Waver_Shot_02"), false);
			m_pGameInstance->SetVolume(TEXT("Sound_Waver_Shot_02"), 1.f);
			break;
		case 3:
			m_pGameInstance->Play(TEXT("Sound_Waver_Shot_03"), false);
			m_pGameInstance->SetVolume(TEXT("Sound_Waver_Shot_03"), 1.f);
			break;
		case 4:
			m_pGameInstance->Play(TEXT("Sound_Waver_Shot_04"), false);
			m_pGameInstance->SetVolume(TEXT("Sound_Waver_Shot_04"), 1.f);
			break;
		}


		CBeam::BULLET_DESC TransformDesc = {};
		TransformDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		_float fTempY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		fTempY += 1.5f;
		TransformDesc.vPosition = XMVectorSetY(TransformDesc.vPosition, fTempY);
		TransformDesc.fSpeed = 18.f;

		for (_int i = -5; i < 5; ++i) {
			TransformDesc.vDirection = XMVectorAdd(GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_POSITION), XMVector4Normalize(GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_RIGHT)) * static_cast<float>(i));
			CONTAINER->BeamAwake(&TransformDesc);
		}

		m_ePistolGunnerState = PISTOLGUNNER_ATTACK;
	}
}

void CWaver::State_Attack(_float fTimeDelta)
{
	Hard_Animation_Change(1, false, 0.5f);
	if (m_pModelCom->Get_Finished() == true)
		m_ePistolGunnerState = PISTOLGUNNER_IDLE;
}

void CWaver::State_Die(_float fTimeDelta)
{
	Hard_Animation_Change(2, false);
	Set_TempDead(true);
}

void CWaver::Get_DeathCheck()
{
	//if (PLAYER->Get_PlayerToTargetDistance(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) < 2.5f && (PLAYER->Get_IsAttack() == true) && m_ePistolGunnerState != PISTOLGUNNER_DIE) {
	//	m_ePistolGunnerState = PISTOLGUNNER_DIE;
	//	//Hard_Animation_Change(1, false);
	//}
}

void CWaver::Soft_Animation_Change(_uint _iAnimationNum, _bool _bLoop, _float _bSpeedMultiply)
{
	if (m_pModelCom->Get_CurrentAnimIndex() == _iAnimationNum)
		return;

	if (m_pModelCom->Get_Finished() == true) {
		m_pModelCom->Set_AnimIndex(_iAnimationNum, _bLoop);
		m_pModelCom->Set_AnimSpeed(_bSpeedMultiply);
		m_pModelCom->Set_Finished(false);
	}
}

void CWaver::Hard_Animation_Change(_uint _iAnimationNum, _bool _bLoop, _float _bSpeedMultiply)
{
	if (m_pModelCom->Get_CurrentAnimIndex() == _iAnimationNum)
		return;

	m_pModelCom->Set_AnimIndex(_iAnimationNum, _bLoop);
	m_pModelCom->Set_AnimSpeed(_bSpeedMultiply);
	m_pModelCom->Set_Finished(false);
}

CWaver* CWaver::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWaver* pInstance = new CWaver(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWaver"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CWaver::Clone(void* pArg)
{
	CWaver* pInstance = new CWaver(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWaver"));
		Safe_Release(pInstance);
	}

	GAMEINSTANCE->Imgui_SetSelectObject(pInstance);

	return pInstance;
}

void CWaver::Free()
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
