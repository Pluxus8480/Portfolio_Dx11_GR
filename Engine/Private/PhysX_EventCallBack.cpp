#include "PhysX_Manager.h"
#include "PhysX_EventCallBack.h"

CPhysX_EventCallBack::CPhysX_EventCallBack()
{

}

void CPhysX_EventCallBack::onConstraintBreak(PxConstraintInfo* constraints, PxU32 count)
{
	//cout << "onConstraintBreak" << endl;
}

void CPhysX_EventCallBack::onWake(PxActor** actors, PxU32 count)
{
	//cout << "onWake" << endl;
}

void CPhysX_EventCallBack::onSleep(PxActor** actors, PxU32 count)
{
	//cout << "onSleep" << endl;
}

void CPhysX_EventCallBack::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{

	for (PxU32 i = 0; i < nbPairs; ++i) {
		const PxContactPair& cp = pairs[i];
		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND) {
			MSG_BOX(L"onContact");
		}
	}
}


// 트리거를 쓰는 얘들은 OnTrigger
void CPhysX_EventCallBack::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	MSG_BOX(L"onContact");
	//while (count--)
	//{
	//	const PxTriggerPair& current = *pairs++;
	//	if (current.status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
	//	{
	//		weak_ptr<CPhysXCollider> pColliderCom = GET_SINGLE(CPhysX_Manager)->Find_PhysXCollider(*(_uint*)current.triggerActor->userData);
	//		weak_ptr<CPhysXCollider> pOtherColliderCom = GET_SINGLE(CPhysX_Manager)->Find_PhysXCollider(*(_uint*)current.otherActor->userData);

	//		if (pColliderCom.lock()->Get_ColliderDesc().bTrigger &&
	//			pOtherColliderCom.lock()->Get_ColliderDesc().bTrigger)
	//		{
	//			pColliderCom.lock()->PhysXCollisionEnter(pOtherColliderCom);
	//			pOtherColliderCom.lock()->PhysXCollisionEnter(pColliderCom);
	//		}

	//		//CPhysXCollider* pColliderCom = (CPhysXCollider*)current.triggerActor->userData;
	//		//CPhysXCollider* pOtherColliderCom = (CPhysXCollider*)current.otherActor->userData;

	//		//weak_ptr<CPhysXCollider> pColliderCom = *reinterpret_cast<std::weak_ptr<CPhysXCollider>*>(&current.triggerActor->userData);
	//		//weak_ptr<CPhysXCollider> pOtherColliderCom = *reinterpret_cast<std::weak_ptr<CPhysXCollider>*>(&current.otherActor->userData);


	//	}
	//	if (current.status & PxPairFlag::eNOTIFY_TOUCH_LOST)
	//	{
	//		weak_ptr<CPhysXCollider> pColliderCom = GET_SINGLE(CPhysX_Manager)->Find_PhysXCollider(*(_uint*)current.triggerActor->userData);
	//		weak_ptr<CPhysXCollider> pOtherColliderCom = GET_SINGLE(CPhysX_Manager)->Find_PhysXCollider(*(_uint*)current.otherActor->userData);

	//		if (pColliderCom.lock()->Get_ColliderDesc().bTrigger &&
	//			pOtherColliderCom.lock()->Get_ColliderDesc().bTrigger)
	//		{
	//			pColliderCom.lock()->PhysXCollisionExit(pOtherColliderCom);
	//			pOtherColliderCom.lock()->PhysXCollisionExit(pColliderCom);
	//		}
	//	}
	//	cout << "Shape is leaving trigger volume\n";
	//}
}

PxSimulationEventCallback* CPhysX_EventCallBack::Create()
{
	PxSimulationEventCallback* pInstance = new CPhysX_EventCallBack();

	return pInstance;
}

void CPhysX_EventCallBack::Release()
{
	delete this;
}
