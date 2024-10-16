#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CVIBuffer_Trail;
END

BEGIN(Client)

class CTrail final : public Engine::CPartObject
{
public:
	typedef struct TrailObj_Desc : public CPartObject::PARTOBJ_DESC
	{
		const _float4x4* pBoneMatrix;
		const _float4x4* pBoneMatrix2;
		_float4x4* pTarget;
	}TRAILOBJ_DESC;

private:
	CTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrail(const CTrail& rhs);
	virtual ~CTrail() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_TrailOn(_bool bOn) { m_bOn = bOn; }
	_bool Get_TrailOn() { return m_bOn; }

private:
	_bool m_bOn = false;

private:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Trail* m_pBufferCom = { nullptr };

private:
	_float4x4* m_pBoneMatrix = { nullptr };
	_float4x4* m_pBoneMatrix2 = { nullptr };
	_float4x4* m_pTarget = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};


END