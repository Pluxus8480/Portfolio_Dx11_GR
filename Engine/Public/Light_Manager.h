#pragma once
#
#include "Base.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
private:
	CLight_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLight_Manager() = default;

public:
	const LIGHT_DESC* Get_LightDesc(_uint iIndex) const;
	
public:
	HRESULT Initialize();
	HRESULT TickEngine(_float fTimeDelta);
	HRESULT Add_Light(void* LightDesc);
	class CLight* Add_Light_Return(void* LightDesc);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

	void LightControl(string strLightTag, _bool bOn);
	void LightControl(string strLightTag);

	void LightClear();

	void LightDeleteCheck();
private:
	list<CLight*>			m_Lights;

	_uint m_iLightCount = 0;

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;

public:
	static CLight_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

};

END