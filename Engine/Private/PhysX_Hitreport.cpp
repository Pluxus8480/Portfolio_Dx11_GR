#pragma once
#include "PhysX_Hitreport.h"

void CharacterControllerHitReport::onShapeHit(const PxControllerShapeHit& hit)
{
        // 충돌한 Actor 가져오기
        PxRigidActor* actor = hit.shape->getActor();

            // 충돌 방향 벡터
            PxVec3 hitDirection = hit.dir;

            // Look 방향을 기준으로 좌측/우측 충돌 판별
            float crossProductZ = lookDirection.cross(hitDirection).z;

            rightHit = false;
            leftHit = false;
            sideHit = false;

            if (crossProductZ > 0) {
                leftHit = true;
                sideHit = true;
            }
            else if (crossProductZ < 0) {
                rightHit = true;
                sideHit = true;
            }
 
}
