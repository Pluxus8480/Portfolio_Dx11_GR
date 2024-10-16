#pragma once

#include "Renderer.h"
#include "Component_Manager.h"
#include "PhysX_Manager.h"
#include "Camera_Manager.h"
#include "Ui_Manager.h"
#include "Key_Manager.h"
#include "IMGUI_Manager.h"
#include "Collision_Manager.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(_uint iNumLevels, const ENGINE_DESC& EngineDesc, _Out_ ID3D11Device** ppDevice, _Out_ ID3D11DeviceContext** ppContext);
	void Tick_Engine(_float fTimeDelta);
	HRESULT Draw();
	void Clear(_uint iLevelIndex);

#pragma region GRAPHIC_DEVICE
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();	
	HRESULT Present();
#pragma endregion

#pragma region LEVEL_MANAGER
public:
	HRESULT Change_Level(class CLevel* pNewLevel);
	_uint Get_CurrentLevelIndex() const;
#pragma endregion

#pragma region TIMER_MANAGER
public:
	_float	Get_TimeDelta(const wstring& strTimerTag);
	HRESULT Add_Timer(const wstring& strTimerTag);
	void Compute_TimeDelta(const wstring& strTimerTag);

	void Set_TimeMultiplier(_float _fTimeMultiplier);
	_float Get_TimeMultiplier();
#pragma endregion

#pragma region OBJECT_MANAGER
public:
	HRESULT Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_Clone(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);
	CGameObject* Clone_GameObject(const wstring& strPrototypeTag, void* pArg = nullptr);
	CGameObject* Add_Clone_Return(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);
	class CComponent* Find_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComponentTag, _uint iIndex = 0);
	class CGameObject* Find_GameObject(_uint iLevelIndex, const wstring& strLayerTag, _uint iIndex);
	class CLayer* Find_Layer(_uint iLevelIndex, const wstring& strLayerTag);
#pragma endregion

#pragma region COMPONENT_MANAGER
public:
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg = nullptr);
#pragma endregion

#pragma region KEY_MANAGER
	bool GetKeyDown(eKeyCode _keyCode) { return	 m_pKey_Manager->GetKeyDown(_keyCode); }
	bool GetKeyUp(eKeyCode _keyCode) { return m_pKey_Manager->GetKeyUp(_keyCode); }
	bool GetKey(eKeyCode _keyCode) { return m_pKey_Manager->GetKey(_keyCode); }
	bool GetKeyNone(eKeyCode _keyCode) { return m_pKey_Manager->GetKeyNone(_keyCode); }
	//_float2 GetMousePos() { return m_pKey_Manager->GetMousePos(); }
#pragma endregion

#pragma region RENDERER
public:
	HRESULT Add_RenderObjects(CRenderer::RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);

	void Set_DebugMode(_bool bDebug);
	_bool Get_DebugMode();
#pragma endregion

#pragma region MATH_MANAGER
public:
	_float2 Lissajous_Curve(IN _float _fTimeDelta, IN OUT _float& _fLissajousTime, IN _float _fWitth, IN _float _fHeight, IN _float _fLagrangianX, IN _float _fLagrangianY, IN _float _fPhaseDelta, IN _float _fLissajousSpeed);
	_float3 Vector3x3_BezierCurve(IN _float3 _vStart, IN _float3 _vControl, IN _float3 _vEnd, IN OUT _float& _fTimeControl, IN _float _fTimeDelta, IN _float _fSpeed = 1.f);

	_float  Random_Float(_float _fMin, _float _fMax);
	_float2 Random_Float2(_float2 _f2Min, _float2 _f2Max);
	_float3 Random_Float3(_float3 _f3Min, _float3 _f3Max);

	_int	Random_Int(_int _iMin, _int _iMax);
	_vector XMQuaternionRotationBetweenVectors(_vector start, _vector end);
	_int	Random_Binary();
	_bool	Random_Bool();

	PxVec3  Add_PxVec3toXMVECTOR(PxVec3 _Pxvec3, _vector _Xmvec);
#pragma endregion

