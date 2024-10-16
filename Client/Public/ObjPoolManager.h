#pragma once
#include "Client_Defines.h"
#include "GameInstance.h"
#include "Base.h"

BEGIN(Client)

class CObjPoolManager : public CBase
{
	DECLARE_SINGLETON(CObjPoolManager)

private:
	CObjPoolManager();
	virtual ~CObjPoolManager() = default;


public:
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void PriorityTick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);
	void Render();

	void BulletPooling();
	class CBullet* BulletAwake(void* pArg);

	void BeamPooling();
	class CBeam* BeamAwake(void* pArg);

	void ShockWavePooling();
	class CShockWave* ShockWaveAwake(void* pArg);

private:
	CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
	list<CBullet*> m_BulletList;
	_uint iBulletCnt = 30;

	list<CBeam*> m_BeamList;
	_uint iBeamCnt = 30;

	list<CShockWave*> m_ShockWaveList;
	_uint iShockWaveCnt = 30;

public:
	virtual void Free() override;
};

END