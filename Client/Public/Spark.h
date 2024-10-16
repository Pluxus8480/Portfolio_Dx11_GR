#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Point_Instance;
END


BEGIN(Client)

class CSpark final : public CGameObject
{
private:
	CSpark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpark(const CSpark& rhs);
	virtual ~CSpark() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	void Set_Pivot(_float3 vPivot) { m_pVIBufferCom->Set_Pivot(vPivot); }
	_float3 Get_Pivot() { return m_pVIBufferCom->Get_Pivot(); }

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };

	_float						m_fFrame = {};
	_float						m_fLifeTime = 1.5f;	
private:
	HRESULT Ready_Components();

public:
	static CSpark* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END