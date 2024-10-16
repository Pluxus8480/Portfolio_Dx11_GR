#include "Camera_Manager.h"
#include "GameInstance.h"

CCamera_Manager::CCamera_Manager()
{
}

CTransform* CCamera_Manager::Get_CurCamera_Transform()
{
	if(m_pCurCamera == nullptr)
		return nullptr;
	
	return m_pCurCamera->Get_Transform();
}

void CCamera_Manager::Set_Transform(TRANSFORMATIONSTATE eState, _fmatrix TransformationMatrix)
{
	XMStoreFloat4x4(&m_TransformationMatrices[eState], TransformationMatrix);
}

_matrix CCamera_Manager::Get_Transform_Matrix(TRANSFORMATIONSTATE eState) const
{
	return XMLoadFloat4x4(&m_TransformationMatrices[eState]);
}

const _float4x4* CCamera_Manager::Get_Transform_Float4x4(TRANSFORMATIONSTATE eState) const
{
	return &m_TransformationMatrices[eState];
}

_matrix CCamera_Manager::Get_Transform_Matrix_Inverse(TRANSFORMATIONSTATE eState) const
{
	return XMLoadFloat4x4(&m_TransformationMatrix_Inverse[eState]);
}

const _float4x4* CCamera_Manager::Get_Transform_Float4x4_Inverse(TRANSFORMATIONSTATE eState) const
{
	return &m_TransformationMatrix_Inverse[eState];
}

_vector CCamera_Manager::Get_CamPosition_Vector() const
{
	return XMLoadFloat4(&m_vCamPosition);
}

const _float4* CCamera_Manager::Get_CamPosition_Float4() const
{
	return &m_vCamPosition;
}

_matrix CCamera_Manager::Get_BillBoardMatrix() const
{
	return XMLoadFloat4x4(&m_BillBoardMatrix);
}

_float4x4* CCamera_Manager::Get_BillBoardMatrix4x4()
{
	return &m_BillBoardMatrix;
}

_matrix CCamera_Manager::Get_WorldMatrix() const
{
	return m_pCurCamera->Get_WorldMatrix();
}

void CCamera_Manager::Camera_Shake()
{

	if (GAMEINSTANCE->GetKeyDown(eKeyCode::Y))
	{
		m_fShakeTime = 1.f;
		m_fShakePower = 1.f;
	}

	if (m_fShakeTime > 0.f)
	{
		m_fShakeTime -= GAMEINSTANCE->Get_TimeDelta(TEXT("Timer_60"));
		m_fShakePower = m_fShakeTime * m_fShakePower;
		_float fShakeX = GAMEINSTANCE->Random_Float(0.f, 100.f) * 0.01f * m_fShakePower;
		_float fShakeY = GAMEINSTANCE->Random_Float(0.f, 100.f) * 0.01f * m_fShakePower;
		_float fShakeZ = GAMEINSTANCE->Random_Float(0.f, 100.f) * 0.01f * m_fShakePower;

		m_TempWorldMatrix = XMMatrixMultiply(m_TempWorldMatrix, XMMatrixTranslation(fShakeX, fShakeY, fShakeZ));
	}
	else
	{
		m_fShakeTime = 0.f;
		m_fShakePower = 0.f;
	}

}

HRESULT CCamera_Manager::Initialize()
{
	for (size_t i = 0; i < D3DTS_END; i++)
	{
		XMStoreFloat4x4(&m_TransformationMatrices[i], XMMatrixIdentity());
		XMStoreFloat4x4(&m_TransformationMatrix_Inverse[i], XMMatrixIdentity());
	}
	XMStoreFloat4x4(&m_BillBoardMatrix, XMMatrixIdentity());
	XMStoreFloat4(&m_vCamPosition, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	return S_OK;
}

HRESULT CCamera_Manager::Change_Camera(const wstring& _wstrCameraKey)
{
	auto iter = m_mapCamera.find(_wstrCameraKey);

	if (iter == m_mapCamera.end())
	{
		MSG_BOX(TEXT("Not Found Camera"));
		assert(false);
		return E_FAIL;
	}

	m_pCurCamera = iter->second;

	return S_OK;
}

HRESULT CCamera_Manager::Create_Camera(CCoreCamera* _pCamera, const wstring& _wstrCameraKey)
{
	if (_pCamera == nullptr)
		return E_FAIL;
	
	auto iter = m_mapCamera.find(_wstrCameraKey);
	
	if (iter != m_mapCamera.end())
	{
		Safe_Release(_pCamera);
		//MSG_BOX(TEXT("Already Exist Camera"));
		return S_OK;
	}
	
	if (nullptr == _pCamera)
	{
		MSG_BOX(TEXT("Failed to Create Camera"));
		return E_FAIL;
	}
	
	m_pCurCamera = _pCamera;
	m_mapCamera.emplace(_wstrCameraKey, _pCamera);
	return S_OK;
}

void CCamera_Manager::Update()
{
	if(m_pCurCamera == nullptr)
		return;

	CCoreCamera::CAMERA_DESC tDesc = m_pCurCamera->Get_CAMERA_DESC();

	for (size_t i = 0; i < D3DTS_END; i++)
	{
		XMStoreFloat4x4(&m_TransformationMatrix_Inverse[i],
			XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformationMatrices[i])));
	}


	tDesc.fFovy = XMConvertToRadians(m_fFov);


	memcpy(&m_vCamPosition, &m_TransformationMatrix_Inverse[D3DTS_VIEW].m[3][0], sizeof(_float4));

	m_TempWorldMatrix = m_pCurCamera->Get_WorldMatrix();

	Camera_Shake();

	Set_Transform(D3DTS_VIEW, XMMatrixInverse(nullptr, m_TempWorldMatrix));

	Set_Transform(D3DTS_PROJ, XMMatrixPerspectiveFovLH(tDesc.fFovy, tDesc.fAspect, tDesc.fNear, tDesc.fFar));

	memcpy(&m_BillBoardMatrix.m[1][1], &m_TransformationMatrix_Inverse[D3DTS_VIEW].m[1][1], sizeof(_float));
	memcpy(&m_BillBoardMatrix.m[1][3], &m_TransformationMatrix_Inverse[D3DTS_VIEW].m[1][3], sizeof(_float));
	memcpy(&m_BillBoardMatrix.m[3][1], &m_TransformationMatrix_Inverse[D3DTS_VIEW].m[3][1], sizeof(_float));
	memcpy(&m_BillBoardMatrix.m[3][3], &m_TransformationMatrix_Inverse[D3DTS_VIEW].m[3][3], sizeof(_float));

	XMStoreFloat4x4(&m_BillBoardMatrix, XMMatrixInverse(nullptr,XMLoadFloat4x4(&m_BillBoardMatrix)));

	/*m_BillboardMatrix._11 = m_ViewMatrix._11;
	m_BillboardMatrix._13 = m_ViewMatrix._13;
	m_BillboardMatrix._31 = m_ViewMatrix._31;
	m_BillboardMatrix._33 = m_ViewMatrix._33;*/

}

CCamera_Manager* CCamera_Manager::Create()
{
	CCamera_Manager* pInstance = new CCamera_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CCamera_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Manager::Free()
{
	__super::Free();
}
