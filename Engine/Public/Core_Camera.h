#pragma once

#include "Base.h"
#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)

class CTransform;
class ENGINE_DLL CCoreCamera : public CGameObject
{

public:
	typedef struct Camdesc : public CTransform::TRANSFORM_DESC
	{
		_float4			vEye = {};
		_float4			vAt = {};
		_float			fFovy = {};
		_float			fAspect = {};
		_float			fNear = {};
		_float			fFar = {};

		_float			fMouseSensor = 0.1f;
	}CAMERA_DESC;

public:
	CCoreCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCoreCamera(const CCoreCamera& rhs);
	virtual ~CCoreCamera() = default;

	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() PURE;

	wstring Get_Key() const { return m_strKey; }

	void Set_Target(CGameObject* _pTarget) { m_pTarget = _pTarget; }
	CAMERA_DESC		Get_CAMERA_DESC() { return m_CameraDesc; }

	_matrix Get_WorldMatrix() { return m_pTransformCom->Get_WorldMatrix(); }
	_float4x4 Get_WorldMatrix_4x4() { return m_pTransformCom->Get_WorldFloatOrigin4x4(); }

protected:
	virtual void  Key_Input(_float fTimeDelta) {};
	virtual void  Camera_Event(_float fTimeDelta) {};
	void		  Set_Pos(const _float4 _vPos) { m_CameraDesc.vEye = _vPos; }
	void		  Set_At(const _float4 _vAt) { m_CameraDesc.vAt = _vAt; }

protected:
	CGameObject* m_pTarget = { nullptr };

	wstring		m_strKey;

	CAMERA_DESC m_CameraDesc;

	_float		m_MoveSensitivity = 1.f;

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

END