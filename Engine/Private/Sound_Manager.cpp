#include "Sound_Manager.h"

#include "Sound.h"
#include "FMOD_Core.h"

CSound_Manager::CSound_Manager()
{
}

HRESULT CSound_Manager::Initialize()
{
	m_pFmodCore = CFMOD_Core::Create();

	return S_OK;
}

HRESULT CSound_Manager::Create_Sound(const string& strPath, const wstring& strSoundTag)
{
	CSound* pSound = CSound::Create(m_pFmodCore, strPath);

	if (nullptr == pSound)
	{
		assert(false);
		return E_FAIL;
	}

	auto iter = m_Sounds.find(strSoundTag);

	if (iter == m_Sounds.end())
	{
		m_Sounds.insert({ strSoundTag, pSound });
	}
	else
	{
		Safe_Release(pSound);
	}

	return S_OK;
}

HRESULT CSound_Manager::Play(const wstring& strSoundTag, _bool bLoop)
{
	auto it = m_Sounds.find(strSoundTag);

	if (m_Sounds.end() == it)
		return E_FAIL;

	it->second->Play(bLoop);

	return S_OK;
}

HRESULT CSound_Manager::Stop(const wstring& strSoundTag)
{
	auto it = m_Sounds.find(strSoundTag);

	if (m_Sounds.end() == it)
		return E_FAIL;

	it->second->Stop();

	return S_OK;
}

HRESULT CSound_Manager::SetVolume(const wstring& strSoundTag, const _float& fVolume)
{
	auto it = m_Sounds.find(strSoundTag);

	if (m_Sounds.end() == it)
		return E_FAIL;

	it->second->SetVolume(fVolume);

	return S_OK;
}

_bool CSound_Manager::Is_Playing(const wstring& strSoundTag)
{
	auto it = m_Sounds.find(strSoundTag);

	if (m_Sounds.end() == it)
		return false;

	return it->second->Is_Playing();
}

CSound_Manager* CSound_Manager::Create()
{
	CSound_Manager* pInstance = new CSound_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CSound_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSound_Manager::Free()
{
	for (auto& pair : m_Sounds)
	{
		Safe_Release(pair.second);
	}

	Safe_Release(m_pFmodCore);
}