#pragma region CAMERA_MANAGER
public:
	void Set_Transform(CCamera_Manager::TRANSFORMATIONSTATE eState, _fmatrix TransformationMatrix);
	_matrix Get_Transform_Matrix(CCamera_Manager::TRANSFORMATIONSTATE eState) const;
	const _float4x4* Get_Transform_Float4x4(CCamera_Manager::TRANSFORMATIONSTATE eState) const;
	_matrix Get_Transform_Matrix_Inverse(CCamera_Manager::TRANSFORMATIONSTATE eState) const;
	const _float4x4* Get_Transform_Float4x4_Inverse(CCamera_Manager::TRANSFORMATIONSTATE eState) const;
	_vector Get_CamPosition_Vector() const;
	const _float4* Get_CamPosition_Float4() const;
	const _float3* Get_CamPosition_Float3() const;
	_matrix Get_BillBoard_Matrix() const;
	_float4x4* Get_BillBoard_Matrix4x4();

	HRESULT Create_Camera(CCoreCamera* _pCamera, const wstring& _wstrCameraKey);
	HRESULT Change_Camera(const wstring& _wstrCameraKey);
	void Set_Target(CGameObject* _pTarget);

	CCoreCamera* Get_CurCamera();
	CTransform* Get_CurCamera_Transform();

	void Set_UseGlitchLens(_bool _bUseGlitchLens);
	_bool Get_UseGlitchLens();

	void Set_UseRedBlockGlitch(_bool _bUseRedBlockGlitch);
	_bool Get_UseRedBlockGlitch();

	void Set_GlitchPower(_float _fPower);
	_float Get_GlitchPower();

	void Set_GlitchRate(_float _fRate);
	_float Get_GlitchRate();

	void Set_GlitchSpeed(_float _fSpeed);
	_float Get_GlitchSpeed();

	void Set_GlitchColorRate(_float _fColorRate);
	_float Get_GlitchColorRate();

	void Set_RedBlockMixRate(_float _fMixRate);
	_float Get_RedBlockMixRate();

	void Set_BlackScreenRate(_float _fRate);
	_float Get_BlackScreenRate();

	void Set_BlackScreen(_bool _bUseBlackScreen);
	_bool Get_BlackScreen();

	void Set_Fov(_float _fFov);
	_float Get_Fov();

	void Set_BlurScreen(_bool _bUseBlurScreen);
	_bool Get_BlurScreen();

	void Set_BlueScreen(_bool _bUseBlueScreen);
	_bool Get_BlueScreen();

	void Set_RedScreen(_bool _bUseRedScreen);
	_bool Get_RedScreen();

	void Set_BlurPower(_float _fPower);
	_float Get_BlurPower();

	void Set_BlueScreenRate(_float _fRate);
	_float Get_BlueScreenRate();

	void Set_ShakePower(_float _fPower);
	_float Get_ShakePower();

	void Set_ShakeTime(_float _fTime);
	_float Get_ShakeTime();

#pragma endregion

#pragma region UI_MANAGER
	HRESULT Add_Ui_LifePrototype(const wstring& Ui_LifePrototypeTag, class CUi* Ui_LifePrototype);
	HRESULT Add_Ui_Active(const wstring& Ui_ActiveTag, eUiRenderType Ui_RenderType, class CUi* Ui_Active);
	HRESULT Add_Ui_LifeClone(const wstring& Ui_LifePrototypeTag, eUiRenderType UiRenderType, void* pArg);
	void Set_UiManager_Winsize(_uint iWinSizeX, _uint iWinSizeY);
	void Set_Ui_ActiveState(const wstring& Ui_ActiveTag, bool _isActive = true);
	// for UiClear
	void Ui_Render();
	bool Get_Ui_ActiveState(const wstring& Ui_ActiveTag);
	void Set_Ui_ActiveTextureIndex(const wstring& Ui_ActiveTag, int _iTextureIndex);
	int  Get_Ui_ActiveTextureIndex(const wstring& Ui_ActiveTag);

	CUi* Get_ActiveBlendUI(const wstring& strKey);
	CUi* Get_ActiveNonBlendUI(const wstring& strKey);


#pragma endregion

#pragma region PHYSX_MANAGER
	HRESULT Create_PhysX_Scene(const wstring& Scene_Tag);
	PxPhysics* Get_PhysX_Physics() { return m_pPhysX_Manager->Get_Physics(); }
	PxScene* Get_PhysX_Scene() { return m_pPhysX_Manager->Get_Scene(); }
	PxControllerManager* Get_PhysX_ControllerManager() { return m_pPhysX_Manager->Get_ControllerManager(); }
	 
	void Simulate_CurrentScene(_float fTimeDelta);

	void Set_PhysX_Callback(PxSimulationEventCallback* _pPhysX_Callback);
	PxSimulationEventCallback* Get_PhysX_Callback();

