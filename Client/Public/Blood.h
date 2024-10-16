#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
END

BEGIN(Client)

class CBlood final : public CGameObject
{
private:
	CBlood(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBlood(const CBlood& rhs);
	virtual ~CBlood() = default;

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
	
	_float m_fFrame = 0.f;
	_float m_fFrameMax = 0.02f;

	_uint m_iTextureIndex = 0;
	_uint m_iTextureMax = 35;
private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

public:
	static CBlood* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END