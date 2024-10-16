#pragma once

#include "Base.h"

BEGIN(Engine)

class CMath_Manager : public CBase
{

private:
	CMath_Manager();
	virtual ~CMath_Manager() = default;


public:
	void Initialize();
	void Tick(_float fTimeDelta);
	void Clear();
	void Start();
	void Render();

public:
	_float2 Lissajous_Curve(IN _float _fTimeDelta, IN OUT _float& _fLissajousTime, IN _float _fWitth, IN _float _fHeight, IN _float _fLagrangianX, IN _float _fLagrangianY, IN _float _fPhaseDelta, IN _float _fLissajousSpeed);
	_float3 Vector3x3_BezierCurve(IN _float3 _vStart, IN _float3 _vControl, IN _float3 _vEnd, IN OUT _float& _fTimeControl, IN _float _fTimeDelta, IN _float _fSpeed = 1.f);
	//_float2 Vector2x3_BezierCurve(IN _float2 _vStart, IN _float2 _vControl, IN _float2 _vEnd, IN OUT _float& _fTimeControl, IN _float _fTimeDelta, IN _float _fSpeed = 1.f);
	_vector XMQuaternionRotationBetweenVectors(XMVECTOR start, XMVECTOR end);
private:

public:
	_float  Random_Float(_float _fMin, _float _fMax);
	_float2 Random_Float2(_float2 _f2Min, _float2 _f2Max);
	_float3 Random_Float3(_float3 _f3Min, _float3 _f3Max);

	_int	Random_Int(_int _iMin, _int _iMax);

	_int	Random_Binary();
	_bool	Random_Bool();

	_float  BezierB1(_float t);
	_float  BezierB2(_float t);
	_float  BezierB3(_float t);

	PxVec3  Add_PxVec3toXMVECTOR(PxVec3 _vec3, _vector _XMVEC);
private:
	_bool m_bLiteRandomMode = false;

public:
	static CMath_Manager* Create();
	virtual void Free() override;
};

END