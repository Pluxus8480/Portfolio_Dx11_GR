#include "PlayerArm.h"
#include "Mesh.h"
#include "GameInstance.h"
#include "PlayerManager.h"
#include "Wire.h"
#include "Katana.h"

CPlayerArm::CPlayerArm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CPlayerArm::CPlayerArm(const CPlayerArm& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CPlayerArm::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerArm::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_pModelCom->Set_AnimIndex(0, true);

	CKatana::WEAPON_DESC TransformDesc = {};
	TransformDesc.pParentTransform = m_pTransformCom;
	TransformDesc.pBoneMatrix = m_pModelCom->Get_BoneMatrixPtr("Weapon_r");
	TransformDesc.fSpeedPerSec = 0.f;
	TransformDesc.fRotationPerSec = 0.f;
	TransformDesc.pWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();

	CPartObject* katana = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Katana"), &TransformDesc));

	if (katana == nullptr)
		return E_FAIL;

	m_vecParts.emplace_back(katana);

	CWire::WIRE_DESC WireDesc = {};
	WireDesc.pBoneMatrix = m_pModelCom->Get_BoneMatrixPtr("Weapon_l");
	WireDesc.pTarget = m_pTransformCom->Get_WorldFloat4x4();

	m_pWire = dynamic_cast<CWire*>(GAMEINSTANCE->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Effect"),TEXT("Prototype_Wire_Effect"), &WireDesc));

	return S_OK;
}

void CPlayerArm::PriorityTick(_float fTimeDelta)
{
	m_pTransformCom->Set_WorldMatrix(GAMEINSTANCE->Get_CurCamera()->Get_WorldMatrix_4x4());


	m_pTransformCom->Set_Scale({ 0.01f,0.01f,0.01f });
	/*m_pTransformCom->PureTurn(m_pTransformCom->Get_Up(), 180.f);

	m_pTransformCom->Go_Straight(0.f);
	m_pTransformCom->Go_Right(0.2f);
	m_pTransformCom->Go_Down(1.4f);*/

	/*if(GAMEINSTANCE->GetKey(eKeyCode::Y))
		m_fRadian += fTimeDelta;

	if (GAMEINSTANCE->GetKey(eKeyCode::P))
		m_fRadian -= fTimeDelta;

	if (GAMEINSTANCE->GetKey(eKeyCode::L))
		m_fRight += fTimeDelta;

	if (GAMEINSTANCE->GetKey(eKeyCode::J))
		m_fRight -= fTimeDelta;

	if (GAMEINSTANCE->GetKey(eKeyCode::I))
		m_fFront += fTimeDelta;

	if (GAMEINSTANCE->GetKey(eKeyCode::K))
		m_fFront -= fTimeDelta;

	if (GAMEINSTANCE->GetKey(eKeyCode::U))
		m_fUp += fTimeDelta;

	if (GAMEINSTANCE->GetKey(eKeyCode::O))
		m_fUp -= fTimeDelta;*/


	m_pTransformCom->PureTurn(m_pTransformCom->Get_Up(), m_fRadian);

	m_pTransformCom->Go_Straight(m_fFront);
	m_pTransformCom->Go_Right(m_fRight);
	m_pTransformCom->Go_Down(m_fUp);

	/*if(GAMEINSTANCE->GetKeyDown(eKeyCode::One))
		MSG_BOX(TEXT("Front : %f, Right : %f, Up : %f"), m_fFront, m_fRight, m_fUp);*/

	for (auto& pParts : m_vecParts)
		pParts->PriorityTick(fTimeDelta);


}

void CPlayerArm::Tick(_float fTimeDelta)
{
	/*if (GAMEINSTANCE->GetKeyDown(eKeyCode::Right))
		m_pModelCom->Set_AnimIndex(m_pModelCom->Get_CurrentAnimIndex() + 1, true);*/

	m_pModelCom->Play_Animation(fTimeDelta);

	if (PLAYER->Get_PlayerState() == CPlayer_Manager::PLAYER_WIREACTION && PLAYER->Get_WireState() == CPlayer_Manager::WIRE_LOOP) {
		m_bWireOn = true;
		m_pWire->Set_BoneMatrix(PLAYER->Get_GrabTarget()->Get_Transform()->Get_WorldFloat4x4());
		m_pWire->Set_TrailOn(true);
	}

	if (m_bWireOn && PLAYER->Get_WireState() != CPlayer_Manager::WIRE_LOOP) {
		m_bWireOn = false;
		m_pWire->Set_TrailOn(false);
		m_pWire->Clear_BoneMatrix();
	}

	for (auto& pParts : m_vecParts)
		pParts->Tick(fTimeDelta);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CPlayerArm::LateTick(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);

	for (auto& pParts : m_vecParts)
		pParts->LateTick(fTimeDelta);
}

HRESULT CPlayerArm::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CCamera_Manager::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CCamera_Manager::D3DTS_PROJ))))
		return E_FAIL;

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i)))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		if (FAILED(m_pModelCom->Bind_Buffers(static_cast<_uint>(i))))
			return E_FAIL;

		m_pModelCom->Render((_uint)i);
	}
	///*if (FAILED(m_pShaderCom->Bind_Float("g_fRandR", m_pGameInstance->Random_Float(0.f, 0.5f))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_Float("g_fRandG", m_pGameInstance->Random_Float(0.f, 0.5f))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_Float("g_fRandB", m_pGameInstance->Random_Float(0.f, 0.5f))))*/
		//return E_FAIL;

	return S_OK;
}

