#include "Level_GamePlay.h"
#include "Camera_TPV.h"
#include "GameInstance.h"
#include "PlayerManager.h"
#include "EnemyManager.h"
#include "Level_Loading.h"
#include "ShockWave.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext } 
{

}

HRESULT CLevel_GamePlay::Initialize()
{
	m_iLevelID = LEVEL_GAMEPLAY;

	//
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	/* 로딩 레벨에 보여주기위한 객체(배경, 로딩바, 로딩상태폰트)들을 생성한다. */
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
		return E_FAIL;

	if(FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if(FAILED(Ready_Light()))
		return E_FAIL;

	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_Main_Skill"));
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_Main_Border"));
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_Main_Gauge"));
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_Main_Icon"));
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_IconBack1"));
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_Icon1"));
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_IconBack2"));
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_Icon2"));
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_IconBack3"));
	m_pGameInstance->Set_Ui_ActiveState(TEXT("SKILL_Icon3"));
	m_pGameInstance->Set_Ui_ActiveState(TEXT("Aim"));
	m_pGameInstance->Set_Ui_ActiveState(TEXT("HpBar"));
	
	m_pGameInstance->Play(TEXT("Sound_BGM"), true);
	m_pGameInstance->SetVolume(TEXT("Sound_BGM"), 0.15f);
	//if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
	//	return E_FAIL;

	//if (FAILED(Ready_LandObjects()))
	//	return E_FAIL;

	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	_int iFps = (_int)(1.f / fTimeDelta);
	SetWindowText(g_hWnd, to_wstring(iFps).c_str());
	//SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));

	ShowCursor(false);

	if (GAMEINSTANCE->GetKeyDown(eKeyCode::E))
		GAMEINSTANCE->LightControl("Light_Point");
	
	if (  ENEMYMANAGER->Get_EnemyCnt() == 0)
		Level_Chanege_Sequenece(fTimeDelta);

	if (GAMEINSTANCE->GetKeyDown(eKeyCode::Z)) {
		GAMEINSTANCE->LightClear();
		PLAYER->Set_Ui_Off();
		m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_LAB));
	}
		
	return;
}

