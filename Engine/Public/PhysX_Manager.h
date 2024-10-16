#pragma once

#include "Base.h"

BEGIN(Engine)

class CPhysX_Manager : public CBase
{

private:
	CPhysX_Manager();
	virtual ~CPhysX_Manager() = default;

public:
	void Initialize();
	void Tick(_float fTimeDelta);
	void Clear();
	void Start();
	void Render();

private:
	HRESULT Initialize_PhysX();

	HRESULT Create_Foundation();
	HRESULT Create_Physics();
	HRESULT Create_Dispatcher();
	HRESULT Create_Cooking();

	HRESULT Create_Scene();


public:
	PxFoundation* Get_Foundation() {		
		return m_pFoundation;
	}

	PxPhysics* Get_Physics() {
		return m_pPhysics;
	}

	PxDefaultCpuDispatcher* Get_Dispatcher() {
		return m_pDispatcher;
	}

	PxScene* Get_Scene() {
		return m_pScene;
	}

	PxControllerManager* Get_ControllerManager() {
		return m_pControllerManager;
	}

	void Set_SimulationEventCallback(PxSimulationEventCallback* pCallback) {
		gDefaultSimulationEventCallback = pCallback;
	}

	PxSimulationEventCallback* Get_SimulationEventCallback() {
		return gDefaultSimulationEventCallback;
	}

public:
	void Simulate_CurrentScene(_float fTimeDelta);

private:
	PxFoundation* m_pFoundation = nullptr;
	PxPhysics* m_pPhysics = nullptr;
	PxDefaultCpuDispatcher* m_pDispatcher = nullptr;
	//PxCooking* m_pCooking = nullptr;

	PxScene* m_pScene = nullptr;
	//PxMaterial* gMaterial = nullptr;
	//PxPvd* gPvd = nullptr;

	PxDefaultAllocator gDefaultAllocator;
	PxDefaultErrorCallback gDefaultErrorCallback;
	PxSimulationEventCallback* gDefaultSimulationEventCallback;
	PxControllerManager* m_pControllerManager;
public:
	static CPhysX_Manager* Create();
	virtual void Free() override;
};

END