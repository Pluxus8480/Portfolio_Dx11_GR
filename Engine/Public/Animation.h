#pragma once

#include "Base.h"

/* �ִϸ��̼��� �ϳ�(���ݸ��)�� ������ ������. */
BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	_bool Get_Finished() const {
		return m_isFinished;
	}

	void Set_Finished(_bool isFinished) {
		m_isFinished = isFinished;
	}

public:
	HRESULT Initialize(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);
	HRESULT Initialize(shared_ptr<ANIMATION_DATA> pAIAnimation, const vector<class CBone*>& Bones);
	void Update_TransformationMatrix(_float fTimeDelta, const vector<class CBone*>& Bones, _bool isLoop);
	void Update_Transformation_Aperture(const vector<class CBone*>& Bones);
	void Reset();

	_float Get_Aperture() const {
		return m_fAperture;
	}

	void Set_Aperture(_float fAperture) {
		m_fAperture = fAperture;
	}

	_float Get_AnimationMultiplier() const {
		return m_fAnimationMultiplier;
	}

	void Set_AnimationMultiplier(_float fAnimationMultiplier) {
		m_fAnimationMultiplier = fAnimationMultiplier;
	}

private:
	_char				m_szName[MAX_PATH] = {};
	_float				m_fDuration = {}; /* �ִϸ��̼��� �� ��� �Ÿ�. */
	_float				m_fTickPerSecond = {}; /* �ʴ� ��� �Ÿ�.(��� �ӵ�) */
	_float				m_fCurrentTrackPosition = {}; /* ���� ��� ��ġ */

	_float				m_fAnimationMultiplier = 1.f;
	_float				m_fAperture = 1.f;

	/* �� �ִϸ��̼��� ����ϴµ� �ʿ��� ���� ����. == */
	/* �� �ִԿ��Ӽ��� ����ϴ� ���� ����. */
	_uint						m_iNumChannels = {};
	vector<class CChannel*>		m_Channels;
	/* ���� ä��(��)�� ���� ��� Ű�����ӱ��� ����Ȱ����� �����ϰ� �ִ� �����͸� */
	/* �ִϸ��̼���(�ٱ�)���� ���ػ�Ȳ. */
	vector<_uint>				m_CurrentKeyFrameIndices;

	_bool						m_isFinished = { false };


public:
	static CAnimation* Create(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);
	static CAnimation* Create(shared_ptr<ANIMATION_DATA> pAnimation, const vector<class CBone*>& Bones);
	CAnimation* Clone();
	virtual void Free() override;
};

END