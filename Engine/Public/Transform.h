#pragma once

#include "Component.h"

/* XMFLOAT2, XMFLOAT3, XMFLOAT4 == XMVECTOR */

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	typedef struct tagTransformDesc
	{
		_float		fSpeedPerSec = {};
		_float		fRotationPerSec = {};
		_float4		vPosition = { 0.f, 0.f, 0.f, 1.f};
		_float4x4	matWorld = {};
	}TRANSFORM_DESC;

public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };
	enum POSITION { POS_X, POS_Y, POS_Z, POS_END };

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_vector Get_State(STATE eState) {
		return XMLoadFloat4((_float4*)&m_WorldMatrix.m[eState][0]);
	}

	void Set_WorldMatrix(const _float4x4& matWorld) {
		m_WorldMatrix = matWorld;
	}

	_matrix Get_WorldMatrix() const {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}

	const _float4x4* Get_WorldFloat4x4() const {
		return &m_WorldMatrix;
	}

	const _float4x4 Get_WorldFloatOrigin4x4() const {
		return m_WorldMatrix;
	}

	void Set_State(STATE eState, _fvector vState) {
		XMStoreFloat4((_float4*)&m_WorldMatrix.m[eState][0], vState);
	}
	void Set_Scale(const _float3& vScale);
	void Rotation_XYZ(_float3 vXYZ);

	_vector Get_Straight() { return XMVector3Normalize(Get_State(STATE_LOOK)); }
	_vector Get_PureStraight() { return XMVectorSetY(XMVector3Normalize(Get_State(STATE_LOOK)), 0.f);}
	_vector Get_Right() { return XMVector3Normalize(Get_State(STATE_RIGHT)); }
	_vector Get_PureRight() { return XMVectorSetY(XMVector3Normalize(Get_State(STATE_RIGHT)), 0.f); }
	_vector Get_Up() { return XMVector3Normalize(Get_State(STATE_UP)); }

	void Set_Speed(_float fSpeed) { m_fSpeedPerSec = fSpeed; }
	_float Get_Speed() { return m_fSpeedPerSec; }

	void Head_Roll(_float fRadian);
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override; 

public:

	void Go_Straight(_float fTimeDelta);
	void Go_Straight(_float fTimeDelta, class CNavigation* pNavigation);
	void Go_PureStraight(_float fTimeDelta);
	void Go_Vector(_float fTimeDelta,_fvector vVector);

	void Go_Left(_float fTimeDelta);
	void Go_PureLeft(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);
	void Go_PureRight(_float fTimeDelta);
	void Go_Backward(_float fTimeDelta);
	void Go_PureBackward(_float fTimeDelta);
	void Go_Up(_float fTimeDelta);
	void Go_Down(_float fTimeDelta);
	void Go_PixDown(_float Down);

	void Rotation(_fvector vAxis, _float fRadian);
	void Turn(_fvector vAxis, _float fTimeDelta);
	void PureTurn(_fvector vAxis, _float fRadian);

	void Rotation_Qt(_vector vQuat);
	void Rotation_Qt(_float3 vDegree);
	void LookAt(_fvector vAt);

	void BillBoarding(_float4x4* _BillBoardmatrix);
	_vector Get_Position_NoneY();

	void Set_Position(_fvector vPosition);
	void Set_Position(_float x, _float y, _float z);
	void Set_Position(POSITION eType, _float fValue);
	void Set_Position(_float4 vPosition);

	_float Get_Scale(STATE eState) { return XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_WorldMatrix).r[eState])); }
	_float3 Get_Scaled();
public:
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);

private:
	_float4x4			m_WorldMatrix = {};
	_float				m_fSpeedPerSec = { 0.f };
	_float				m_fRotationPerSec = { 0.f };

	_float				m_fCurrentRotationX = { 0.f };
	_float				m_fCurrentRotationY = { 0.f };
	_float				m_fCurrentRotationZ = { 0.f };

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END