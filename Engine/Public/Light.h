#pragma once

#include "GameObject.h"

BEGIN(Engine)

class CLight final : public CGameObject
{
private:
	CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLight() = default;

public:
	const LIGHT_DESC* Get_LightDesc() const {
		return &m_LightDesc;
	}

public:
	//HRESULT Initialize(const LIGHT_DESC& LightDesc);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	HRESULT Lighting(CShader* pShader, CVIBuffer_Rect* pVIBuffer);

public:
	void Set_LightState(_bool bIsOn) { m_bIsOn = bIsOn; }
	_bool Get_LightState() { return m_bIsOn; }

	void Set_LightDiffuse(_float4 vDiffuse) { m_LightDesc.vDiffuse = vDiffuse; }
	_float4 Get_LightDiffuse() { return m_LightDesc.vDiffuse; }
	_float* Get_LightDiffusePtr() { return &m_LightDesc.vDiffuse.x; }

	void Set_LightSpecular(_float4 vSpecular) { m_LightDesc.vSpecular = vSpecular; }
	_float4 Get_LightSpecular() { return m_LightDesc.vSpecular; }
	_float* Get_LightSpecularPtr() { return &m_LightDesc.vSpecular.x; }

	void Set_LightAmbient(_float4 vAmbient) { m_LightDesc.vAmbient = vAmbient; }
	_float4 Get_LightAmbient() { return m_LightDesc.vAmbient; }
	_float* Get_LightAmbientPtr() { return &m_LightDesc.vAmbient.x; }

	void Set_LightRange(_float fRange) { m_LightDesc.fRange = fRange; }
	_float Get_LightRange() { return m_LightDesc.fRange; }
	_float* Get_LightRangePtr() { return &m_LightDesc.fRange; }

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	_bool m_bIsOn = true;
	//_bool m_bIsBlink = false;

private:
	LIGHT_DESC			m_LightDesc = {};

	class CShader*		m_pShaderCom = { nullptr };
	class CModel*		m_pModelCom = { nullptr };

	_float m_fTimeAcc = 0.f;
	_uint  m_iPattern = 0;
public:
	static CLight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* LightDesc);
	virtual CGameObject* Clone(void* pArg) override { return nullptr;  };
	virtual void Free() override;
};

END