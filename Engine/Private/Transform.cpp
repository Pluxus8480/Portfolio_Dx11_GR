#include "..\Public\Transform.h"
#include "Shader.h"
#include "Navigation.h"

CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent{ pDevice, pContext }
{
}

CTransform::CTransform(const CTransform & rhs)
	: CComponent{ rhs }
	, m_WorldMatrix{ rhs.m_WorldMatrix }
{

}


_float3 CTransform::Get_Scaled()
{
	_matrix		WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);

	return _float3(XMVectorGetX(XMVector3Length(WorldMatrix.r[STATE_RIGHT])), 
		XMVectorGetX(XMVector3Length(WorldMatrix.r[STATE_UP])), 
		XMVectorGetX(XMVector3Length(WorldMatrix.r[STATE_LOOK])));
}

void CTransform::Set_Scale(const _float3 & vScale)
{
	XMMATRIX		WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);

	//for (size_t i = 0; i < STATE_POSITION; i++)
	//	Set_State(STATE(i), XMVector3Normalize(WorldMatrix.r[i]) * ((_float*)&vScale)[i]);


	Set_State(STATE_RIGHT, XMVector3Normalize(WorldMatrix.r[STATE_RIGHT]) * vScale.x);
	Set_State(STATE_UP, XMVector3Normalize(WorldMatrix.r[STATE_UP]) * vScale.y);
	Set_State(STATE_LOOK, XMVector3Normalize(WorldMatrix.r[STATE_LOOK]) * vScale.z);

}

void CTransform::Rotation_XYZ(_float3 vXYZ)
{
	// Get current RotationMatrix from the WorldMatrix by decomposition.
	_vector vScale, vRotationQuat, vTranslation;
	XMMatrixDecompose(&vScale, &vRotationQuat, &vTranslation, Get_WorldMatrix());
	_matrix RotationMatrix = XMMatrixRotationQuaternion(vRotationQuat);

	// Multiply the WorldMatrix by the Inverse of current RotationMatrix (to get a WorldMatrix without any Rotation)
	_matrix InverseRotationMatrix = XMMatrixInverse(nullptr, RotationMatrix);
	_matrix WorldMatrixWithoutRotation = XMMatrixMultiply(Get_WorldMatrix(), InverseRotationMatrix);

	// Make a NewRotationMatrix with new angle values
	_matrix NewRotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(vXYZ.x), XMConvertToRadians(vXYZ.y), XMConvertToRadians(vXYZ.z));
	m_fCurrentRotationX = vXYZ.x;
	m_fCurrentRotationY = vXYZ.y;
	m_fCurrentRotationZ = vXYZ.z;

	// Set NewRotationMatrix to WorldMatrixWithoutRotation
	Set_State(CTransform::STATE_RIGHT, XMVector3TransformNormal(WorldMatrixWithoutRotation.r[0], NewRotationMatrix));
	Set_State(CTransform::STATE_UP, XMVector3TransformNormal(WorldMatrixWithoutRotation.r[1], NewRotationMatrix));
	Set_State(CTransform::STATE_LOOK, XMVector3TransformNormal(WorldMatrixWithoutRotation.r[2], NewRotationMatrix));
}

void CTransform::Head_Roll(_float fRadian)
{
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	_matrix		RotationMatrix = XMMatrixRotationAxis(vLook, fRadian);

	// XMVector3TransformNormal
	Set_State(STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector4Transform(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector4Transform(vLook, RotationMatrix));
}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return S_OK;

	TRANSFORM_DESC*		pTransformDesc = static_cast<TRANSFORM_DESC*>(pArg);

	m_fSpeedPerSec = pTransformDesc->fSpeedPerSec;
	m_fRotationPerSec = pTransformDesc->fRotationPerSec;
	Set_State(STATE_POSITION, XMLoadFloat4(&pTransformDesc->vPosition));

	return S_OK;
}

