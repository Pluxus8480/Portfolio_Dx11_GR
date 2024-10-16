#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
END

BEGIN(Client)

class CGraplingPointObject final : public CGameObject
{
private:
	CGraplingPointObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGraplingPointObject(const CGraplingPointObject& rhs);
	virtual ~CGraplingPointObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Collision_RayHit() override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	_float m_fRotating = 0.f;

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

public:
	static CGraplingPointObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END