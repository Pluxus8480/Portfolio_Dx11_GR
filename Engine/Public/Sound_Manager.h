#pragma once

#include "Base.h"


BEGIN(Engine)

class CSound_Manager final : public CBase
{
private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

	HRESULT Initialize();
public:
	HRESULT Create_Sound(const string& strPath, const wstring& strSoundTag);
	HRESULT Play(const wstring& strSoundTag, _bool bLoop);
	HRESULT Stop(const wstring& strSoundTag);
	HRESULT SetVolume(const wstring& strSoundTag, const _float& fVolume);

	_bool Is_Playing(const wstring& strSoundTag);
private:
	class CFMOD_Core* m_pFmodCore = { nullptr };

	map<const wstring, class CSound*> m_Sounds;

public:
	static CSound_Manager* Create();
	virtual void Free() override;
};

END