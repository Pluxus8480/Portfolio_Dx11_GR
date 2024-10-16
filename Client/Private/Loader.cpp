#include "Loader.h"

#include "GameInstance.h"

#include "BackGround.h"
#include "Terrain.h"
#include "ObjPoolManager.h"
#include "Load_Include.h"
//#include "Monster.h"
//#include "Player.h"
//#include "Camera.h"
//#include "Effect.h"
//#include "Sky.h"


CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pDevice);
}

// typedef unsigned(__stdcall* _beginthreadex_proc_type)(void*);
unsigned int APIENTRY Loading_Main(void* pArg)
{
	CLoader*		pLoader = (CLoader*)pArg;	

	return pLoader->Loading();	
}


HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Loading_Main, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

unsigned int CLoader::Loading()
{
	EnterCriticalSection(&m_CriticalSection);

	CoInitializeEx(nullptr, 0);

	HRESULT		hr{};

	switch (m_eNextLevelID)
	{
	case LEVEL_LOGO:
		hr = Loading_For_Logo_Level();
		break;
	case LEVEL_GAMEPLAY:
		hr = Loading_For_GamePlay_Level();
		break;
	case LEVEL_TOOL:
		hr = Loading_For_Tool_Level();
		break;
	case LEVEL_LAB:
		hr = Loading_For_Lab_Level();
		break;
	case LEVEL_LAST:
		hr = Loading_For_Boss_Level();
		break;
	}

	LeaveCriticalSection(&m_CriticalSection);

	if (FAILED(hr))
		return 1;

	return 0;
}

HRESULT CLoader::Load_Ui()
{
	if (FAILED(Load_Ui_Skill_Textures()))
		assert(false);

	if(FAILED(Load_Ui_Skill_Prototype()))
		assert(false);

	return S_OK;
}

HRESULT CLoader::Load_Ui_Skill_Textures()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Hp_Bar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GhostRunner/Texture/Hp/hud_boss_health_bar_gauge.png")))))
		assert(false);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("SKILL_Hud_Bar_Bg"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GhostRunner/Texture/Ui/Skill/hud_bar_bg.png")))))
		assert(false);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("SKILL_Hud_Bar_Border"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GhostRunner/Texture/Ui/Skill/hud_bar_outline.png")))))
		assert(false);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("SKILL_Hud_Bar_Energy_Bg"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GhostRunner/Texture/Ui/Skill/energy_bar_bg.png")))))
		assert(false);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("SKILL_Target"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GhostRunner/Texture/Ui/Skill/Target0.png")))))
		assert(false);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("SKILL_Icon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GhostRunner/Texture/Ui/Skill/Skill%d.png"),3))))
		assert(false);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("SKILL_IconBack"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GhostRunner/Texture/Ui/Skill/skill_select_selected_bg%d.png"), 2))))
		assert(false);


	return S_OK;
}

