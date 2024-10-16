#pragma once

#include "Base.h"
#include "Core_Camera.h"
#include "GameObject.h"

BEGIN(Engine)

class CCamera_Manager final : public CBase
{
public:
	enum TRANSFORMATIONSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };
private:
	CCamera_Manager();
	virtual ~CCamera_Manager() = default;

public:
	CTransform* Get_CurCamera_Transform();
	void Set_Transform(TRANSFORMATIONSTATE eState, _fmatrix TransformationMatrix);

public:
	_matrix Get_Transform_Matrix(TRANSFORMATIONSTATE eState) const;
	const _float4x4* Get_Transform_Float4x4(TRANSFORMATIONSTATE eState) const;
	_matrix Get_Transform_Matrix_Inverse(TRANSFORMATIONSTATE eState) const;
	const _float4x4* Get_Transform_Float4x4_Inverse(TRANSFORMATIONSTATE eState) const;
	_vector Get_CamPosition_Vector() const;
	const _float4* Get_CamPosition_Float4() const;
	_matrix Get_BillBoardMatrix() const;
	_float4x4* Get_BillBoardMatrix4x4();

	_matrix Get_WorldMatrix() const;
	//_float4x4 Get_WorldMatrix4x4() const;

	HRESULT Change_Camera(const wstring& _wstrCameraKey);
	void Set_Target(CGameObject* _pTarget) { m_pTarget = _pTarget; }

	//HRESULT Render_CurCamera(const _float& fTimeDelta);
	HRESULT Create_Camera(CCoreCamera* _pCamera, const wstring& _wstrCameraKey);
	CCoreCamera* Get_CurCamera() { return m_pCurCamera; }

	void Camera_Shake();

	void Set_ShakePower(_float _fPower) { m_fShakePower = _fPower; }
	_float Get_ShakePower() { return m_fShakePower; }

	void Set_ShakeTime(_float _fTime) { m_fShakeTime = _fTime; }
	_float Get_ShakeTime() { return m_fShakeTime; }

#pragma region GlitchLens
	void Set_UseGlitchLens(_bool _bUseGlitchLens) { m_bUseGlitchLens = _bUseGlitchLens; }
	_bool Get_UseGlitchLens() { return m_bUseGlitchLens; }

	void Set_UseRedBlockGlitch(_bool _bUseRedBlockGlitch) { m_bUseRedBlockGlitch = _bUseRedBlockGlitch; }
	_bool Get_UseRedBlockGlitch() { return m_bUseRedBlockGlitch; }

	void Set_GlitchPower(_float _fPower) { m_fShake_power = _fPower; }
	_float Get_GlitchPower() { return m_fShake_power; }

	void Set_GlitchRate(_float _fRate) { m_fShake_rate = _fRate; }
	_float Get_GlitchRate() { return m_fShake_rate; }

	void Set_GlitchSpeed(_float _fSpeed) { m_fShake_speed = _fSpeed; }
	_float Get_GlitchSpeed() { return m_fShake_speed; }

	void Set_GlitchColorRate(_float _fColorRate) { m_fShake_color_rate = _fColorRate; }
	_float Get_GlitchColorRate() { return m_fShake_color_rate; }

	void Set_RedBlockMixRate(_float _fMixRate) { m_fRed_mix_rate = _fMixRate; }
	_float Get_RedBlockMixRate() { return m_fRed_mix_rate; }

	void Set_BlackScreenRate(_float _fRate) { m_fBlack_Screen_Rate = _fRate; }
	_float Get_BlackScreenRate() { return m_fBlack_Screen_Rate; }

	void Set_BlackScreen(_bool _bUseBlackScreen) { m_bUseBlackScreen = _bUseBlackScreen; }
	_bool Get_BlackScreen() { return m_bUseBlackScreen; }

	void Set_Fov(_float _fFov) { m_fFov = _fFov; }
	_float Get_Fov() { return m_fFov; }

	void Set_BlurScreen(_bool _bUseBlurScreen) { m_bUseBlurScreen = _bUseBlurScreen; }
	_bool Get_BlurScreen() { return m_bUseBlurScreen; }

	void Set_BlurStrength(_float _fStrength) { g_BlurStrength = _fStrength; }
	_float Get_BlurStrength() { return g_BlurStrength; }

	void Set_BlueScreen(_bool _bUseBlueScreen) { m_bUseBlueScreen = _bUseBlueScreen; }
	_bool Get_BlueScreen() { return m_bUseBlueScreen; }

	void Set_RedScreen(_bool _bUseRedScreen) { m_bUseRedScreen = _bUseRedScreen; }
	_bool Get_RedScreen() { return m_bUseRedScreen; }

	void Set_BlueScreenRate(_float _fRate) { g_BlueScreenRate = _fRate; }
	_float Get_BlueScreenRate() { return g_BlueScreenRate; }

#pragma endregion

public:
	HRESULT Initialize();
	void Update();

private:
	_matrix				m_TempWorldMatrix = {};

	_float4x4			m_TransformationMatrices[D3DTS_END];
	_float4x4			m_TransformationMatrix_Inverse[D3DTS_END];
	_float4x4			m_BillBoardMatrix = {};
	_float4				m_vCamPosition = {};

	_bool               m_bUseGlitchLens = false;
	_bool				m_bUseRedBlockGlitch = false;
	_bool				m_bUseBlackScreen = false;
	_bool				m_bUseBlurScreen = false;
	_bool				m_bUseBlueScreen = false;
	_bool				m_bUseRedScreen = false;

#pragma region CameraShake

	_float				m_fShakeTime = 0.f;
	_float				m_fShakePower = 0.f;

#pragma endregion


#pragma region Lens

	_float m_fFov = 90.f;

	// Glitch effect parameters
	_float m_fShake_power = 0.03f;
	_float m_fShake_rate = 0.1f; // Reduced shake rate to lower frequency of glitches
	_float m_fShake_speed = 5.0f;
	_float m_fShake_block_size = 30.5f;
	_float m_fShake_color_rate = 0.01f;
	_float m_fRed_mix_rate = 0.01f; // Red color mix rate
	_float m_fRed_block_min_length = 300.0f; // Minimum length of the red glitch block in pixels
	_float m_fRed_block_max_length = 800.0f; // Maximum length of the red glitch block in pixels
	_float m_fRed_block_height = 15.0f; // Height of the red glitch block in pixels
	_float m_fBlack_Screen_Rate = 0.0f; // Alpha value of the black screen

	// Blur effect parameters
	_float g_BlurStrength = 5.f;
	_float g_BlueScreenRate = 0.f;
#pragma endregion
	map<wstring, CCoreCamera*> m_mapCamera;
	wstring m_wstrCurCameraKey;
	CCoreCamera* m_pCurCamera = nullptr;
	CGameObject* m_pTarget = nullptr;

public:
	static CCamera_Manager* Create();
	virtual void Free() override;
};

END