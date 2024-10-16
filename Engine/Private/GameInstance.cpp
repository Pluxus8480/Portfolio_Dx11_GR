#include "..\Public\GameInstance.h"

#include "Graphic_Device.h"
#include "Object_Manager.h"
#include "Level_Manager.h"
#include "Timer_Manager.h"
#include "Math_Manager.h"
#include "PhysX_EventCallBack.h"
#include "ResourceContainer.h"
#include "Model_Converter_Manager.h"
#include "Light_Manager.h"
#include "Target_Manager.h"
#include "Sound_Manager.h"
#include "Font_Manager.h"


IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{

}

HRESULT CGameInstance::Initialize_Engine(_uint iNumLevels, const ENGINE_DESC& EngineDesc, _Out_ ID3D11Device** ppDevice, _Out_ ID3D11DeviceContext** ppContext)
{
	/* 엔진을 사용하기위해 필요한 기타 초기화 작업들을 거친다. */

	/* 그래픽 디바이스 초기화. */
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	/* 인풋 디바이스 초기화. */

	/* 사운드 디바이스 초기화. */

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	/* 레벨 매니져를 준비해놓는다. */
	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	/* 키 매니져를 준비해놓는다. */
	m_pKey_Manager = CKey_Manager::Create();
	if (nullptr == m_pKey_Manager)
			return E_FAIL;

	/* MathManager Already*/
	m_pMath_Manager = CMath_Manager::Create();
	if (nullptr == m_pMath_Manager)
			return E_FAIL;

	/* Object Manager Already */
	m_pObject_Manager = CObject_Manager::Create(iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	/* Component Manager Already */
	m_pComponent_Manager = CComponent_Manager::Create(iNumLevels);
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	m_pTarget_Manager = CTarget_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	/*Camera Manager Already*/
	m_pCamera_Manager = CCamera_Manager::Create();
	if(nullptr == m_pCamera_Manager)
		return E_FAIL;

	/*Ui Manager Already*/
	m_pUi_Manager = CUi_Manager::Create(EngineDesc ,*ppDevice, *ppContext);
	if(nullptr == m_pUi_Manager)
		return E_FAIL;

	/*Simulaion Callback*/
	m_pPhysX_Callback = CPhysX_EventCallBack::Create();
	if(nullptr == m_pPhysX_Callback)
		return E_FAIL;

	/*PhysX Manager Already*/
	m_pPhysX_Manager = CPhysX_Manager::Create();
	if(nullptr == m_pPhysX_Manager)
		return E_FAIL;

	/*Model Converter*/
	m_pModel_Converter_Manager = CModel_Converter_Manager::Create();
	if(nullptr == m_pModel_Converter_Manager)
		return E_FAIL;

	/*Resource Container*/
	m_pResource_Container = CResource_Container::Create(*ppDevice, *ppContext);
	if(nullptr == m_pResource_Container)
		return E_FAIL;

	/*Light Manager*/
	m_pLight_Manager = CLight_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	/*Font Manager*/
	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	/*Collision Manager*/
	m_pCollision_Manager = CCollision_Manager::Create();
	if (nullptr == m_pCollision_Manager)
		return E_FAIL;

	/*Sound Manager*/
	m_pSound_Manager = CSound_Manager::Create();
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Tick_Engine(_float fTimeDelta)
{
	if (nullptr == m_pLevel_Manager || 
		nullptr == m_pObject_Manager)
		return;

	m_pKey_Manager->Update();

	Imgui_TickEngine();

	m_pCollision_Manager->CollisionDeleteCheck();
	m_pObject_Manager->DeleteCheck();

	m_pObject_Manager->PriorityTick(fTimeDelta);

	/* 카메라가 갱신해준 뷰, 투영행렬을 이용하자. */
	
	m_pUi_Manager->PriorityTick(fTimeDelta);
	m_pUi_Manager->Tick(fTimeDelta);
	m_pUi_Manager->LateTick(fTimeDelta);
	
	m_pLight_Manager->TickEngine(fTimeDelta);

	m_pCamera_Manager->Update();

	m_pObject_Manager->Tick(fTimeDelta);
	m_pObject_Manager->LateTick(fTimeDelta);

	m_pLevel_Manager->Tick(fTimeDelta);
	
	Collision_Check();
	Simulate_CurrentScene(fTimeDelta);

}

HRESULT CGameInstance::Draw()
{
	if (nullptr == m_pLevel_Manager || 
		nullptr == m_pRenderer)
		return E_FAIL;


	m_pRenderer->Draw();
	Ui_Render();

	return m_pLevel_Manager->Render();
}
void CGameInstance::Clear(_uint iLevelIndex)
{
	m_pRenderer->Clear();
	m_pObject_Manager->Prepare_Clear(iLevelIndex);
	m_pCollision_Manager->CollisionDeleteCheck();
	m_pObject_Manager->Clear(iLevelIndex);	
	m_pComponent_Manager->Clear(iLevelIndex);
}

HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);	
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	return m_pGraphic_Device->Present();
}

