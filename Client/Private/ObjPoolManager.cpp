#pragma once
#include "ObjPoolManager.h"
#include "Bullet.h"
#include "Beam.h"
#include "ShockWave.h"

IMPLEMENT_SINGLETON(CObjPoolManager)

CObjPoolManager::CObjPoolManager()
{

}

void CObjPoolManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pGameInstance = CGameInstance::Get_Instance();
    m_pDevice = pDevice;
    m_pContext = pContext;
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

void CObjPoolManager::PriorityTick(_float fTimeDelta)
{

}

void CObjPoolManager::Tick(_float fTimeDelta)
{

}

void CObjPoolManager::LateTick(_float fTimeDelta)
{

}

void CObjPoolManager::Render()
{

}

void CObjPoolManager::BulletPooling()
{
    CTransform::TRANSFORM_DESC			TransformDesc{};
    TransformDesc.vPosition = _float4(9999.f, 9999.f, 9999.f,1.f);
    TransformDesc.fSpeedPerSec = 8.f;
    TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

    for(_uint i = 0; i < iBulletCnt; ++i)
	{
        m_BulletList.emplace_back(dynamic_cast<CBullet*>(m_pGameInstance->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Bullet"), &TransformDesc)));
	}

}

CBullet* CObjPoolManager::BulletAwake(void* pArg)
{
    for (auto& Bullet : m_BulletList)
	{
		if (Bullet->Get_ContainState() == CContainedObject::CONTAIN_SLEEP)
		{
			Bullet->Activate(pArg);
			return Bullet;
		}
	}

    return nullptr;

}

void CObjPoolManager::BeamPooling()
{
    CTransform::TRANSFORM_DESC			TransformDesc{};
    TransformDesc.vPosition = _float4(9999.f, 9999.f, 9999.f, 1.f);
    TransformDesc.fSpeedPerSec = 8.f;
    TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

    for (_uint i = 0; i < iBulletCnt; ++i)
    {
        m_BeamList.emplace_back(dynamic_cast<CBeam*>(m_pGameInstance->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Beam"), &TransformDesc)));
    }

}

CBeam* CObjPoolManager::BeamAwake(void* pArg)
{
    for (auto& Beam : m_BeamList)
    {
        if (Beam->Get_ContainState() == CContainedObject::CONTAIN_SLEEP)
        {
            Beam->Activate(pArg);
            return Beam;
        }
    }

    return nullptr;
}

void CObjPoolManager::ShockWavePooling()
{
    CTransform::TRANSFORM_DESC			TransformDesc{};
    TransformDesc.vPosition = _float4(9999.f, 9999.f, 9999.f, 1.f);
    TransformDesc.fSpeedPerSec = 8.f;
    TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

    for (_uint i = 0; i < iBulletCnt; ++i)
    {
        m_ShockWaveList.emplace_back(dynamic_cast<CShockWave*>(m_pGameInstance->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_ShockWave"), &TransformDesc)));
    }
}

CShockWave* CObjPoolManager::ShockWaveAwake(void* pArg)
{
    for (auto& ShockWave : m_ShockWaveList)
    {
        if (ShockWave->Get_ContainState() == CContainedObject::CONTAIN_SLEEP)
        {
			ShockWave->Activate(pArg);
			return ShockWave;
		}
	}

    return nullptr;
}


void CObjPoolManager::Free()
{
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
}
