#include "Level_Tool.h"
#include "Camera_TPV.h"
#include "Camera_Tool.h"
#include "GameInstance.h"

CLevel_Tool::CLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{

}

HRESULT CLevel_Tool::Initialize()
{
	m_iLevelID = LEVEL_TOOL;

	GAMEINSTANCE->Set_Imgui_State(eImgui_State::IMGUI_STATE_RENDER);
	
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if(FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
		return E_FAIL;

	if(FAILED(Ready_Light()))
		return E_FAIL;


	return S_OK;
}

void CLevel_Tool::Tick(_float fTimeDelta)
{
	_int iFps = (_int)(1.f / fTimeDelta);
	SetWindowText(g_hWnd, to_wstring(iFps).c_str());

	return;
}

HRESULT CLevel_Tool::Render()
{

	//SetWindowText(g_hWnd, TEXT("Edit Tool"));

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_Camera(const wstring& strLayerTag)
{
	CCamera_Tool::CAMERA_DESC			CameraDesc{};

	CameraDesc.fMouseSensor = 0.05f;
	
	CameraDesc.vEye = _float4(1049.83f, 17.67f, -321.63f, 1.f);
	CameraDesc.vAt = _float4(1049.83f, 17.67f, -296.63f, 1.f);

	if (m_eToolMap == CITY) {
		CameraDesc.vEye = _float4(60.f, 20.f, -55.f, 1.f);
		CameraDesc.vAt = _float4(60.f, 20.f, -30.f, 1.f);
	}

	//Lab
	if (m_eToolMap == LAB) {
		CameraDesc.vEye = _float4(1049.83f, 17.67f, -321.63f, 1.f);
		CameraDesc.vAt = _float4(1049.83f, 17.67f, -296.63f, 1.f);
	}

	CameraDesc.fFovy = XMConvertToRadians(90.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 3000.f;
	CameraDesc.fSpeedPerSec = 20.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	CCamera_Tool* Camera = dynamic_cast<CCamera_Tool*>(m_pGameInstance->Add_Clone_Return(LEVEL_TOOL, strLayerTag, TEXT("Prototype_GameObject_Camera_Tool"), &CameraDesc));

	m_pGameInstance->Create_Camera(Camera, TEXT("Camera_Tool_Main"));

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_Map(const wstring& strLayerTag)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (m_eToolMap == CITY) {
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL, strLayerTag, TEXT("Prototype_Map_KillRun_1"), &TransformDesc)))
			return E_FAIL;
	}

	if (m_eToolMap == LAB) {
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL, strLayerTag, TEXT("Prototype_Map_KillRun_2"), &TransformDesc)))
			return E_FAIL;
	}

	if (m_eToolMap == BOSS) {
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL, strLayerTag, TEXT("Prototype_Map_Last"), &TransformDesc)))
			return E_FAIL;
	}



	/*m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_Main_Skill"));
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_Main_Border"));
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_Main_Gauge"));
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_Main_Icon"));
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_IconBack1"));
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_Icon1"));
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_IconBack2"));
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_Icon2"));
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_IconBack3"));
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_Icon3"));*/

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	/*CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	XMStoreFloat4x4(&TransformDesc.matWorld, XMMatrixIdentity());
	XMStoreFloat4x4(&TransformDesc.matWorld, XMMatrixScaling(0.01f, 0.01f, 0.01f));

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL, strLayerTag, TEXT("Prototype_GameObject_PistolGunner"), &TransformDesc)))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_Effect(const wstring& strLayerTag)
{

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Light()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDiffuse = _float4(0.4f, 0.4f, 0.4f, 1.f);
	strcpy_s(LightDesc.szName, "Light_Directional_Main");
	LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	if (FAILED(m_pGameInstance->Add_Light(&LightDesc)))
		return E_FAIL;

	/*LightDesc.vDirection = _float4(-1.f, -1.f, -1.f, 0.f);

	if (FAILED(m_pGameInstance->Add_Light(&LightDesc)))
		return E_FAIL;*/

	return S_OK;
}




CLevel_Tool* CLevel_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Tool* pInstance = new CLevel_Tool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Tool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Tool::Free()
{
	__super::Free();

}