void CPlayerArm::Soft_Animation_Change(_uint _iAnimationNum, _bool _bLoop, _float _bSpeedMultiply)
{
	if (m_pModelCom->Get_CurrentAnimIndex() == _iAnimationNum)
		return;

	if (m_pModelCom->Get_Finished() == true) {
		m_pModelCom->Set_AnimIndex(_iAnimationNum, _bLoop);
		m_pModelCom->Set_AnimSpeed(_bSpeedMultiply);
		m_pModelCom->Set_Finished(false);
	}
}

void CPlayerArm::Hard_Animation_Change(_uint _iAnimationNum, _bool _bLoop, _float _bSpeedMultiply)
{
	if(m_pModelCom->Get_CurrentAnimIndex() == _iAnimationNum)
		return;

	if(m_pModelCom->Get_Finished() != true && (m_pModelCom->Get_CurrentAnimIndex() == 5 || m_pModelCom->Get_CurrentAnimIndex() == 4))
		return;

	m_pModelCom->Set_AnimIndex(_iAnimationNum, _bLoop);
	m_pModelCom->Set_AnimSpeed(_bSpeedMultiply);
	m_pModelCom->Set_Finished(false);
}

HRESULT CPlayerArm::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_GhostRunner_Arm"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC	BoundingDesc{};

	BoundingDesc.fRadius = 100.0f;
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.fRadius, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	m_pColliderCom->Set_Owner(this);
	GAMEINSTANCE->Add_Collider(CCollision_Manager::COLLISION_PLAYER, m_pColliderCom);

	Set_MainTag("Player");
	Set_SubTag("Arm");
	///* For PhysX Collider*/
	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
	//	TEXT("Com_PhysX"), reinterpret_cast<CComponent**>(&m_pPhysXCollider))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CPlayerArm::Bind_ShaderResources()
{
	return S_OK;
}

CPlayerArm* CPlayerArm::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayerArm* pInstance = new CPlayerArm(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayerArm"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CPlayerArm::Clone(void* pArg)
{
	CPlayerArm* pInstance = new CPlayerArm(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPlayerArm"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerArm::Free()
{
	__super::Free();

	for (auto& pParts : m_vecParts)
		Safe_Release(pParts);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pPhysXCollider);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pWire);

}
