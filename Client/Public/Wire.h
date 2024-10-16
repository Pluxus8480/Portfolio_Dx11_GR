#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CVIBuffer_TrailRect;
END

BEGIN(Client)

class CWire final : public CGameObject
{
public:
	typedef struct WireDesc	{
		const _float4x4* pBoneMatrix;
		const _float4x4* pTarget;
	}WIRE_DESC;

private:
	CWire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWire(const CWire& rhs);
	virtual ~CWire() = default;

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

	void Set_BoneMatrix(const _float4x4* pBoneMatrix) { m_pBoneMatrix2 = (_float4x4*)pBoneMatrix; }
	void Clear_BoneMatrix() { m_pBoneMatrix2 = nullptr; }

private:
	_bool m_bOn = false;
	_bool m_bStart = false;
private:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_TrailRect* m_pBufferCom = { nullptr };

private:
	_float4x4* m_pBoneMatrix = { nullptr };
	_float4x4* m_pBoneMatrix2 = { nullptr };
	_float4x4* m_pTarget = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CWire* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};


END