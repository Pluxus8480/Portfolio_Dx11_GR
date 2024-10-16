#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "Light.h"

BEGIN(Client)

class CLevel_Lab final : public CLevel
{
private:
	CLevel_Lab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Lab() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:

	HRESULT Ready_Layer_Camera(const wstring& strLayerTag);
	HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);
	HRESULT Ready_Layer_Map(const wstring& strLayerTag);
	HRESULT Ready_Layer_Monster(const wstring& strLayerTag);
	HRESULT Ready_Layer_Effect(const wstring& strLayerTag);
	HRESULT Ready_Light();

	HRESULT Ready_LandObjects();

	void Level_Chanege_Sequenece(_float fTimeDelta);
	/* 실제 게임플레이 화면의 배경을 구성하는데 있어 필요한 객체들을 모아놓은 레이어를 생성한다. */
private:

	_uint m_bIsPhase = 0;
	_float m_fPhaseTime = 3.f;
	CLight* m_pLight = nullptr;

	_bool m_bReflect = true;
	_float m_fDiffuse = 0.8f;

public:
	static CLevel_Lab* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