HRESULT CLevel_GamePlay::Render()
{
	

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const wstring & strLayerTag)
{
	CCamera_TPV::CAMERA_DESC			CameraDesc{};

	CameraDesc.fMouseSensor = 0.05f;
	CameraDesc.vEye			= _float4(60.f, 20.f, -55.f, 1.f);
	CameraDesc.vAt			= _float4(60.f, 20.f, -30.f, 1.f);
	CameraDesc.fFovy		= XMConvertToRadians(90.0f);
	CameraDesc.fAspect		= (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear		= 0.1f;
	CameraDesc.fFar			= 3000.f;
	CameraDesc.fSpeedPerSec = 20.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	CCamera_TPV* Camera = dynamic_cast<CCamera_TPV*>(m_pGameInstance->Add_Clone_Return(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_Camera_TPV"), &CameraDesc));
		
	m_pGameInstance->Create_Camera(Camera,TEXT("Camera_TPV_Main"));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const wstring & strLayerTag)
{	
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Terrain"))))
		//return E_FAIL;

	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	/*if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_PistolGunner"), &TransformDesc)))
		return E_FAIL;*/

	PLAYER->Set_PlayerArm(dynamic_cast<CPlayerArm*>(m_pGameInstance->Add_Clone_Return(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_PlayerArm"), &TransformDesc)));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Map(const wstring& strLayerTag)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_Map_KillRun_1"), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const wstring& strLayerTag)
{
	/*if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Spark"))))
		return E_FAIL;*/


	CTransform::TRANSFORM_DESC	TransformDesc{};
	TransformDesc.vPosition = _float4(60.f, 20.f, -40.f, 1.f);

	/*if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Samurai"), &TransformDesc)))
		return E_FAIL;*/

	/*TransformDesc.vPosition = _float4(60.f, 15.f, -40.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Bullet"), &TransformDesc)))
		return E_FAIL;

	
	TransformDesc.vPosition = _float4(51.1f, -3.4f, 90.7f, 1.f);

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Uzi"), &TransformDesc)))
		return E_FAIL;


	TransformDesc.vPosition = _float4(55.1f, -3.4f, 90.7f, 1.f);

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Waver"), &TransformDesc)))
		return E_FAIL;*/




	GAMEINSTANCE->Map_Load(LEVEL_GAMEPLAY);
	ENEMYMANAGER->EnemyPooling(GAMEINSTANCE->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Monster")));
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const wstring & strLayerTag)
{


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Light()
{
		LIGHT_DESC			LightDesc{};

		LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
		LightDesc.vDiffuse = _float4(0.4f, 0.4f, 0.4f, 1.f);
		//LightDesc.szName = TEXT("Light_Directional_Main");
		strcpy_s(LightDesc.szName, "Light_Directional_Main");
		//LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
		LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
		LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
		LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

		if (FAILED(m_pGameInstance->Add_Light(&LightDesc)))
			return E_FAIL;


			LightDesc.eType = LIGHT_DESC::TYPE_POINT;
			LightDesc.vPosition = _float4(60.f, 15.f, -40.f, 1.f);
			LightDesc.fRange = 20.f;
			strcpy_s(LightDesc.szName, "Light_Point");
			LightDesc.vDiffuse = _float4(1.0f, 0.f, 0.f, 1.f);
			LightDesc.vAmbient = _float4(0.1f, 0.3f, 0.1f, 1.f);
			LightDesc.vSpecular = LightDesc.vDiffuse;

		if (FAILED(m_pGameInstance->Add_Light(&LightDesc)))
			return E_FAIL;

		/*LightDesc.vDirection = _float4(-1.f, -1.f, -1.f, 0.f);

		if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
			return E_FAIL;*/

		/*LightDesc.eType = LIGHT_DESC::TYPE_POINT;
		LightDesc.vPosition = _float4(10.f, 3.f, 10.f, 1.f);
		LightDesc.fRange = 20.f;

		LightDesc.vDiffuse = _float4(1.f, 0.0f, 0.f, 1.f);
		LightDesc.vAmbient = _float4(0.3f, 0.1f, 0.1f, 1.f);
		LightDesc.vSpecular = LightDesc.vDiffuse;

		if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
			return E_FAIL;

		LightDesc.eType = LIGHT_DESC::TYPE_POINT;
		LightDesc.vPosition = _float4(30.f, 3.f, 10.f, 1.f);
		LightDesc.fRange = 20.f;

		LightDesc.vDiffuse = _float4(0.0f, 1.f, 0.f, 1.f);
		LightDesc.vAmbient = _float4(0.1f, 0.3f, 0.1f, 1.f);
		LightDesc.vSpecular = LightDesc.vDiffuse;

		if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
			return E_FAIL;*/

		return S_OK;

}

HRESULT CLevel_GamePlay::Ready_LandObjects()
{
	

	return S_OK;
}

void CLevel_GamePlay::Level_Chanege_Sequenece(_float fTimeDelta)
{
	if (GAMEINSTANCE->Get_BlackScreenRate() > 0.85f) {
		m_pGameInstance->Stop(TEXT("Sound_BGM"));
		m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_LAB));
	}

	GAMEINSTANCE->LightClear();

	PLAYER->Set_Ui_Off();
	GAMEINSTANCE->Set_UseGlitchLens(true);
	GAMEINSTANCE->Set_UseRedBlockGlitch(true);

	GAMEINSTANCE->Set_GlitchPower(GAMEINSTANCE->Get_GlitchPower() * pow(1.5f, 0.016f));
	GAMEINSTANCE->Set_GlitchRate(GAMEINSTANCE->Get_GlitchRate() * pow(1.5f, 0.016f));
	GAMEINSTANCE->Set_GlitchSpeed(GAMEINSTANCE->Get_GlitchSpeed() * pow(2.f, 0.016f));
	GAMEINSTANCE->Set_GlitchColorRate(GAMEINSTANCE->Get_GlitchColorRate() * pow(1.5f, 0.016f));

	if (GAMEINSTANCE->Get_RedBlockMixRate() < 0.2f)
		GAMEINSTANCE->Set_RedBlockMixRate(GAMEINSTANCE->Get_RedBlockMixRate() * pow(5.f, 0.016f));

	if (GAMEINSTANCE->Get_RedBlockMixRate() > 0.2f)
		GAMEINSTANCE->Set_BlackScreen(true);

	if (GAMEINSTANCE->Get_BlackScreen() && GAMEINSTANCE->Get_BlackScreenRate() < 0.85f)
		GAMEINSTANCE->Set_BlackScreenRate(GAMEINSTANCE->Get_BlackScreenRate() + (0.016f * 0.33f));

	GAMEINSTANCE->Set_TimeMultiplier(0.1f);




}

CLevel_GamePlay * CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GamePlay*	pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_GamePlay"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();

}