HRESULT CLoader::Load_Ui_Skill_Prototype()
{

	if (FAILED(m_pGameInstance->Add_Ui_Active(TEXT("SKILL_Main_Skill"),eUiRenderType::Render_Blend,
		CMainSkill::Create(m_pDevice, m_pContext))))
		assert(false);

	if (FAILED(m_pGameInstance->Add_Ui_Active(TEXT("SKILL_Main_Border"), eUiRenderType::Render_Blend,
		CMainBorder::Create(m_pDevice, m_pContext))))
		assert(false);

	if (FAILED(m_pGameInstance->Add_Ui_Active(TEXT("SKILL_Main_Gauge"), eUiRenderType::Render_Blend,
		CSkillEnergyBar::Create(m_pDevice, m_pContext))))
		assert(false);

	if (FAILED(m_pGameInstance->Add_Ui_Active(TEXT("SKILL_Main_Icon"), eUiRenderType::Render_Blend,
		CMainSkillIcon::Create(m_pDevice, m_pContext))))
		assert(false);

	if (FAILED(m_pGameInstance->Add_Ui_Active(TEXT("SKILL_IconBack1"), eUiRenderType::Render_Blend,
		CSkillIconBack::Create(m_pDevice, m_pContext))))
		assert(false);
	
	if (FAILED(m_pGameInstance->Add_Ui_Active(TEXT("Aim"), eUiRenderType::Render_Blend,
		CAim::Create(m_pDevice, m_pContext))))
		assert(false);

	if (FAILED(m_pGameInstance->Add_Ui_Active(TEXT("HpBar"), eUiRenderType::Render_Blend,
		CHpBar::Create(m_pDevice, m_pContext))))
		assert(false);

	m_pGameInstance->Get_ActiveBlendUI(TEXT("SKILL_IconBack1"))->Set_IndexNum(0);

	if (FAILED(m_pGameInstance->Add_Ui_Active(TEXT("SKILL_IconBack2"), eUiRenderType::Render_Blend,
		CSkillIconBack::Create(m_pDevice, m_pContext))))
		assert(false);

	m_pGameInstance->Get_ActiveBlendUI(TEXT("SKILL_IconBack2"))->Set_IndexNum(1);

	if (FAILED(m_pGameInstance->Add_Ui_Active(TEXT("SKILL_IconBack3"), eUiRenderType::Render_Blend,
		CSkillIconBack::Create(m_pDevice, m_pContext))))
		assert(false);

	m_pGameInstance->Get_ActiveBlendUI(TEXT("SKILL_IconBack3"))->Set_IndexNum(2);

	if (FAILED(m_pGameInstance->Add_Ui_Active(TEXT("SKILL_Icon1"), eUiRenderType::Render_Blend,
		CSkillIcon::Create(m_pDevice, m_pContext))))
		assert(false);

	m_pGameInstance->Get_ActiveBlendUI(TEXT("SKILL_Icon1"))->Set_IndexNum(0);

	if (FAILED(m_pGameInstance->Add_Ui_Active(TEXT("SKILL_Icon2"), eUiRenderType::Render_Blend,
		CSkillIcon::Create(m_pDevice, m_pContext))))
		assert(false);

	m_pGameInstance->Get_ActiveBlendUI(TEXT("SKILL_Icon2"))->Set_IndexNum(1);

	if (FAILED(m_pGameInstance->Add_Ui_Active(TEXT("SKILL_Icon3"), eUiRenderType::Render_Blend,
		CSkillIcon::Create(m_pDevice, m_pContext))))
		assert(false);

	m_pGameInstance->Get_ActiveBlendUI(TEXT("SKILL_Icon3"))->Set_IndexNum(2);

	return S_OK;
}

HRESULT CLoader::Load_Object_Textures()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Grappling_Point_Dot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GhostRunner/Texture/Grapling_hook/Grappling_point_dot.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Grappling_Point_Frame"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GhostRunner/Texture/Grapling_hook/Grappling_point_frame.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Grappling_Point_Outer_Ring"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GhostRunner/Texture/Grapling_hook/Grappling_point_outer_ring.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Aim"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GhostRunner/Texture/Aim/crosshair.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Spark"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GhostRunner/Texture/Spark/Spark.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Blood"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GhostRunner/Texture/BloodBody/blood%d.png"), 36))))
		return E_FAIL;


	return S_OK;
}

