#pragma once

#include "Client_Defines.h"
#include "Base.h"

/* 2. �ε� ���� ������ �ʿ��� �ڿ��� �����Ѵ�.  */
/* ���� ������ ���� �ڿ� �ε� : ��Ƽ������ */

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	HRESULT Initialize(LEVEL eNextLevelID);
	_uint Loading();
	_bool isFinished() const {
		return m_isFinished;
	}

	void Show_LoadingText() {
		SetWindowText(g_hWnd, m_szLoadingText);
	}

	//Ui Loading 
	HRESULT Load_Ui();
	
	HRESULT Load_Ui_Skill_Textures();
	HRESULT Load_Ui_Skill_Prototype();

	HRESULT Load_Object_Textures();

	HRESULT Load_Model();



private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	HANDLE					m_hThread = {};
	CRITICAL_SECTION		m_CriticalSection = {};
	LEVEL					m_eNextLevelID = { LEVEL_END };

	_tchar					m_szLoadingText[MAX_PATH] = TEXT("");
	_float					m_fLoadingPercent = { 0.0f };
	_bool					m_isFinished = { false };

private:
	CGameInstance*			m_pGameInstance = { nullptr };

private:
	HRESULT Loading_For_Logo_Level();
	HRESULT Loading_For_GamePlay_Level();
	HRESULT Loading_For_Lab_Level();
	HRESULT Loading_For_Tool_Level();
	HRESULT Loading_For_Boss_Level();

public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID);
	virtual void Free() override;
};

END