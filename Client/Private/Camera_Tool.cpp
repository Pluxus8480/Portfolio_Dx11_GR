#include "Camera_Tool.h"
#include "GameInstance.h"
#include "PlayerManager.h"

CCamera_Tool::CCamera_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCoreCamera{ pDevice, pContext }
{
}

CCamera_Tool::CCamera_Tool(const CCamera_Tool& rhs)
	: CCoreCamera{ rhs }
{
}

HRESULT CCamera_Tool::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Tool::Initialize(void* pArg)
{
	CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(pArg);
	m_fMouseSensor = pDesc->fMouseSensor;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Set_MainTag("Player");

	/* For PhysX Collider*/
	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
	//	TEXT("Com_PhysX"), reinterpret_cast<CComponent**>(&m_pPhysXCollider))))
	//	return E_FAIL;

	///* For RigidBody*/
	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
	//	TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidbody), m_pTransformCom)))
	//	return E_FAIL;

	//_float3 m_vPosition = {};
	//XMStoreFloat3(&m_vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	//m_pPhysXCollider->createCharacterController(CGameInstance::Get_Instance()->Get_PhysX_ControllerManager(), PxVec3(m_vPosition.x, m_vPosition.y, m_vPosition.z));

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

void CCamera_Tool::PriorityTick(_float fTimeDelta)
{

}

void CCamera_Tool::Tick(_float fTimeDelta)
{
	//m_pRigidbody->Update(fTimeDelta);
	Key_Input(fTimeDelta);
	if(m_bCameraRoll== true)
		Camera_Roll(fTimeDelta);
	/*m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());*/
}

void CCamera_Tool::LateTick(_float fTimeDelta)
{

	Fall_Check();
	Camera_Effect(fTimeDelta);

}

HRESULT CCamera_Tool::Render()
{

	return S_OK;
}

void CCamera_Tool::Camera_Effect(_float fTimeDelta)
{
	//Camera_HeadTilt(fTimeDelta);
	Camera_Shake(fTimeDelta);

}

void CCamera_Tool::Camera_Roll(_float fTimeDelta)
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
					m_fVerticalAngle += iMouseMove * m_fMouseSensor;
					m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * iMouseMove * m_fMouseSensor);
			}
		}
		ClientToScreen(g_hWnd, &ptWindow);

		SetCursorPos(ptWindow.x, ptWindow.y);
	}
}

void CCamera_Tool::Camera_HeadTilt(_float fTimeDelta)
{

}

void CCamera_Tool::Camera_HeadTilt_Reset(_float fTimeDelta)
{

}

void CCamera_Tool::Camera_Shake(_float fTimeDelta)
{

}

void CCamera_Tool::Key_Input(_float fTimeDelta)
{
	if (m_pGameInstance->GetKey(eKeyCode::A))
	{
		m_pTransformCom->Go_Left(fTimeDelta);
	}
	if (m_pGameInstance->GetKey(eKeyCode::D))
	{
		m_pTransformCom->Go_Right(fTimeDelta);
	}
	if (m_pGameInstance->GetKey(eKeyCode::W))
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
	if (m_pGameInstance->GetKey(eKeyCode::S))
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}

	if (m_pGameInstance->GetKeyDown(eKeyCode::RButton)) 
	{
		m_bCameraRoll = true;
		POINT		ptWindow = { g_iWinSizeX >> 1, g_iWinSizeY >> 1 };
		ClientToScreen(g_hWnd, &ptWindow);
		SetCursorPos(ptWindow.x, ptWindow.y);
	}
	
	if (m_pGameInstance->GetKeyUp(eKeyCode::RButton))
	{
		m_bCameraRoll = false;
	}

}

void CCamera_Tool::Fall_Check()
{
}

CCamera_Tool* CCamera_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Tool* pInstance = new CCamera_Tool(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCamera_Tool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CCamera_Tool::Clone(void* pArg)
{
	CCamera_Tool* pInstance = new CCamera_Tool(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCamera_Tool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CCamera_Tool::Free()
{
	__super::Free();
	Safe_Release(m_pPhysXCollider);
	Safe_Release(m_pRigidbody);

}
