#pragma once
#include "PhysX_Hitreport.h"

void CharacterControllerHitReport::onShapeHit(const PxControllerShapeHit& hit)
{
        // �浹�� Actor ��������
        PxRigidActor* actor = hit.shape->getActor();

            // �浹 ���� ����
            PxVec3 hitDirection = hit.dir;

            // Look ������ �������� ����/���� �浹 �Ǻ�
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