#pragma region LEVEL_MANAGER
HRESULT CGameInstance::Change_Level(CLevel * pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Change_Level(pNewLevel);
}

_uint CGameInstance::Get_CurrentLevelIndex() const
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Get_LevelID();
}
#pragma endregion

#pragma region TIMER_MANAGER
_float CGameInstance::Get_TimeDelta(const wstring & strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.f;

	return m_pTimer_Manager->Get_TimeDelta(strTimerTag);
}
HRESULT CGameInstance::Add_Timer(const wstring & strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timer(strTimerTag);
}
void CGameInstance::Compute_TimeDelta(const wstring & strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return ;

	return m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}
void CGameInstance::Set_TimeMultiplier(_float _fTimeMultiplier)
{
	if (nullptr == m_pTimer_Manager)
		return;

	m_pTimer_Manager->Set_TimeMultiplier(_fTimeMultiplier);
}
_float CGameInstance::Get_TimeMultiplier()
{
	if (nullptr == m_pTimer_Manager)
		assert(false);

	return m_pTimer_Manager->Get_TimeMultiplier();
}
#pragma endregion

#pragma region OBJECT_MANAGER
HRESULT CGameInstance::Add_Prototype(const wstring & strPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(strPrototypeTag, pPrototype);
	
}

CGameObject* CGameInstance::Find_GameObject(_uint iLevelIndex, const wstring& strLayerTag, _uint iIndex)
{

	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Find_GameObject(iLevelIndex, strLayerTag, iIndex);
}

CLayer* CGameInstance::Find_Layer(_uint iLevelIndex, const wstring& strLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Find_Layer(iLevelIndex, strLayerTag);
}