#pragma endregion

#pragma region IMGUI_MANAGER
	HRESULT Create_Imgui(HWND hWnd, ID3D11Device* Device, ID3D11DeviceContext* pContext);
	HRESULT Imgui_ProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HRESULT Imgui_ViewInit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	void Imgui_SetSelectObject(CGameObject* pObject);

	void Set_Imgui_State(eImgui_State _bIsImguiOn);
	eImgui_State Get_Imgui_State();

	void Imgui_TickEngine();
	void Imgui_Render();

	void Map_Load(_int iMapNum);
#pragma endregion

#pragma region MODEL_CONVERTER_MANAGER
	HRESULT Clear_MeshData();
	void	Set_MeshData();

	MESH_DATA* Get_MeshData();
#pragma endregion

#pragma region RESOURCE_CONTAINER
	CTexture* Auto_Search_Texture(const wstring& strTextureTag);


#pragma endregion

#pragma region FONT_MANAGER
	HRESULT Add_Font(const _wstring& strFontTag, const _wstring& strFontFilePath);
	HRESULT Render_Font(const _wstring& strFontTag, const _wstring& strText, const _float2& vPosition, _fvector vColor = XMVectorSet(0.f, 0.f, 0.f, 1.f), _float fRotation = 0.f, _float fScale = 1.f);
#pragma endregion

#pragma region LIGHT_MANAGER
	const LIGHT_DESC* Get_LightDesc(_uint iIndex) const;
	HRESULT Add_Light(void* LightDesc);
	class CLight* Add_Light_Return(void* LightDesc);
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	void LightControl(string strLightTag, _bool bOn);
	void LightControl(string strLightTag);
	void LightClear();
#pragma endregion

#pragma region TARGET_MANAGER
	HRESULT Add_RenderTarget(const _wstring& strRenderTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixel, const _float4& vClearColor);
	HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strRenderTargetTag);
	HRESULT Begin_MRT(const _wstring& strMRTTag);
	HRESULT End_MRT();
	HRESULT Bind_RT_SRV(const _wstring& strRenderTargetTag, class CShader* pShader, const _char* pConstantName);
#pragma endregion

#pragma region COLLISION_MANAGER
	void Add_Collider(CCollision_Manager::COLLIDER_TYPE _colliderType, class CCollider* pCollider);
	void Collision_Check();
#pragma endregion

#pragma region SOUND_MANAGER
	HRESULT Create_Sound(const string& strPath, const wstring& strSoundTag);
	HRESULT Play(const wstring& strSoundTag, _bool bLoop);
	HRESULT Stop(const wstring& strSoundTag);
	HRESULT SetVolume(const wstring& strSoundTag, const _float& fVolume);
	_bool Is_Playing(const wstring& strSoundTag);
#pragma endregion

#ifdef _DEBUG
public:
	HRESULT Add_DebugComponent(class CComponent* pComponent);
	HRESULT Ready_RTDebug(const _wstring& strRenderTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_RTDebug(const _wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif

private:
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CComponent_Manager*		m_pComponent_Manager = { nullptr };
	class CKey_Manager*				m_pKey_Manager = { nullptr };
	class CCamera_Manager*			m_pCamera_Manager = { nullptr };
	class CRenderer*				m_pRenderer = { nullptr };	
	class CUi_Manager*				m_pUi_Manager = { nullptr };
	class CMath_Manager*			m_pMath_Manager = { nullptr };
	class CPhysX_Manager*			m_pPhysX_Manager = { nullptr };
	class CImgui_Manager*			m_pImgui_Manager = { nullptr };
	class CResource_Container*		m_pResource_Container = { nullptr };
	class CModel_Converter_Manager* m_pModel_Converter_Manager = { nullptr };
	class CCollision_Manager*		m_pCollision_Manager = { nullptr };
	class CSound_Manager*			m_pSound_Manager = { nullptr };

	class CFont_Manager* m_pFont_Manager = { nullptr };
	class CLight_Manager* m_pLight_Manager = { nullptr };
	class CTarget_Manager* m_pTarget_Manager = { nullptr };

	PxSimulationEventCallback*		m_pPhysX_Callback = { nullptr };
public:
	static void Release_Engine();
	virtual void Free() override;
	
};

END