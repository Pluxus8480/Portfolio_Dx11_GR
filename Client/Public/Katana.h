#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CKatana final : public Engine::CPartObject
{
public:
	typedef struct KatanaDesc: public CPartObject::PARTOBJ_DESC
	{
		const _float4x4* pBoneMatrix;
		const _float4x4* pWorldMatrix;
		_uint* pState;
	}WEAPON_DESC;

private:
	CKatana(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKatana(const CKatana& rhs);
	virtual ~CKatana() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Set_On(_bool pState) { m_bOn = pState; }
	_bool Get_On() { return m_bOn; }

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

	_bool m_bOn = false;
	class CTrail* m_pTrailCom = { nullptr };

private:
	const _float4x4* m_pBoneMatrix = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CKatana* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};


END