HRESULT CLoader::Load_Model()
{
	_matrix		LocalTransformMatrix = XMMatrixIdentity();

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_PistolGunner"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/GhostRunner/Model/AnimModel/PistolGunner/PistolGunner.fbx", LocalTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Uzi"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/GhostRunner/Model/AnimModel/Uzi/Uzi.fbx", LocalTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Waver"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/GhostRunner/Model/AnimModel/Waver/Waver.fbx", LocalTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Samurai"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/GhostRunner/Model/AnimModel/Samurai/Samurai.fbx", LocalTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Boss"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/GhostRunner/Model/AnimModel/Adrian/Adrian.fbx", LocalTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_PistolGunnerWeapon"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/GhostRunner/Model/NonAnimModel/PistolGunnerWeapon/PistolGunnerWeapon.fbx", LocalTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_GhostRunner_Arm"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/GhostRunner/Model/AnimModel/Arm/Arm.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_GhostRunner_Katana"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/GhostRunner/Model/NonAnimModel/Katana/Katana.fbx", LocalTransformMatrix))))
		return E_FAIL;*/

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_GhostRunner_Katana"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::ANIM, "../Bin/Resources/GhostRunner/Model/NonAnimModel/Katana/Katana.fbx", LocalTransformMatrix))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_KillRun_1_Map"),
	//	CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/GhostRunner/Model/NonAnimModel/Map/Killrun01.fbx", LocalTransformMatrix))))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Last_Map"),
	//	CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/GhostRunner/Model/NonAnimModel/Map/Lastmap.fbx", LocalTransformMatrix))))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_KillRun_2_Map"),
	//	CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/GhostRunner/Model/NonAnimModel/Map/Killrun02.fbx", LocalTransformMatrix))))
	//	return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Bullet"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/GhostRunner/Model/NonAnimModel/Bullet/Bullet.fbx", LocalTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Beam"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/GhostRunner/Model/NonAnimModel/Beam/Beam.fbx", LocalTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_ShockWave"),
		CModel::Create(m_pDevice, m_pContext, MODEL_TYPE::NONANIM, "../Bin/Resources/GhostRunner/Model/NonAnimModel/ShockWave/ShockWave.fbx", LocalTransformMatrix))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_For_Logo_Level()
{	
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));
	/* For.Prototype_Component_Texture_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default%d.png"), 1))))
		return E_FAIL;

	Load_Object_Textures();

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩 중 입니다."));


	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩 중 입니다."));
	
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Bgm/Stage1.ogg", TEXT("Sound_BGM"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Bgm/Stage2.ogg", TEXT("Sound_BGM1"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Bgm/LeCastleVania.mp3", TEXT("Sound_BGM2"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Bgm/Stage3.ogg", TEXT("Sound_BGM3"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/SFX_VR_Key_Explode_01.ogg", TEXT("Explode"));

	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Step/FOL_step_metal_01.ogg", TEXT("Sound_Player_Walk_01"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Step/FOL_step_metal_02.ogg", TEXT("Sound_Player_Walk_02"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Step/FOL_step_metal_03.ogg", TEXT("Sound_Player_Walk_03"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Step/FOL_step_metal_04.ogg", TEXT("Sound_Player_Walk_04"));

	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Jump/FOL_jump_01.ogg", TEXT("Sound_Player_Jump_01"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Jump/FOL_jump_01.ogg", TEXT("Sound_Player_Jump_02"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Jump/FOL_jump_01.ogg", TEXT("Sound_Player_Jump_03"));

	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Land/FOL_land_01.ogg", TEXT("Sound_Player_Land_01"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Land/FOL_land_02.ogg", TEXT("Sound_Player_Land_02"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Land/FOL_land_03.ogg", TEXT("Sound_Player_Land_03"));

	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Dash/FOL_dash_air_01.ogg", TEXT("Sound_Player_Dash_01"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Dash/FOL_dash_air_02.ogg", TEXT("Sound_Player_Dash_02"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Dash/FOL_dash_air_charge_01.ogg", TEXT("Sound_Player_Dash_Charge_01"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Dash/FOL_dash_air_charge_02.ogg", TEXT("Sound_Player_Dash_Charge_02"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Dash/FOL_dash_air_charge_loop.ogg", TEXT("Sound_Player_Dash_Charge_loop"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Dash/FOL_slide.ogg", TEXT("Sound_Player_Slide"));


	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Katana/FOL_sword_whoosh_01.ogg", TEXT("Sound_Player_Katana_01"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Katana/FOL_sword_whoosh_02.ogg", TEXT("Sound_Player_Katana_02"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Katana/FOL_sword_whoosh_03.ogg", TEXT("Sound_Player_Katana_03"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Katana/FOL_sword_whoosh_04.ogg", TEXT("Sound_Player_Katana_04"));

	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Katana/Attack/SFX_sword_hit_01.ogg", TEXT("Sound_Player_Katana_Attack_01"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Katana/Attack/SFX_sword_hit_02.ogg", TEXT("Sound_Player_Katana_Attack_02"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Katana/Attack/SFX_sword_hit_03.ogg", TEXT("Sound_Player_Katana_Attack_03"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Katana/Attack/SFX_sword_hit_04.ogg", TEXT("Sound_Player_Katana_Attack_04"));

	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Die/DX_player_death_01.ogg", TEXT("Sound_Player_Die_01"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Die/DX_player_death_02.ogg", TEXT("Sound_Player_Die_02"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Die/DX_player_death_03.ogg", TEXT("Sound_Player_Die_03"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Die/DX_player_death_04.ogg", TEXT("Sound_Player_Die_04"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Die/DX_player_death_05.ogg", TEXT("Sound_Player_Die_05"));

	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Wire/FOL_hook_shot_01.ogg", TEXT("Sound_Player_Wire_01"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Wire/FOL_hook_shot_02.ogg", TEXT("Sound_Player_Wire_02"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Wire/FOL_hook_shot_03.ogg", TEXT("Sound_Player_Wire_03"));

	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Katana/Parry/Sekiro1.ogg", TEXT("Sound_Player_Katana_Parry_01"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Katana/Parry/Sekiro2.ogg", TEXT("Sound_Player_Katana_Parry_02"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Katana/Parry/Sekiro3.ogg", TEXT("Sound_Player_Katana_Parry_03"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Katana/Parry/Sekiro4.ogg", TEXT("Sound_Player_Katana_Parry_04"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Katana/Parry/Sekiro5.ogg", TEXT("Sound_Player_Katana_Parry_05"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Katana/Parry/Sekiro6.ogg", TEXT("Sound_Player_Katana_Parry_06"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Katana/Parry/Sekiro7.ogg", TEXT("Sound_Player_Katana_Parry_07"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Katana/Parry/Sekiro8.ogg", TEXT("Sound_Player_Katana_Parry_08"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Katana/Parry/Sekiro9.ogg", TEXT("Sound_Player_Katana_Parry_09"));

	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Skill/MindHack/FOL_mind_hack_01.ogg", TEXT("Sound_Player_Skill_MindHack"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Skill/Nami/FOL_nami_shot_hi_01.ogg", TEXT("Sound_Player_Skill_Nami_01"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Skill/Nami/FOL_nami_shot_hi_02.ogg", TEXT("Sound_Player_Skill_Nami_02"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Skill/SFX_energy_bar_full.ogg", TEXT("Sound_Player_Skill_Full"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Player/Skill/SFX_energy_bar_switch.ogg", TEXT("Sound_Player_Skill_Switch"));

	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/PistolGunner/Alert/DX_pistol_alert_01.ogg", TEXT("Sound_PistolGunner_Alert_01"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/PistolGunner/Alert/DX_pistol_alert_02.ogg", TEXT("Sound_PistolGunner_Alert_02"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/PistolGunner/Alert/DX_pistol_alert_03.ogg", TEXT("Sound_PistolGunner_Alert_03"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/PistolGunner/Alert/DX_pistol_alert_04.ogg", TEXT("Sound_PistolGunner_Alert_04"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/PistolGunner/Alert/DX_pistol_alert_05.ogg", TEXT("Sound_PistolGunner_Alert_05"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/PistolGunner/Alert/DX_pistol_alert_06.ogg", TEXT("Sound_PistolGunner_Alert_06"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/PistolGunner/Alert/DX_pistol_alert_07.ogg", TEXT("Sound_PistolGunner_Alert_07"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/PistolGunner/Alert/DX_pistol_alert_08.ogg", TEXT("Sound_PistolGunner_Alert_08"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/PistolGunner/Alert/DX_pistol_alert_09.ogg", TEXT("Sound_PistolGunner_Alert_09"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/PistolGunner/Alert/DX_pistol_alert_10.ogg", TEXT("Sound_PistolGunner_Alert_10"));

	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/PistolGunner/Death/DX_pistol_death_01.ogg", TEXT("Sound_PistolGunner_Death_01"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/PistolGunner/Death/DX_pistol_death_02.ogg", TEXT("Sound_PistolGunner_Death_02"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/PistolGunner/Death/DX_pistol_death_03.ogg", TEXT("Sound_PistolGunner_Death_03"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/PistolGunner/Death/DX_pistol_death_04.ogg", TEXT("Sound_PistolGunner_Death_04"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/PistolGunner/Death/DX_pistol_death_05.ogg", TEXT("Sound_PistolGunner_Death_05"));

	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/PistolGunner/SFX_Pistol_Shot_high_1.ogg", TEXT("Sound_PistolGunner_Shot_01"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/PistolGunner/SFX_Pistol_Shot_high_2.ogg", TEXT("Sound_PistolGunner_Shot_02"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/PistolGunner/SFX_Pistol_Shot_high_3.ogg", TEXT("Sound_PistolGunner_Shot_03"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/PistolGunner/SFX_Pistol_Shot_high_4.ogg", TEXT("Sound_PistolGunner_Shot_04"));

	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Uzi/Alert/DX_uzi_alert_01.ogg", TEXT("Sound_Uzi_Alert_01"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Uzi/Alert/DX_uzi_alert_01.ogg", TEXT("Sound_Uzi_Alert_02"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Uzi/Alert/DX_uzi_alert_03.ogg", TEXT("Sound_Uzi_Alert_03"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Uzi/Alert/DX_uzi_alert_04.ogg", TEXT("Sound_Uzi_Alert_04"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Uzi/Alert/DX_uzi_alert_05.ogg", TEXT("Sound_Uzi_Alert_05"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Uzi/Alert/DX_uzi_alert_06.ogg", TEXT("Sound_Uzi_Alert_06"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Uzi/Alert/DX_uzi_alert_07.ogg", TEXT("Sound_Uzi_Alert_07"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Uzi/Alert/DX_uzi_alert_08.ogg", TEXT("Sound_Uzi_Alert_08"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Uzi/Alert/DX_uzi_alert_09.ogg", TEXT("Sound_Uzi_Alert_09"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Uzi/Alert/DX_uzi_alert_10.ogg", TEXT("Sound_Uzi_Alert_10"));

	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Uzi/Death/DX_uzi_death_01.ogg", TEXT("Sound_Uzi_Death_01"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Uzi/Death/DX_uzi_death_02.ogg", TEXT("Sound_Uzi_Death_02"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Uzi/Death/DX_uzi_death_03.ogg", TEXT("Sound_Uzi_Death_03"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Uzi/Death/DX_uzi_death_04.ogg", TEXT("Sound_Uzi_Death_04"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Uzi/Death/DX_uzi_death_05.ogg", TEXT("Sound_Uzi_Death_05"));

	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Uzi/Reload/DX_uzi_burst_aftr_01.ogg", TEXT("Sound_Uzi_Reload_01"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Uzi/Reload/DX_uzi_burst_aftr_02.ogg", TEXT("Sound_Uzi_Reload_02"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Uzi/Reload/DX_uzi_burst_aftr_03.ogg", TEXT("Sound_Uzi_Reload_03"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Uzi/Reload/DX_uzi_burst_aftr_04.ogg", TEXT("Sound_Uzi_Reload_04"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Uzi/Reload/DX_uzi_burst_aftr_05.ogg", TEXT("Sound_Uzi_Reload_05"));

	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Uzi/SFX_SMG_Fire_High_01.ogg", TEXT("Sound_Uzi_Shot_01"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Uzi/SFX_SMG_Fire_High_02.ogg", TEXT("Sound_Uzi_Shot_02"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Uzi/SFX_SMG_Fire_High_03.ogg", TEXT("Sound_Uzi_Shot_03"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Uzi/SFX_SMG_Fire_High_04.ogg", TEXT("Sound_Uzi_Shot_04"));

	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Waver/Shot/SFX_waver_charge_first.ogg", TEXT("Sound_Waver_Alert"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Waver/Shot/SFX_waver_shot_01.ogg", TEXT("Sound_Waver_Shot_01"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Waver/Shot/SFX_waver_shot_02.ogg", TEXT("Sound_Waver_Shot_02"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Waver/Shot/SFX_waver_shot_03.ogg", TEXT("Sound_Waver_Shot_03"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Waver/Shot/SFX_waver_shot_04.ogg", TEXT("Sound_Waver_Shot_04"));

	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Waver/Death/SFX_machine_destroy_01.ogg", TEXT("Sound_Waver_Death_01"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Waver/Death/SFX_machine_destroy_02.ogg", TEXT("Sound_Waver_Death_02"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Waver/Death/SFX_machine_destroy_03.ogg", TEXT("Sound_Waver_Death_03"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Waver/Death/SFX_machine_destroy_04.ogg", TEXT("Sound_Waver_Death_04"));

	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Samurai/Alert/DX_elite_alert_01.ogg", TEXT("Sound_Samurai_Alert_01"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Samurai/Alert/DX_elite_alert_02.ogg", TEXT("Sound_Samurai_Alert_02"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Samurai/Alert/DX_elite_alert_03.ogg", TEXT("Sound_Samurai_Alert_03"));

	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Samurai/Block/DX_elite_blocked_01.ogg", TEXT("Sound_Samurai_Block_01"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Samurai/Block/DX_elite_blocked_02.ogg", TEXT("Sound_Samurai_Block_02"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Samurai/Block/DX_elite_blocked_03.ogg", TEXT("Sound_Samurai_Block_03"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Samurai/Block/DX_elite_blocked_04.ogg", TEXT("Sound_Samurai_Block_03"));

	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Samurai/Charge/DX_elite_charge_01.ogg", TEXT("Sound_Samurai_Charge_01"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Samurai/Charge/DX_elite_charge_02.ogg", TEXT("Sound_Samurai_Charge_02"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Samurai/Charge/DX_elite_charge_03.ogg", TEXT("Sound_Samurai_Charge_03"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Samurai/Charge/DX_elite_charge_04.ogg", TEXT("Sound_Samurai_Charge_04"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Samurai/Charge/DX_elite_charge_05.ogg", TEXT("Sound_Samurai_Charge_05"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Samurai/Charge/DX_elite_charge_06.ogg", TEXT("Sound_Samurai_Charge_06"));

	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Samurai/Death/DX_elite_death_01.ogg", TEXT("Sound_Samurai_Death_01"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Samurai/Death/DX_elite_Death_02.ogg", TEXT("Sound_Samurai_Death_02"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Samurai/Death/DX_elite_Death_03.ogg", TEXT("Sound_Samurai_Death_03"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Samurai/Death/DX_elite_Death_04.ogg", TEXT("Sound_Samurai_Death_04"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Samurai/Death/DX_elite_Death_05.ogg", TEXT("Sound_Samurai_Death_05"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Samurai/Death/DX_elite_Death_06.ogg", TEXT("Sound_Samurai_Death_06"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Samurai/Death/DX_elite_Death_07.ogg", TEXT("Sound_Samurai_Death_07"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Samurai/Death/DX_elite_Death_08.ogg", TEXT("Sound_Samurai_Death_08"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Samurai/Death/DX_elite_Death_09.ogg", TEXT("Sound_Samurai_Death_09"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Samurai/Death/DX_elite_Death_10.ogg", TEXT("Sound_Samurai_Death_10"));

	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Samurai/Kill/DX_elite_kill_01.ogg", TEXT("Sound_Samurai_Kill_01"));
	m_pGameInstance->Create_Sound("../Bin/Resources/Sounds/Fx/Samurai/Kill/DX_elite_kill_02.ogg", TEXT("Sound_Samurai_Kill_02"));


	lstrcpy(m_szLoadingText, TEXT("셰이더을(를) 로딩 중 입니다."));

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UiGauge"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_UiGauge.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shaderfiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;


	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxBullet"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxBullet.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxRectInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxRectInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectInstance.hlsl"), VTXRECTINSTANCE::Elements, VTXRECTINSTANCE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPointInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTXPOINTINSTANCE::Elements, VTXPOINTINSTANCE::iNumElements))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩 중 입니다."));

	/* For.Prototype_GameObject_Camera_Free */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_TPV"),
		CCamera_TPV::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Tool"),
		CCamera_Tool::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PistolGunner"),
		CPistolGunner::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Uzi"),
		CUzi::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Waver"),
		CWaver::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Samurai"),
		CSamurai::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss"),
		CBoss::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerArm"),
		CPlayerArm::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_PistolGunnerWeapon"),
		CPistol::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Katana"),
		CKatana::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bullet"),
		CBullet::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Beam"),
		CBeam::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShockWave"),
		CShockWave::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Spark"),
		CSpark::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Blood"),
		CBlood::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Grappling_Point"),
		CGraplingPointObject::Create(m_pDevice, m_pContext))))
			return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_Trail_Effect"),
		CTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_Wire_Effect"),
		CWire::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("맵을(를) 로딩 중 입니다."));

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_Map_KillRun_1"),
		CKillRunMap::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_Map_KillRun_2"),
		CLabMap::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_Map_Last"),
		CLastMap::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	Load_Ui();
	Load_Model();
	/* for.Prototype_GameObject_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	CONTAINER->BulletPooling();
	CONTAINER->BeamPooling();
	CONTAINER->ShockWavePooling();

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;
	
	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Level()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩 중 입니다."));

	CVIBuffer_Instance::INSTANCE_DESC		InstanceDesc{};
	InstanceDesc.iNumInstance = 50;
	InstanceDesc.vScale = _float2(0.2f, 0.2f);
	InstanceDesc.vCenter = _float3(0.f,0.f,0.f);
	//InstanceDesc.vCenter = _float3(0.f,0.f,0.f);
	InstanceDesc.vRange = _float3(0.3f, 0.3f, 0.3f);
	InstanceDesc.vSpeed = _float2(9.f, 10.f);
	InstanceDesc.vPivot = _float3(0.f,0.f,0.f);
	InstanceDesc.vLifeTime = _float2(1.f, 1.f);
	InstanceDesc.isLoop = false;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Particle_Spark"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, &InstanceDesc))))
		return E_FAIL;



	/* For.Prototype_Component_VIBuffer_Terrain */
	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;*/

	///* For.Prototype_Component_VIBuffer_Cube */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
	//	CVIBuffer_Cube::Create(m_pGraphic_Device))))
	//	return E_FAIL;
	//

	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩 중 입니다."));
	

	lstrcpy(m_szLoadingText, TEXT("셰이더을(를) 로딩 중 입니다."));

	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩 중 입니다."));

	/* For.Prototype_GameObject_Terrain */
	/*if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;*/

	/* For.Prototype_GameObject_PistolGunner */

	///* For.Prototype_GameObject_Camera */
	//if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera"),
	//	CCamera::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	///* For.Prototype_GameObject_Player */
	//if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
	//	CPlayer::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	///* For.Prototype_GameObject_Monster */
	//if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"),
	//	CMonster::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	///* For.Prototype_GameObject_Sky */
	//if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
	//	CSky::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	///* For.Prototype_GameObject_Effect */
	//if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect"),
	//	CEffect::Create(m_pGraphic_Device))))
	//	return E_FAIL;
	

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Lab_Level()
{
	lstrcpy(m_szLoadingText, TEXT("연구실 레벨 로딩이 완료되었습니다."));
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Tool_Level()
{
	lstrcpy(m_szLoadingText, TEXT("Tool Loading"));

	lstrcpy(m_szLoadingText, TEXT("Load Complete"));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Boss_Level()
{
	lstrcpy(m_szLoadingText, TEXT("마지막 레벨 로딩이 완료되었습니다."));
	m_isFinished = true;

	return S_OK;
}

CLoader * CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLoader*	pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed to Created : CLoader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_CriticalSection);

	DeleteObject(m_hThread);
	CloseHandle(m_hThread);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
