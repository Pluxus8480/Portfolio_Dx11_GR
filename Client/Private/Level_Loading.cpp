#include "..\Public\Level_Loading.h"

#include "GameInstance.h"
#include "Loader.h"
#include "Level_Logo.h"
#include "Level_GamePlay.h"
#include "Level_Tool.h"
#include "Level_Lab.h"
#include "Level_Boss.h"
#include "GameManager.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext } 
{

}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevelID)
{
	m_iLevelID = LEVEL_LOADING;

	m_eNextLevelID = eNextLevelID;

	/* 자원로딩을 위한 로더객체를 생성해준다. */
	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevelID);
	if (nullptr == m_pLoader)
		return E_FAIL;	

	/*GAMEMANAGER->VideoSetting();*/
	return S_OK;
}

void CLevel_Loading::Tick(_float fTimeDelta)
{
	
	/*GAMEMANAGER->VideoRender();*/


	if (true == m_pLoader->isFinished())
	{
		if (GetKeyState(VK_SPACE) & 0x8000)
		{
			CLevel*		pLevel = { nullptr };

			switch (m_eNextLevelID)
			{
			case LEVEL_LOGO:
				pLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_GAMEPLAY:
				pLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_TOOL:
				pLevel = CLevel_Tool::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_LAB:
				pLevel = CLevel_Lab::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_LAST:
				pLevel = CLevel_Boss::Create(m_pDevice, m_pContext);
				break;
			}

			if (nullptr == pLevel)
				return;

			if (FAILED(m_pGameInstance->Change_Level(pLevel)))
				return;
		}		
	}	
}

HRESULT CLevel_Loading::Render()
{	
	m_pLoader->Show_LoadingText();
	
	return S_OK;
}


CLevel_Loading * CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLevel_Loading*	pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Loading"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
