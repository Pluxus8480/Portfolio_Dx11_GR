#include "Sound.h"

CSound::CSound()
{
}

void CSound::Play(const bool& _bLoop)
{
	if (_bLoop)
		m_pSound->setMode(FMOD_LOOP_NORMAL);
	else
		m_pSound->setMode(FMOD_LOOP_OFF);

	m_pFMOD_Core->SoundPlay(m_pSound, &m_pChannel);
}

void CSound::Stop()
{
	m_pChannel->stop();
}

_bool CSound::Is_Playing()
{
	_bool isPlaying;
	m_pChannel->isPlaying(&isPlaying);
	return isPlaying;
}

void CSound::SetVolume(const float& _fVolume)
{
	m_pChannel->setVolume(_fVolume);
}

HRESULT CSound::Initialize(CFMOD_Core* pFMOD_Core, const string& strPath)
{
	m_pFMOD_Core = pFMOD_Core;
	Safe_AddRef(m_pFMOD_Core);

	if (false == m_pFMOD_Core->CreateSound(strPath, &m_pSound))
		return E_FAIL;

	return S_OK;
}

CSound* CSound::Create(CFMOD_Core* pFMOD_Core, const string& strPath)
{
	CSound* pInstance = new CSound;

	if (FAILED(pInstance->Initialize(pFMOD_Core, strPath)))
	{
		MSG_BOX(TEXT("Failed to Created : CSound"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSound::Free()
{
	m_pSound->release();
	m_pSound = nullptr;

	Safe_Release(m_pFMOD_Core);
}
