#include "../Public/MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "GameObject.h"
#include "PhysXCollider.h"
#include "VIBuffer_Trail.h"
#include "RigidBody.h"

#include "PlayerManager.h"
#include "GameManager.h"
#include "ObjPoolManager.h"
#include "EnemyManager.h"

CMainApp::CMainApp()
	: m_pGameInstance {CGameInstance::Get_Instance()}
{
	Safe_AddRef(m_pGameInstance);

	/*D3D11_SAMPLER_DESC		SamplerDesc; */

}

HRESULT CMainApp::Initialize()
{
	ENGINE_DESC			EngineDesc{};

	EngineDesc.hWnd = g_hWnd;
	EngineDesc.isWindowed = true;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;

	if (FAILED(m_pGameInstance->Initialize_Engine(LEVEL_END, EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Create_Imgui(g_hWnd, m_pDevice, m_pContext)))
		return E_FAIL;

	m_pPlayerManager = CPlayer_Manager::Get_Instance();
	m_pPlayerManager->Initialize(m_pDevice, m_pContext);

	m_pGameManager = CGame_Manager::Get_Instance();
	m_pGameManager->Initialize(m_pDevice, m_pContext);

	m_pObjPoolManager = CObjPoolManager::Get_Instance();
	m_pObjPoolManager->Initialize(m_pDevice, m_pContext);

	m_pEnemyManager = CEnemyManager::Get_Instance();
	m_pEnemyManager->Initialize(m_pDevice, m_pContext);

	if (FAILED(Ready_Prototoype_Component_ForStatic()))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;	

	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	m_pPlayerManager->Tick(fTimeDelta);
	m_pGameManager->Tick(fTimeDelta);
	m_pGameInstance->Tick_Engine(fTimeDelta);

	m_pObjPoolManager->PriorityTick(fTimeDelta);
	m_pObjPoolManager->Tick(fTimeDelta);
	m_pObjPoolManager->LateTick(fTimeDelta);

	m_pEnemyManager->PriorityTick(fTimeDelta);
	m_pEnemyManager->Tick(fTimeDelta);
	m_pEnemyManager->LateTick(fTimeDelta);

}

HRESULT CMainApp::Render()
{
	m_pGameInstance->Clear_BackBuffer_View(_float4(0.0f, 0.f, 0.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	m_pGameInstance->Draw();

	m_pGameInstance->Imgui_Render();

	m_pGameInstance->Present();



	return S_OK;
}



HRESULT CMainApp::Open_Level(LEVEL eLevelID)
{
	if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototoype_Component_ForStatic()
{
	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_PhysX_Collider*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		CPhysXCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_RigidBody*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		CRigidbody::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp*		pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CMainApp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	__super::Free();

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	m_pPlayerManager->Free();
	m_pPlayerManager->Destroy_Instance();

	m_pGameManager->Free();
	m_pGameManager->Destroy_Instance();

	m_pObjPoolManager->Free();
	m_pObjPoolManager->Destroy_Instance();

	m_pEnemyManager->Free();
	m_pEnemyManager->Destroy_Instance();

	Safe_Release(m_pGameInstance);

	CGameInstance::Release_Engine();
}