HRESULT CGameInstance::Add_Clone(_uint iLevelIndex, const wstring & strLayerTag, const wstring & strPrototypeTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL; 

	return m_pObject_Manager->Add_Clone(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
}

CGameObject* CGameInstance::Clone_GameObject(const wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Clone_GameObject(strPrototypeTag, pArg);
}

CGameObject* CGameInstance::Add_Clone_Return(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg)
{
	if(nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Add_Clone_Return(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
}

CComponent * CGameInstance::Find_Component(_uint iLevelIndex, const wstring & strLayerTag, const wstring & strComponentTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Find_Component(iLevelIndex, strLayerTag, strComponentTag, iIndex);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const wstring & strPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);	
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const wstring & strPrototypeTag, void * pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
}

#pragma endregion

#pragma region RENDERER
HRESULT CGameInstance::Add_RenderObjects(CRenderer::RENDERGROUP eRenderGroup, CGameObject * pRenderObject)
{
	if (nullptr == m_pRenderer)
		return E_FAIL;

	return m_pRenderer->Add_RenderObjects(eRenderGroup, pRenderObject);	
}
void CGameInstance::Set_DebugMode(_bool bDebug)
{
	if (nullptr == m_pRenderer)
		return;

	m_pRenderer->Set_DebugMode(bDebug);
}
_bool CGameInstance::Get_DebugMode()
{
	if (nullptr == m_pRenderer)
		assert(false);

	return m_pRenderer->Get_DebugMode();
}
#pragma endregion

#pragma region MATH_MANAGER

_float2 CGameInstance::Lissajous_Curve(IN _float _fTimeDelta, IN OUT _float& _fLissajousTime, IN _float _fWitth, IN _float _fHeight, IN _float _fLagrangianX, IN _float _fLagrangianY, IN _float _fPhaseDelta, IN _float _fLissajousSpeed)
{
	if (nullptr == m_pMath_Manager)
		assert(false);

	return m_pMath_Manager->Lissajous_Curve(_fTimeDelta, _fLissajousTime, _fWitth, _fHeight, _fLagrangianX, _fLagrangianY, _fPhaseDelta, _fLissajousSpeed);
}

_float3 CGameInstance::Vector3x3_BezierCurve(IN _float3 _vStart, IN _float3 _vControl, IN _float3 _vEnd, IN OUT _float& _fTimeControl, IN _float _fTimeDelta, IN _float _fSpeed)
{
	if (nullptr == m_pMath_Manager)
		assert(false);

	return m_pMath_Manager->Vector3x3_BezierCurve(_vStart, _vControl, _vEnd, _fTimeControl, _fTimeDelta, _fSpeed);
}

_float CGameInstance::Random_Float(_float _fMin, _float _fMax)
{
	if (nullptr == m_pMath_Manager)
		assert(false);

	return m_pMath_Manager->Random_Float(_fMin, _fMax);
}

_float2 CGameInstance::Random_Float2(_float2 _fMin, _float2 _fMax)
{
	if (nullptr == m_pMath_Manager)
		assert(false);

	return m_pMath_Manager->Random_Float2(_fMin, _fMax);
}	

_float3 CGameInstance::Random_Float3(_float3 _fMin, _float3 _fMax)
{
	if (nullptr == m_pMath_Manager)
		assert(false);

	return m_pMath_Manager->Random_Float3(_fMin, _fMax);
}

_int CGameInstance::Random_Int(_int _iMin, _int _iMax)
{
	if (nullptr == m_pMath_Manager)
		assert(false);

	return m_pMath_Manager->Random_Int(_iMin, _iMax);
}

_vector CGameInstance::XMQuaternionRotationBetweenVectors(_vector start, _vector end)
{
	if (nullptr == m_pMath_Manager)
		assert(false);

	return m_pMath_Manager->XMQuaternionRotationBetweenVectors(start, end);
}

_int CGameInstance::Random_Binary()
{
	if (nullptr == m_pMath_Manager)
		assert(false);

	return m_pMath_Manager->Random_Binary();
}

_bool CGameInstance::Random_Bool()
{
	if (nullptr == m_pMath_Manager)
		assert(false);

	return m_pMath_Manager->Random_Bool();
}

PxVec3 CGameInstance::Add_PxVec3toXMVECTOR(PxVec3 _Pxvec3, _vector _Xmvec)
{
	if (nullptr == m_pMath_Manager)
		assert(false);

	return m_pMath_Manager->Add_PxVec3toXMVECTOR(_Pxvec3, _Xmvec);
}

#pragma endregion

#pragma region CAMERA_MANAGER
void CGameInstance::Set_Transform(CCamera_Manager::TRANSFORMATIONSTATE eState, _fmatrix TransformationMatrix)
{
	if (nullptr == m_pCamera_Manager)
		return;

	m_pCamera_Manager->Set_Transform(eState, TransformationMatrix);
}

_matrix CGameInstance::Get_Transform_Matrix(CCamera_Manager::TRANSFORMATIONSTATE eState) const
{
	if (nullptr == m_pCamera_Manager)
		return XMMatrixIdentity();

	return m_pCamera_Manager->Get_Transform_Matrix(eState);
}

const _float4x4* CGameInstance::Get_Transform_Float4x4(CCamera_Manager::TRANSFORMATIONSTATE eState) const
{
	if (nullptr == m_pCamera_Manager)
		return nullptr;

	return m_pCamera_Manager->Get_Transform_Float4x4(eState);
}

_matrix CGameInstance::Get_Transform_Matrix_Inverse(CCamera_Manager::TRANSFORMATIONSTATE eState) const
{
	if (nullptr == m_pCamera_Manager)
		return XMMatrixIdentity();

	return m_pCamera_Manager->Get_Transform_Matrix_Inverse(eState);
}

const _float4x4* CGameInstance::Get_Transform_Float4x4_Inverse(CCamera_Manager::TRANSFORMATIONSTATE eState) const
{
	if (nullptr == m_pCamera_Manager)
		return nullptr;

	return m_pCamera_Manager->Get_Transform_Float4x4_Inverse(eState);
}

_vector CGameInstance::Get_CamPosition_Vector() const
{
	if (nullptr == m_pCamera_Manager)
		return XMVectorZero();

	return m_pCamera_Manager->Get_CamPosition_Vector();
}

const _float4* CGameInstance::Get_CamPosition_Float4() const
{
	if (nullptr == m_pCamera_Manager)
		return nullptr;

	return m_pCamera_Manager->Get_CamPosition_Float4();
}

_matrix CGameInstance::Get_BillBoard_Matrix() const
{
	if (nullptr == m_pCamera_Manager)
		return XMMatrixIdentity();

	return m_pCamera_Manager->Get_BillBoardMatrix();
}

 _float4x4* CGameInstance::Get_BillBoard_Matrix4x4()
{
	if (nullptr == m_pCamera_Manager)
		return nullptr;

	return m_pCamera_Manager->Get_BillBoardMatrix4x4();
}

HRESULT CGameInstance::Create_Camera(CCoreCamera* _pCamera, const wstring& _wstrCameraKey)
{
	if(nullptr == m_pCamera_Manager)
		return E_FAIL;

	return m_pCamera_Manager->Create_Camera(_pCamera, _wstrCameraKey);
}

HRESULT CGameInstance::Change_Camera(const wstring& _wstrCameraKey)
{
	if (nullptr == m_pCamera_Manager)
		return E_FAIL;

	return m_pCamera_Manager->Change_Camera(_wstrCameraKey);
}

void CGameInstance::Set_Target(CGameObject* _pTarget)
{
	if (nullptr == m_pCamera_Manager)
		return;

	m_pCamera_Manager->Set_Target(_pTarget);
}

CCoreCamera* CGameInstance::Get_CurCamera()
{
	if (nullptr == m_pCamera_Manager)
		return nullptr;

	return m_pCamera_Manager->Get_CurCamera();
}

CTransform* CGameInstance::Get_CurCamera_Transform()
{
	if (nullptr == m_pCamera_Manager)
		return nullptr;
	
	return m_pCamera_Manager->Get_CurCamera_Transform();
}

void CGameInstance::Set_UseGlitchLens(_bool _bUseGlitchLens)
{
	if (nullptr == m_pCamera_Manager)
		return;

	m_pCamera_Manager->Set_UseGlitchLens(_bUseGlitchLens);
}

_bool CGameInstance::Get_UseGlitchLens()
{
	if (nullptr == m_pCamera_Manager)
		assert(false);

	return m_pCamera_Manager->Get_UseGlitchLens();
}

void CGameInstance::Set_UseRedBlockGlitch(_bool _bUseRedBlockGlitch)
{
	if (nullptr == m_pCamera_Manager)
		return;

	m_pCamera_Manager->Set_UseRedBlockGlitch(_bUseRedBlockGlitch);
}

_bool CGameInstance::Get_UseRedBlockGlitch()
{
	if (nullptr == m_pCamera_Manager)
		assert(false);

	return m_pCamera_Manager->Get_UseRedBlockGlitch();
}

void CGameInstance::Set_GlitchPower(_float _fPower)
{
	if (nullptr == m_pCamera_Manager)
		return;

	m_pCamera_Manager->Set_GlitchPower(_fPower);
}

_float CGameInstance::Get_GlitchPower()
{
	if (nullptr == m_pCamera_Manager)
		assert(false);

	return m_pCamera_Manager->Get_GlitchPower();
}

void CGameInstance::Set_GlitchRate(_float _fRate)
{
	if (nullptr == m_pCamera_Manager)
		return;

	m_pCamera_Manager->Set_GlitchRate(_fRate);
}

_float CGameInstance::Get_GlitchRate()
{
	if (nullptr == m_pCamera_Manager)
		assert(false);

	return m_pCamera_Manager->Get_GlitchRate();
}

void CGameInstance::Set_GlitchSpeed(_float _fSpeed)
{
	if (nullptr == m_pCamera_Manager)
		return;

	m_pCamera_Manager->Set_GlitchSpeed(_fSpeed);
}

_float CGameInstance::Get_GlitchSpeed()
{
	if (nullptr == m_pCamera_Manager)
		assert(false);

	return m_pCamera_Manager->Get_GlitchSpeed();
}

void CGameInstance::Set_GlitchColorRate(_float _fColorRate)
{
	if (nullptr == m_pCamera_Manager)
		return;

	m_pCamera_Manager->Set_GlitchColorRate(_fColorRate);
}

_float CGameInstance::Get_GlitchColorRate()
{
	if (nullptr == m_pCamera_Manager)
		assert(false);

	return m_pCamera_Manager->Get_GlitchColorRate();
}

void CGameInstance::Set_RedBlockMixRate(_float _fMixRate)
{
	if (nullptr == m_pCamera_Manager)
		return;

	m_pCamera_Manager->Set_RedBlockMixRate(_fMixRate);
}

_float CGameInstance::Get_RedBlockMixRate()
{
	if (nullptr == m_pCamera_Manager)
		assert(false);

	return m_pCamera_Manager->Get_RedBlockMixRate();
}

void CGameInstance::Set_BlackScreenRate(_float _fRate)
{
	if (nullptr == m_pCamera_Manager)
		return;

	m_pCamera_Manager->Set_BlackScreenRate(_fRate);
}

_float CGameInstance::Get_BlackScreenRate()
{
	if (nullptr == m_pCamera_Manager)
		assert(false);

	return m_pCamera_Manager->Get_BlackScreenRate();
}

void CGameInstance::Set_BlackScreen(_bool _bUseBlackScreen)
{
	if (nullptr == m_pCamera_Manager)
		return;

	m_pCamera_Manager->Set_BlackScreen(_bUseBlackScreen);

}

_bool CGameInstance::Get_BlackScreen()
{
	if (nullptr == m_pCamera_Manager)
		assert(false);

	return m_pCamera_Manager->Get_BlackScreen();
}

void CGameInstance::Set_Fov(_float _fFov)
{
	if (nullptr == m_pCamera_Manager)
		return;

	m_pCamera_Manager->Set_Fov(_fFov);

}

_float CGameInstance::Get_Fov()
{
	if (nullptr == m_pCamera_Manager)
		assert(false);

	return m_pCamera_Manager->Get_Fov();
}

void CGameInstance::Set_BlurScreen(_bool _bUseBlurScreen)
{
	if (nullptr == m_pCamera_Manager)
		return;

	m_pCamera_Manager->Set_BlurScreen(_bUseBlurScreen);
}

_bool CGameInstance::Get_BlurScreen()
{
	if (nullptr == m_pCamera_Manager)
		assert(false);

	return m_pCamera_Manager->Get_BlurScreen();
}

void CGameInstance::Set_BlueScreen(_bool _bUseBlueScreen)
{
	if (nullptr == m_pCamera_Manager)
		return;

	m_pCamera_Manager->Set_BlueScreen(_bUseBlueScreen);
}

_bool CGameInstance::Get_BlueScreen()
{
	if (nullptr == m_pCamera_Manager)
		assert(false);

	return m_pCamera_Manager->Get_BlueScreen();
}

void CGameInstance::Set_RedScreen(_bool _bUseRedScreen)
{
	if (nullptr == m_pCamera_Manager)
		return;

	m_pCamera_Manager->Set_RedScreen(_bUseRedScreen);
}

_bool CGameInstance::Get_RedScreen()
{
	if (nullptr == m_pCamera_Manager)
		assert(false);

	return m_pCamera_Manager->Get_RedScreen();
}

void CGameInstance::Set_BlurPower(_float _fPower)
{
	if (nullptr == m_pCamera_Manager)
		return;

	m_pCamera_Manager->Set_BlurStrength(_fPower);
}

_float CGameInstance::Get_BlurPower()
{
	if (nullptr == m_pCamera_Manager)
		assert(false);

	return m_pCamera_Manager->Get_BlurStrength();
}

void CGameInstance::Set_BlueScreenRate(_float _fRate)
{
	if (nullptr == m_pCamera_Manager)
		return;

	m_pCamera_Manager->Set_BlueScreenRate(_fRate);
}

_float CGameInstance::Get_BlueScreenRate()
{
	if (nullptr == m_pCamera_Manager)
		assert(false);

	return m_pCamera_Manager->Get_BlueScreenRate();
}

void CGameInstance::Set_ShakePower(_float _fPower)
{
	if (nullptr == m_pCamera_Manager)
		return;

	m_pCamera_Manager->Set_ShakePower(_fPower);
}

_float CGameInstance::Get_ShakePower()
{
	if(nullptr == m_pCamera_Manager)
		assert(false);

	return m_pCamera_Manager->Get_ShakePower();
}

void CGameInstance::Set_ShakeTime(_float _fTime)
{
	if (nullptr == m_pCamera_Manager)
		return;

	m_pCamera_Manager->Set_ShakeTime(_fTime);
}

_float CGameInstance::Get_ShakeTime()
{
	if (nullptr == m_pCamera_Manager)
		assert(false);
	
	return m_pCamera_Manager->Get_ShakeTime();
}

#pragma endregion

#pragma region UI_MANAGER

HRESULT CGameInstance::Add_Ui_LifePrototype(const wstring& Ui_LifePrototypeTag, CUi* Ui_LifePrototype)
{
	if (nullptr == m_pUi_Manager)
		return E_FAIL;

	return m_pUi_Manager->Add_Ui_LifePrototype(Ui_LifePrototypeTag, Ui_LifePrototype);
}

HRESULT CGameInstance::Add_Ui_Active(const wstring& Ui_ActiveTag, eUiRenderType Ui_RenderType, CUi* Ui_Active)
{
	if (nullptr == m_pUi_Manager)
		return E_FAIL;

	return m_pUi_Manager->Add_Ui_Active(Ui_ActiveTag, Ui_RenderType, Ui_Active);
}

HRESULT CGameInstance::Add_Ui_LifeClone(const wstring& Ui_LifePrototypeTag, eUiRenderType UiRenderType, void* pArg)
{
	if (nullptr == m_pUi_Manager)
		return E_FAIL;

	return m_pUi_Manager->Add_Ui_LifeClone(Ui_LifePrototypeTag, UiRenderType, pArg);
}

void CGameInstance::Set_UiManager_Winsize(_uint iWinSizeX, _uint iWinSizeY)
{
	if (nullptr == m_pUi_Manager)
		return;

	m_pUi_Manager->Set_WinSize(iWinSizeX, iWinSizeY);
}

void CGameInstance::Set_Ui_ActiveState(const wstring& Ui_ActiveTag, bool _isActive)
{
	if (nullptr == m_pUi_Manager)
		return;

	m_pUi_Manager->Set_Ui_ActiveState(Ui_ActiveTag, _isActive);
}

void CGameInstance::Ui_Render()
{
	m_pUi_Manager->Ui_Render_Begin();
	m_pUi_Manager->Ui_Render();
	m_pUi_Manager->Ui_Render_End();
}

bool CGameInstance::Get_Ui_ActiveState(const wstring& Ui_ActiveTag)
{
	return m_pUi_Manager->Get_Ui_ActiveState(Ui_ActiveTag);
}

void CGameInstance::Set_Ui_ActiveTextureIndex(const wstring& Ui_ActiveTag, int _iTextureIndex)
{
	m_pUi_Manager->Set_Ui_ActiveTextureIndex(Ui_ActiveTag, _iTextureIndex);
}

int CGameInstance::Get_Ui_ActiveTextureIndex(const wstring& Ui_ActiveTag)
{
	return m_pUi_Manager->Get_Ui_ActiveTextureIndex(Ui_ActiveTag);
}

CUi* CGameInstance::Get_ActiveBlendUI(const wstring& strKey)
{
	return m_pUi_Manager->Get_ActiveBlendUI(strKey);
}

CUi* CGameInstance::Get_ActiveNonBlendUI(const wstring& strKey)
{
	return m_pUi_Manager->Get_ActiveNonBlendUI(strKey);
}

#pragma endregion

#pragma region PHYSX_MANAGER
HRESULT CGameInstance::Create_PhysX_Scene(const wstring& Scene_Tag)
{

	return S_OK;
}

void CGameInstance::Simulate_CurrentScene(_float fTimeDelta)
{
	if (nullptr == m_pPhysX_Manager)
		return;

	m_pPhysX_Manager->Simulate_CurrentScene(fTimeDelta);
}

void CGameInstance::Set_PhysX_Callback(PxSimulationEventCallback* _pPhysX_Callback)
{
	if (nullptr == m_pPhysX_Manager)
		return;

	m_pPhysX_Manager->Set_SimulationEventCallback(_pPhysX_Callback);
}

PxSimulationEventCallback* CGameInstance::Get_PhysX_Callback()
{
	if (nullptr == m_pPhysX_Callback)
		assert(false);

	return m_pPhysX_Callback;
}

#pragma endregion

#pragma region IMGUI_MANAGER
HRESULT CGameInstance::Create_Imgui(HWND hWnd, ID3D11Device* Device, ID3D11DeviceContext* pContext)
{
	if (m_pImgui_Manager != nullptr)
		assert(false);

	m_pImgui_Manager = CImgui_Manager::Create(hWnd, Device, pContext);

	return S_OK;
}

HRESULT CGameInstance::Imgui_ProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(m_pImgui_Manager == nullptr)
		return E_FAIL;

	if(FAILED(m_pImgui_Manager->Imgui_ProcHandler(hWnd, message, wParam, lParam)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameInstance::Imgui_ViewInit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	if (m_pImgui_Manager == nullptr)
		return E_FAIL;

	if (FAILED(m_pImgui_Manager->Imgui_ViewInit(pDevice, pContext)))
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Imgui_SetSelectObject(CGameObject* pObject)
{
	if (m_pImgui_Manager == nullptr)
		return;

	m_pImgui_Manager->Imgui_SetSelectObject(pObject);
}

void CGameInstance::Set_Imgui_State(eImgui_State _bIsImguiOn)
{
	if (m_pImgui_Manager == nullptr)
		return;

	m_pImgui_Manager->Imgui_SetState(_bIsImguiOn);
}

eImgui_State CGameInstance::Get_Imgui_State()
{
	if (m_pImgui_Manager == nullptr)
		return eImgui_State::IMGUI_STATE_END;

	return m_pImgui_Manager->Imgui_GetState();
}


void CGameInstance::Imgui_TickEngine()
{
	if (m_pImgui_Manager == nullptr)
		return;

	if (eImgui_State::IMGUI_STATE_SHUTDOWN != m_pImgui_Manager->Imgui_GetState()) {
		m_pImgui_Manager->PreTick();
		m_pImgui_Manager->Tick();
		m_pImgui_Manager->LateTick();
	}

}

void CGameInstance::Imgui_Render()
{
	if (m_pImgui_Manager == nullptr)
		return;
	
	if(m_pImgui_Manager->Imgui_GetState() == eImgui_State::IMGUI_STATE_RENDER)
		m_pImgui_Manager->Imgui_Render();
}

void CGameInstance::Map_Load(_int iMapNum)
{
	if (m_pImgui_Manager == nullptr)
		return;

	m_pImgui_Manager->Level_MapLoad(iMapNum);
}

#pragma endregion

#pragma region MODEL_CONVERTER_MANAGER


HRESULT CGameInstance::Clear_MeshData()
{
	if(m_pModel_Converter_Manager == nullptr)
		return E_FAIL;

	m_pModel_Converter_Manager->Clear_MeshData();

	if(m_pModel_Converter_Manager->GetMeshData() != nullptr)
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Set_MeshData()
{
	if (m_pModel_Converter_Manager == nullptr)
		return;

	m_pModel_Converter_Manager->Init_MeshData();

	if (m_pModel_Converter_Manager->GetMeshData() == nullptr)
		assert(false);

}

MESH_DATA* CGameInstance::Get_MeshData()
{
	return nullptr;
}
#pragma endregion

#pragma region RESOURCE_CONTAINER

CTexture* CGameInstance::Auto_Search_Texture(const wstring& strTextureTag)
{
	if(m_pResource_Container == nullptr)
		return nullptr;

	return m_pResource_Container->SearchMaterial(strTextureTag);
}

#pragma endregion

#pragma region FONT_MANAGER


HRESULT CGameInstance::Add_Font(const _wstring& strFontTag, const _wstring& strFontFilePath)
{
	return m_pFont_Manager->Add_Font(strFontTag, strFontFilePath);
}

HRESULT CGameInstance::Render_Font(const _wstring& strFontTag, const _wstring& strText, const _float2& vPosition, _fvector vColor, _float fRotation, _float fScale)
{
	return m_pFont_Manager->Render(strFontTag, strText, vPosition, vColor, fRotation, fScale);
}
#pragma endregion

#pragma region LIGHT_MANAGER

const LIGHT_DESC* CGameInstance::Get_LightDesc(_uint iIndex) const
{
	return m_pLight_Manager->Get_LightDesc(iIndex);
}

HRESULT CGameInstance::Add_Light(void* LightDesc)
{
	return m_pLight_Manager->Add_Light(LightDesc);
}

CLight* CGameInstance::Add_Light_Return(void* LightDesc)
{
	return m_pLight_Manager->Add_Light_Return(LightDesc);
}

HRESULT CGameInstance::Render_Lights(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pLight_Manager->Render(pShader, pVIBuffer);
}

void CGameInstance::LightControl(string strLightTag, _bool bOn)
{
	if(nullptr == m_pLight_Manager)
		return;

	m_pLight_Manager->LightControl(strLightTag, bOn);
}

void CGameInstance::LightControl(string strLightTag)
{
	if(nullptr == m_pLight_Manager)
		return;

	m_pLight_Manager->LightControl(strLightTag);
}

void CGameInstance::LightClear()
{
	if(nullptr == m_pLight_Manager)
		return;

	m_pLight_Manager->LightClear();
}

#pragma endregion

#pragma region TARGET_MANAGER
HRESULT CGameInstance::Add_RenderTarget(const _wstring& strRenderTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixel, const _float4& vClearColor)
{
	return m_pTarget_Manager->Add_RenderTarget(strRenderTargetTag, iWidth, iHeight, ePixel, vClearColor);
}

HRESULT CGameInstance::Add_MRT(const _wstring& strMRTTag, const _wstring& strRenderTargetTag)
{
	return m_pTarget_Manager->Add_MRT(strMRTTag, strRenderTargetTag);
}

HRESULT CGameInstance::Begin_MRT(const _wstring& strMRTTag)
{
	return m_pTarget_Manager->Begin_MRT(strMRTTag);
}

HRESULT CGameInstance::End_MRT()
{
	return m_pTarget_Manager->End_MRT();
}

HRESULT CGameInstance::Bind_RT_SRV(const _wstring& strRenderTargetTag, CShader* pShader, const _char* pConstantName)
{
	return m_pTarget_Manager->Bind_RT_SRV(strRenderTargetTag, pShader, pConstantName);
}

#pragma endregion

#pragma region SOUND_MANAGER
HRESULT CGameInstance::Create_Sound(const string& strPath, const wstring& strSoundTag)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->Create_Sound(strPath, strSoundTag);
}

HRESULT CGameInstance::Play(const wstring& strSoundTag, _bool bLoop)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->Play(strSoundTag, bLoop);

	return S_OK;
}

HRESULT CGameInstance::Stop(const wstring& strSoundTag)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->Stop(strSoundTag);
}


HRESULT CGameInstance::SetVolume(const wstring& strSoundTag, const _float& fVolume)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->SetVolume(strSoundTag, fVolume);
}
_bool CGameInstance::Is_Playing(const wstring& strSoundTag)
{
	if (nullptr == m_pSound_Manager)
		return false;

	return m_pSound_Manager->Is_Playing(strSoundTag);
}
#pragma endregion

#ifdef _DEBUG
HRESULT CGameInstance::Add_DebugComponent(CComponent* pComponent)
{
	return m_pRenderer->Add_DebugComponent(pComponent);
}


HRESULT CGameInstance::Ready_RTDebug(const _wstring& strRenderTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	return m_pTarget_Manager->Ready_Debug(strRenderTargetTag, fX, fY, fSizeX, fSizeY);
}

HRESULT CGameInstance::Render_RTDebug(const _wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pTarget_Manager->Render_Debug(strMRTTag, pShader, pVIBuffer);
}

#endif

#pragma region COLLISION_MANAGER

void CGameInstance::Add_Collider(CCollision_Manager::COLLIDER_TYPE _colliderType, CCollider* pCollider)
{
	if(nullptr == m_pCollision_Manager)
		return;

	m_pCollision_Manager->Add_Collider(_colliderType, pCollider);
}

void CGameInstance::Collision_Check()
{
	if(nullptr == m_pCollision_Manager)
		return;

	m_pCollision_Manager->Collision_Check();
}


#pragma endregion

void CGameInstance::Release_Engine()
{
	CGameInstance::Get_Instance()->Free();

	CGameInstance::Get_Instance()->Destroy_Instance();
}

void CGameInstance::Free()
{	
	Safe_Release(m_pImgui_Manager);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pKey_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pMath_Manager);
	Safe_Release(m_pCamera_Manager);
	Safe_Release(m_pUi_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pPhysX_Manager);
	Safe_Delete(m_pPhysX_Callback);
	Safe_Release(m_pModel_Converter_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pCollision_Manager);

	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pResource_Container);
	Safe_Release(m_pGraphic_Device);

}
