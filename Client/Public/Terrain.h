//#pragma once
//
//#include "Client_Defines.h"
//#include "GameObject.h"
//
//BEGIN(Engine)
//class CShader;
//class CTexture;
//class CVIBuffer_Terrain;
//END
//
//
//BEGIN(Client)
//
//class CTerrain final : public CGameObject
//{
//private:
//	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
//	CTerrain(const CTerrain& rhs);
//	virtual ~CTerrain() = default;
//
//public:
//	virtual HRESULT Initialize_Prototype();
//	virtual HRESULT Initialize(void* pArg);
//	virtual void PriorityTick(_float fTimeDelta);
//	virtual void Tick(_float fTimeDelta);
//	virtual void LateTick(_float fTimeDelta);
//	virtual HRESULT Render();
//
//private:
//	CShader* m_pShaderCom = { nullptr };
//	CTexture* m_pTextureCom = { nullptr };
//	CVIBuffer_Terrain* m_pVIBufferCom = { nullptr };
//
//private:
//	HRESULT Ready_Components();
//
//public:
//	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
//	virtual CGameObject* Clone(void* pArg) override;
//	virtual void Free() override;
//};
//
//END