#pragma once
#include "Client_Defines.h"
#include "GameInstance.h"
#include "Base.h"

BEGIN(Client)

class CGame_Manager : public CBase
{
	DECLARE_SINGLETON(CGame_Manager)

private:
	CGame_Manager();
	virtual ~CGame_Manager() = default;


public:
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void PriorityTick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);
	void Render();

	void VideoSetting();
	void VideoRender();

private:
	CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	//IMFSourceReader*	pReader = NULL;
	//IMFMediaType*		pVideoType = NULL;
	//IMFSample*			pSample = NULL;
public:
	virtual void Free() override;
};

END