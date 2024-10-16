#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CPistol final : public Engine::CPartObject
{
public:
	typedef struct pistolDesc: public CPartObject::PARTOBJ_DESC
	{
		const _float4x4* pBoneMatrix;
		_uint* pState;
	}WEAPON_DESC;

private:
	CPistol(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPistol(const CPistol& rhs);
	virtual ~CPistol() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	const _float4x4* m_pBoneMatrix = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CPistol* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};


END