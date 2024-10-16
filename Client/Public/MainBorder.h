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

class CMainBorder final : public CUi
{
private:
	CMainBorder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMainBorder(const CMainBorder& rhs);
	virtual ~CMainBorder() = default;

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

public:
	static CUi* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUi* Clone(void* pArg) override;
	virtual void Free() override;
};

END