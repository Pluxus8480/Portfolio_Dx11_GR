#pragma once

#include "Base.h"

BEGIN(Engine)

class CPhysX_EventCallBack : public PxSimulationEventCallback
{

public:
	CPhysX_EventCallBack();
	virtual ~CPhysX_EventCallBack() = default;

    virtual void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override;
    virtual void onWake(PxActor** actors, PxU32 count) override;
    virtual void onSleep(PxActor** actors, PxU32 count) override;
    virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override;
    virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) override;
    virtual void onAdvance(const PxGpuMirroredPointer<PxGpuParticleSystem>& gpuParticleSystem, CUstream stream) override {};

	static PxSimulationEventCallback* Create();
	void Release();
};

END