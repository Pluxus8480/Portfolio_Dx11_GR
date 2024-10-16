#pragma once
#include "Base.h"
#include "GameObject.h"


BEGIN(Engine)

class ENGINE_DLL CUi abstract : public CGameObject
{
public:
	typedef struct UiPositionSize
	{
		_float m_fX = { 0 };
		_float m_fY = { 0 };
		_float m_fSizeX = { 0 };
		_float m_fSizeY = { 0 };
		_float3 m_Rotation = { 0, 0, 0 };
	}Ui_Pos_Size_Rotation;


protected:
	CUi(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUi(const CUi& rhs);
	virtual ~CUi() = default;


//Pure Public Virtual Function
public:
	virtual HRESULT Initialize_Prototype() PURE;
	virtual HRESULT Initialize(void* pArg) PURE;
	virtual void PriorityTick(_float fTimeDelta) PURE;
	virtual void Tick(_float fTimeDelta) PURE;
	virtual void LateTick(_float fTimeDelta) PURE;
	virtual HRESULT Render() PURE;

//Virtual Public Function
public:
	virtual HRESULT Initialize_Active(void* pArg) { return S_OK; }

//Pure Virtual Protected Function
protected:
	virtual void Initialize_Set_Scale_Pos_Rotation(void* pArg) PURE;

//For LifeUi
protected:
	virtual void Default_Set_LifeTime();

//for UiActive
protected:
	virtual void Initialize_Set_ActiveTime();
	virtual void Initialize_Set_Size();

public:
	bool Is_Dead() { return m_bDead; }
	void Set_Dead() { m_bDead = true; }

	bool Get_Active() { return m_bActive; }
	void Set_Active(bool _isActive, void* pArg = nullptr)
	{
		if (_isActive == true && m_bActive == true)
		{
			return;
		}

		if (_isActive == true)
		{
			Initialize_Active(pArg);
		}

		m_bActive = _isActive;
	}


	_float Get_Divide() { return m_fDivide; }
	void Set_Divide(_float fDivide) { m_fDivide = fDivide; }

	_float2 Get_Pos() { return _float2(m_fX, m_fY); }
	void Set_Pos(_float2 fPos) { m_fX = fPos.x; m_fY = fPos.y; }

	void Set_Texture_Index(_uint iIndex) { m_iTexture_Index = iIndex; }
	_uint Get_Texture_Index() { return m_iTexture_Index; }
	void Add_Texture_Index(_uint iNum) { m_iTexture_Index += iNum; }

	_int Get_IndexNum() { return m_iIndexNum; }
	void Set_IndexNum(_int iIndexNum) { m_iIndexNum = iIndexNum; }

protected:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

protected:
	_float						m_fX = {};
	_float						m_fY = {};
	_float						m_fSizeX = {};
	_float						m_fSizeY = {};
	_float4x4					m_ViewMatrix = {};
	_float4x4					m_ProjMatrix = {};

	_int						m_iIndexNum = { 0 };
protected:
	void Set_Lifetime(_float Lifetime) { m_fLifeTime = Lifetime; }
	void Cal_Life_Blink(_float fTimeDelta);
	bool Cal_BlinkRender(_float BlinkGap);


protected:
	_float m_fLifeTime = { 0.f };
	_float m_fBlink = { 0.f };
	bool m_bBlink = { false };

protected:
	bool m_bDead = { false };
	Ui_Pos_Size_Rotation m_UiDesc;
	bool m_bActive = { false };
	_float m_fActiveTime = { 0.f };
	_uint m_iTexture_Index = { 0 };

	_float3 m_fRotation = { 0.f, 0.f, 0.f };
	_float3 m_fScale = { 1.f, 1.f, 1.f };

	_float3 m_fSpeed = { 0.f, 0.f, 0.f };
	_float  m_fDivide = { 0.f };

//protected:
	bool m_bEnter = { false };


public:
	virtual void Free() override;
	virtual CUi* Clone(void* pArg);
};

END
