#pragma once
#include "GameInstance.h"

class CharacterControllerHitReport : public PxUserControllerHitReport {
public:
    PxVec3 lookDirection = { 0.f,0.f,0.f }; // 캐릭터의 바라보는 방향
    bool rightHit = false;
    bool leftHit = false;
    bool sideHit = false;

    CharacterControllerHitReport() {}

    void SetLookDirection(const PxVec3& direction) {
		lookDirection = direction;
	}

    void onShapeHit(const PxControllerShapeHit& hit) override;

    bool GetRightHit() const {
		return rightHit;
	}

    bool GetLeftHit() const {
        return leftHit;
    }

    bool GetSideHit() const {
		return sideHit;
    }

    void onControllerHit(const PxControllersHit& hit) override {}
    void onObstacleHit(const PxControllerObstacleHit& hit) override {}
};