#pragma once

#include "Base.h"

BEGIN(Engine)

class CResource_Container : public CBase
{

private:
	CResource_Container(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CResource_Container() = default;


public:
	void Initialize();
	void Tick(_float fTimeDelta);
	void Clear();
	void Render();

	CTexture* SearchMaterial(const _wstring& pMaterialTag);

private:
	map<const _wstring,class CTexture*> m_MaterialList;

public:
	static CResource_Container* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;
};

END