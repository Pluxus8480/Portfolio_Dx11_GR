#include "PhysX_Manager.h"
#include "GameInstance.h"

CPhysX_Manager::CPhysX_Manager()
{

}

void CPhysX_Manager::Initialize()
{
	if(FAILED(Initialize_PhysX()))
		assert(false);

	//Test Code --Warning
	if(FAILED(Create_Scene()))
		assert(false);

}

void CPhysX_Manager::Tick(_float fTimeDelta)
{

}

void CPhysX_Manager::Clear()
{

}

void CPhysX_Manager::Start()
{

}

void CPhysX_Manager::Render()
{

}

HRESULT CPhysX_Manager::Initialize_PhysX()
{
	if(FAILED(Create_Foundation()))
		return E_FAIL;

	if(FAILED(Create_Physics()))
		return E_FAIL;

	if(FAILED(Create_Dispatcher()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPhysX_Manager::Create_Foundation()
{
	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocator, gDefaultErrorCallback);

	if (m_pFoundation == nullptr)
		return E_FAIL;

	return S_OK;
}

HRESULT CPhysX_Manager::Create_Physics()
{
	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(), true);

	if(m_pPhysics == nullptr)
		return E_FAIL;

	return S_OK;
}

HRESULT CPhysX_Manager::Create_Dispatcher()
{
	m_pDispatcher = PxDefaultCpuDispatcherCreate(2);

	if (m_pDispatcher == nullptr)
		return E_FAIL;

	return S_OK;
}

HRESULT CPhysX_Manager::Create_Cooking()
{
	//m_pCooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_pFoundation, PxCookingParams(PxTolerancesScale()));

	return S_OK;
}

HRESULT CPhysX_Manager::Create_Scene()
{
	PxSceneDesc sceneDesc(m_pPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.cpuDispatcher = m_pDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	sceneDesc.simulationEventCallback = CGameInstance::Get_Instance()->Get_PhysX_Callback();

	m_pScene = m_pPhysics->createScene(sceneDesc);
	if(m_pScene == nullptr)
		return E_FAIL;

	m_pControllerManager = PxCreateControllerManager(*m_pScene);

	return S_OK;
}

void CPhysX_Manager::Simulate_CurrentScene(_float fTimeDelta)
{
	m_pScene->simulate(fTimeDelta);
	m_pScene->fetchResults(true);
}

CPhysX_Manager* CPhysX_Manager::Create()
{
	CPhysX_Manager* pInstance = new CPhysX_Manager();

	pInstance->Initialize();

	return pInstance;
}

void CPhysX_Manager::Free()
{
	__super::Free();

	Safe_PhysX_Release(m_pControllerManager);
	Safe_PhysX_Release(m_pScene);
	Safe_PhysX_Release(m_pPhysics);
	Safe_PhysX_Release(m_pDispatcher);
	Safe_PhysX_Release(m_pFoundation);

}
