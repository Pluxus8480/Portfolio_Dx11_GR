#pragma once

#include "Base.h"
#include "FMOD_Core.h"

BEGIN(Engine)
class CSound : public CBase
{
public:
    CSound();
    ~CSound() = default;

    void Play(const bool& _bLoop);
    void Stop();
    _bool Is_Playing();
    void SetVolume(const float& _fVolume);
    void SetLoop(const bool& _bLoop) { m_bLoop = _bLoop; }

private:
    HRESULT Initialize(CFMOD_Core* pFMOD_Core, const string& strPath);

private:
    CFMOD_Core* m_pFMOD_Core = { nullptr };

    class FMOD::Sound* m_pSound = { nullptr };
    class FMOD::Channel* m_pChannel = { nullptr };
    _bool m_bLoop = false;

public:
    static CSound* Create(CFMOD_Core* pFMOD_Core, const string& strPath);
    virtual void Free() override;

};
END