void CTransform::Go_Straight(_float fTimeDelta)
{
	_vector	vPosition = Get_State(STATE_POSITION);
	_vector	vLook = Get_State(STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Straight(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector	vPosition = Get_State(STATE_POSITION);
	_vector	vLook = Get_State(STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	/* isMove() : 이대로 움직여도 되냐? */
	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition))
		Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_PureStraight(_float fTimeDelta)
{
	_vector	vPosition = Get_State(STATE_POSITION);
	_vector	vLook = Get_State(STATE_LOOK);

	vLook = XMVectorSetY(vLook, 0.f);
	vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Vector(_float fTimeDelta, _fvector vVector)
{
	_vector	vPosition = Get_State(STATE_POSITION);

	vPosition += XMVector3Normalize(vVector) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta)
{
	_vector	vPosition = Get_State(STATE_POSITION);
	_vector	vRight = Get_State(STATE_RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_PureLeft(_float fTimeDelta)
{
	_vector	vPosition = Get_State(STATE_POSITION);
	_vector	vRight = Get_State(STATE_RIGHT);

	vRight = XMVectorSetY(vRight, 0.f);
	vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta)
{
	_vector	vPosition = Get_State(STATE_POSITION);
	_vector	vRight = Get_State(STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_PureRight(_float fTimeDelta)
{
	_vector	vPosition = Get_State(STATE_POSITION);
	_vector	vRight = Get_State(STATE_RIGHT);

	vRight = XMVectorSetY(vRight, 0.f);
	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Backward(_float fTimeDelta)
{
	_vector	vPosition = Get_State(STATE_POSITION);
	_vector	vLook = Get_State(STATE_LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_PureBackward(_float fTimeDelta)
{
	_vector	vPosition = Get_State(STATE_POSITION);
	_vector	vLook = Get_State(STATE_LOOK);

	vLook = XMVectorSetY(vLook, 0.f);
	vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Up(_float fTimeDelta)
{
	_vector	vPosition = Get_State(STATE_POSITION);
	_vector	vUp = Get_State(STATE_UP);

	vPosition += XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Down(_float fTimeDelta)
{
	_vector	vPosition = Get_State(STATE_POSITION);
	_vector	vUp = Get_State(STATE_UP);

	vPosition -= XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}
void CTransform::Go_PixDown(_float Down)
{
	_vector	vPosition = Get_State(STATE_POSITION);
	_vector	vUp = Get_State(STATE_UP);

	vPosition -= XMVector3Normalize(vUp) * Down;

	Set_State(STATE_POSITION, vPosition);
}
/* 항상 항등회전(회전이 없는 상태)기준 지정한 각도만큼 회전한다. */
/* Radian = XMConvertToRadians(Degree) */
void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_float3		vScaled = Get_Scaled();

	/* 회전은 항등상태로!, 스케일은 기존 상태로. */
	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

 	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	// XMVector3TransformNormal
	Set_State(STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector4Transform(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector4Transform(vLook, RotationMatrix));
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	/* 현재 회전 상태를 얻어온다. */
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

	// XMVector3TransformNormal
	Set_State(STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector4Transform(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector4Transform(vLook, RotationMatrix));
}

void CTransform::PureTurn(_fvector vAxis, _float fRadian)
{
	/* 현재 회전 상태를 얻어온다. */
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	// XMVector3TransformNormal
	Set_State(STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector4Transform(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector4Transform(vLook, RotationMatrix));
}

void CTransform::Rotation_Qt(_vector vQuat)
{
	_float3		vScaled = Get_Scaled();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

	_matrix		RotationMatrix = XMMatrixRotationQuaternion(vQuat);

	// XMVector3TransformNormal
	Set_State(STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector4Transform(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector4Transform(vLook, RotationMatrix));
}

void CTransform::Rotation_Qt(_float3 vDegree)
{
	_float3		vScaled = Get_Scaled();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

	_matrix		RotationMatrix = XMMatrixRotationRollPitchYawFromVector(XMVectorSet(XMConvertToRadians(vDegree.x),XMConvertToRadians(vDegree.y), XMConvertToRadians(vDegree.z),0.f));

	// XMVector3TransformNormal
	Set_State(STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector4Transform(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector4Transform(vLook, RotationMatrix));
}

void CTransform::LookAt(_fvector vAt)
{
	_vector		vLook = XMVectorSetY(vAt - Get_State(STATE_POSITION),XMVectorGetY(Get_State(STATE_LOOK)));
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector		vUp = XMVector3Cross(vLook, vRight);

	_float3		vScaled = Get_Scaled();

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScaled.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

void CTransform::BillBoarding(_float4x4* _BillBoardmatrix)
{
	_float3 vScaled = Get_Scaled();

	m_WorldMatrix.m[0][0] = _BillBoardmatrix->m[0][0];
	m_WorldMatrix.m[0][2] = _BillBoardmatrix->m[0][2];
	m_WorldMatrix.m[2][0] = _BillBoardmatrix->m[2][0];
	m_WorldMatrix.m[2][2] = _BillBoardmatrix->m[2][2];

	Set_Scale(vScaled);
}

_vector CTransform::Get_Position_NoneY()
{
	_vector Position = Get_State(STATE_POSITION);

	return XMVectorSetY(Position, 0.f);
}

void CTransform::Set_Position(_fvector vPosition)
{
	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Set_Position(_float x, _float y, _float z)
{
	Set_State(STATE_POSITION, XMVectorSet(x, y, z, 1.f));
}

void CTransform::Set_Position(POSITION eType, _float fValue)
{
	_vector vPosition = Get_State(STATE_POSITION);

	switch (eType)
	{
	case POSITION::POS_X:
		vPosition = XMVectorSetX(vPosition, fValue);
		break;
	case POSITION::POS_Y:
		vPosition = XMVectorSetY(vPosition, fValue);
		break;
	case POSITION::POS_Z:
		vPosition = XMVectorSetZ(vPosition, fValue);
		break;
	}

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Set_Position(_float4 vPosition)
{
	Set_State(STATE_POSITION, XMVectorSet(vPosition.x,vPosition.y, vPosition.z, 1.f));
}

HRESULT CTransform::Bind_ShaderResource(CShader * pShader, const _char * pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);	
}

CTransform * CTransform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTransform*		pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTransform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent * CTransform::Clone(void * pArg)
{
	return nullptr;
}

void CTransform::Free()
{
	__super::Free();
}
