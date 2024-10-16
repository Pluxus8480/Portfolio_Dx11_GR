#pragma once
#include "Base.h"
#include "Shader.h"

BEGIN(Engine)

enum class eUiRenderType
{
	Render_NonBlend,
	Render_Blend,
	End
};
class CUi_Manager final : public CBase
{

private:
	CUi_Manager(ENGINE_DESC sEngine_Desc,ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUi_Manager() = default;


public:
	HRESULT Add_Ui_LifePrototype(const wstring& Ui_LifePrototypeTag, class CUi* Ui_LifePrototype);
	HRESULT Add_Ui_LifeClone(const wstring& Ui_LifePrototypeTag, eUiRenderType UiRenderType, void* pArg);

	HRESULT Add_Ui_Active(const wstring& Ui_ActiveTag, eUiRenderType UiRenderType, class CUi* Ui_Active);

	CUi* Get_ActiveUi_TagSearch(const wstring& Ui_ActiveTag);
public:
	void PriorityTick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);

	void Ui_Render_Begin();
	HRESULT Ui_Render();
	void Ui_Render_End();

	void Set_WinSize(_uint iWinSizeX, _uint iWinSizeY);

	//for UiActive
	void Set_Ui_ActiveState(const wstring& Ui_ActiveTag, bool _isActive);
	bool Get_Ui_ActiveState(const wstring& Ui_ActiveTag);

	void Set_Ui_ActiveTextureIndex(const wstring& Ui_ActiveTag, int _iTextureIndex);
	int  Get_Ui_ActiveTextureIndex(const wstring& Ui_ActiveTag);

	CUi* Get_ActiveBlendUI(const wstring& strKey);
	CUi* Get_ActiveNonBlendUI(const wstring& strKey);

	_float4x4 Get_UiViewMatrix() {
		return m_ViewMatrix;
	}

	_float4x4 Get_UiProjMatrix() {
		return m_ProjMatrix;
	}
private:
	map<const wstring, class CUi*> m_Ui_LifePrototypes;

	list<class CUi*> m_Ui_Lifelist;
	list<class CUi*> m_Ui_LifeBlendlist;

	map<const wstring, class CUi*> m_Ui_Active;
	map<const wstring, class CUi*> m_Ui_ActiveBlend;

	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CShader* m_pShaderCom = { nullptr };

private:


	_float4x4 m_ViewMatrix;
	_float4x4 m_ProjMatrix;
	_uint m_iWinSizeX = { 0 };
	_uint m_iWinSizeY = { 0 };


public:
	static CUi_Manager* Create(ENGINE_DESC sEngine_Desc, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
