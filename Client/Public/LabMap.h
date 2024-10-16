#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "PhysXCollider.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CLabMap final : public CGameObject
{
private:
	CLabMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLabMap(const CLabMap& rhs);
	virtual ~CLabMap() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CPhysXCollider* m_pPhysXCollider = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	HRESULT Model_Rendering(_int iMeshNum);

	shared_mutex m_RenderMtx;
public:
	static CLabMap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END