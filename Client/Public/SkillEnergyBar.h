#pragma once

#include "Client_Defines.h"
#include "Ui.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CSkillEnergyBar final : public CUi
{
private:
	CSkillEnergyBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkillEnergyBar(const CSkillEnergyBar& rhs);
	virtual ~CSkillEnergyBar() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void Initialize_Set_Scale_Pos_Rotation(void* pArg);

private:
	HRESULT Ready_Components();
	ID3D11BlendState* pBlendState = nullptr;

	_float Gauge = 0.f;
	_float MaxGauge = 83.f;

public:
	static CUi* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUi* Clone(void* pArg) override;
	virtual void Free() override;
};

END