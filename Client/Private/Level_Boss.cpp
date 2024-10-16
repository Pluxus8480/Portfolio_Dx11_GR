#include "Level_Boss.h"
#include "Camera_TPV.h"
#include "GameInstance.h"
#include "PlayerManager.h"
#include "EnemyManager.h"
#include "ShockWave.h"

CLevel_Boss::CLevel_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{

}

HRESULT CLevel_Boss::Initialize()
{
	m_iLevelID = LEVEL_LAST;

	//
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	/* 로딩 레벨에 보여주기위한 객체(배경, 로딩바, 로딩상태폰트)들을 생성한다. */
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Light()))
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

	PLAYER->GameRestart();

	GAMEINSTANCE->Play(TEXT("Sound_BGM3"), true);
	GAMEINSTANCE->SetVolume(TEXT("Sound_BGM3"), 0.15f);

	//if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
	//	return E_FAIL;

	//if (FAILED(Ready_LandObjects()))
	//	return E_FAIL;

	return S_OK;
}

void CLevel_Boss::Tick(_float fTimeDelta)
{
	_int iFps = (_int)(1.f / fTimeDelta);
	SetWindowText(g_hWnd, to_wstring(iFps).c_str());
	//SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));

	ShowCursor(false);

	if (GAMEINSTANCE->GetKeyDown(eKeyCode::E))
		GAMEINSTANCE->LightControl("Light_Point");

	if (m_bIsPhase == 0 && ENEMYMANAGER->Get_EnemyCnt() == 0)
	{
		m_bIsPhase = 1;
	}

	if (m_bIsPhase == 1 && XMVectorGetY(GAMEINSTANCE->Get_CurCamera_Transform()->Get_State(CTransform::STATE_POSITION)) < -70.f) {
		PLAYER->Set_Real_Position(_float3(538.533f, -205.581f, -221.736f));
		PLAYER->Get_RigidBody()->Set_Velocity(PxVec3(0.f, 0.f, 0.f));
		PLAYER->Set_PlayerState(CPlayer_Manager::PLAYER_JUMP);
		m_bIsPhase = 2;

		GAMEINSTANCE->Play(TEXT("Explode"), false);
		GAMEINSTANCE->SetVolume(TEXT("Explode"), 1.f);

		GAMEINSTANCE->Set_ShakePower(1.f);
		GAMEINSTANCE->Set_ShakeTime(1.5f);

		CTransform::TRANSFORM_DESC	TransformDesc{};
		TransformDesc.vPosition = _float4(506.179f, -206.776f, -193.272f, 1.f);

		m_pGameInstance->Add_Clone_Return(LEVEL_LAST, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Boss"), &TransformDesc);

	}

	if (m_bIsPhase == 2 && PLAYER->Get_PlayerToTargetDistance(XMVectorSet(584.336f, -205.482f, -287.684f, 1.f)) < 5.f) GAMEINSTANCE->Clear(LEVEL_LAST);

	return;
}

HRESULT CLevel_Boss::Render()
{


	return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Camera(const wstring& strLayerTag)
{
	PLAYER->Set_Real_Position(_float3(513.278f,-85.680f,-212.185f));
	/*CCamera_TPV::CAMERA_DESC			CameraDesc{};

	CameraDesc.fMouseSensor = 0.05f;
	CameraDesc.vEye = _float4(60.f, 20.f, -55.f, 1.f);
	CameraDesc.vAt = _float4(60.f, 20.f, -30.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(90.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 3000.f;
	CameraDesc.fSpeedPerSec = 20.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);*/

	//CCamera_TPV* Camera = dynamic_cast<CCamera_TPV*>(m_pGameInstance->Add_Clone_Return(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_Camera_TPV"), &CameraDesc));

	//m_pGameInstance->Create_Camera(Camera, TEXT("Camera_TPV_Main"));

	return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Terrain"))))
		//return E_FAIL;

	/*CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);*/

	/*if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_PistolGunner"), &TransformDesc)))
		return E_FAIL;*/

		//PLAYER->Set_PlayerArm(dynamic_cast<CPlayerArm*>(m_pGameInstance->Add_Clone_Return(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_PlayerArm"), &TransformDesc)));

	return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Map(const wstring& strLayerTag)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_LAB, strLayerTag, TEXT("Prototype_Map_Last"), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Monster(const wstring& strLayerTag)
{
	//CTransform::TRANSFORM_DESC	TransformDesc{};
	//TransformDesc.vPosition = _float4(1580.05176f, -15.0000153f, -330.573792f, 1.f);

	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_LAB, strLayerTag, TEXT("Prototype_GameObject_Samurai"), &TransformDesc)))
	//	return E_FAIL;

	//TransformDesc.vPosition = _float4(1580.05176f, -15.0000153f, -340.573792f, 1.f);
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_LAB, strLayerTag, TEXT("Prototype_GameObject_Samurai"), &TransformDesc)))
	//	return E_FAIL;

	//TransformDesc.vPosition = _float4(1580.05176f, -15.0000153f, -320.573792f, 1.f);
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_LAB, strLayerTag, TEXT("Prototype_GameObject_Samurai"), &TransformDesc)))
	//	return E_FAIL;

	/*CTransform::TRANSFORM_DESC	TransformDesc{};
	TransformDesc.vPosition = _float4(60.f, 20.f, -40.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Blood"), &TransformDesc)))
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

	GAMEINSTANCE->Map_Load(LEVEL_LAST);
	ENEMYMANAGER->EnemyPooling(GAMEINSTANCE->Find_Layer(LEVEL_LAST, TEXT("Layer_Monster")));
	return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Effect(const wstring& strLayerTag)
{


	return S_OK;
}

HRESULT CLevel_Boss::Ready_Light()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDiffuse = _float4(0.3f, 0.3f, 0.3f, 1.f);
	strcpy_s(LightDesc.szName, "Light_Directional_Main");
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

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

HRESULT CLevel_Boss::Ready_LandObjects()
{


	return S_OK;
}




CLevel_Boss* CLevel_Boss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Boss* pInstance = new CLevel_Boss(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Boss"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Boss::Free()
{
	__super::Free();

}
