#include "..\Public\Channel.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const aiNodeAnim* pAIChannel, const vector<CBone*>& Bones)
{
	strcpy_s(m_szName, pAIChannel->mNodeName.data);

	auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool
		{
			if (false == strcmp(m_szName, pBone->Get_Name()))
				return true;

			++m_iBoneIndex;

			return false;
		});

	m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);

	_float3			vScale{};
	_float4			vRotation{};
	_float3			vTranslation{};

	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME			KeyFrame{};

		if (i < pAIChannel->mNumScalingKeys)
		{
			memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTrackPosition = (_float)pAIChannel->mScalingKeys[i].mTime;
		}

		if (i < pAIChannel->mNumRotationKeys)
		{
			vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;
			KeyFrame.fTrackPosition = (_float)pAIChannel->mRotationKeys[i].mTime;
		}

		if (i < pAIChannel->mNumPositionKeys)
		{
			memcpy(&vTranslation, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTrackPosition = (_float)pAIChannel->mPositionKeys[i].mTime;
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vTranslation = vTranslation;

		m_KeyFrames.emplace_back(KeyFrame);
	}

	return S_OK;
}

HRESULT CChannel::Initialize(shared_ptr<CHANNEL_DATA> pAIChannel, const vector<class CBone*>& Bones)
{
	strcpy_s(m_szName, pAIChannel->szNodeName.c_str());

	auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool
		{
			if (false == strcmp(m_szName, pBone->Get_Name()))
				return true;

			++m_iBoneIndex;

			return false;
		});

	m_iNumKeyFrames = max(pAIChannel->iNumScalingKeys, pAIChannel->iNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->iNumPositionKeys);

	_float3			vScale{};
	_float4			vRotation{};
	_float3			vTranslation{};

	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME			KeyFrame{};

		if (i < pAIChannel->iNumScalingKeys)
		{
			memcpy(&vScale, &pAIChannel->tKeyFrames[i].vScale, sizeof(_float3));
			KeyFrame.fTrackPosition = (_float)pAIChannel->tKeyFrames[i].fTime;
		}

		if (i < pAIChannel->iNumRotationKeys)
		{
			vRotation.x = pAIChannel->tKeyFrames[i].vRotation.x;
			vRotation.y = pAIChannel->tKeyFrames[i].vRotation.y;
			vRotation.z = pAIChannel->tKeyFrames[i].vRotation.z;
			vRotation.w = pAIChannel->tKeyFrames[i].vRotation.w;
			KeyFrame.fTrackPosition = (_float)pAIChannel->tKeyFrames[i].fTime;
		}

		if (i < pAIChannel->iNumPositionKeys)
		{
			memcpy(&vTranslation, &pAIChannel->tKeyFrames[i].vTranslation, sizeof(_float3));
			KeyFrame.fTrackPosition = (_float)pAIChannel->tKeyFrames[i].fTime;
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vTranslation = vTranslation;

		m_KeyFrames.emplace_back(KeyFrame);
	}

	return S_OK;
}

void CChannel::Update(_float fCurrentTrackPosition, const vector<CBone*>& Bones, _uint* pCurrentKeyFrameIndex)
{
	if (0.0f == fCurrentTrackPosition)
		(*pCurrentKeyFrameIndex) = 0;

	/* Fin. 현재 재생위치에 맞는 상태 행렬을 만든다. */
	_matrix			TransformationMatrix = XMMatrixIdentity();

	_vector			vScale, vRotation, vTranslation;

	KEYFRAME		LastKeyFrame = m_KeyFrames.back();

	/* 현재 재생 위치가 이 뼈의 마지막 키프레임을 넘어서면. */
	if (fCurrentTrackPosition > LastKeyFrame.fTrackPosition)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f);
	}
	/* 현재 재생위치가 두개의 키프레임들 사이에 있다. */
	/* 두개의 키프레임사이를 재생위치에 맞게 선형보간한다. */
	/* (*pCurrentKeyFrameIndex) : 현재 재생중인 트랙포지션 기준 왼쪽에 존재하는 키프레임 인덷ㄱ스 */
	else
	{
		while (fCurrentTrackPosition >= m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].fTrackPosition)
			++(*pCurrentKeyFrameIndex);

		_float		fRatio = (fCurrentTrackPosition - m_KeyFrames[(*pCurrentKeyFrameIndex)].fTrackPosition) /
			(m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].fTrackPosition - m_KeyFrames[(*pCurrentKeyFrameIndex)].fTrackPosition);

		vScale = XMVectorLerp(XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vScale), XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vScale), fRatio);
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vRotation), XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vRotation), fRatio);
		vTranslation = XMVectorSetW(XMVectorLerp(XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vTranslation), XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vTranslation), fRatio), 1.f);
	}

	TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

void CChannel::Update_Linear_Aperture(_float fCurrentTrackPosition, const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex, _float fAperture)
{
	if (0.0f == fCurrentTrackPosition)
		(*pCurrentKeyFrameIndex) = 0;

	/* Fin. 현재 재생위치에 맞는 상태 행렬을 만든다. */
	_matrix			TransformationMatrix = XMMatrixIdentity();

	_vector			vScale, vRotation, vTranslation;

	KEYFRAME		LastKeyFrame = m_KeyFrames.back();

	/* 현재 재생 위치가 이 뼈의 마지막 키프레임을 넘어서면. */
	if (fCurrentTrackPosition > LastKeyFrame.fTrackPosition)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f);
	}
	/* 현재 재생위치가 두개의 키프레임들 사이에 있다. */
	/* 두개의 키프레임사이를 재생위치에 맞게 선형보간한다. */
	/* (*pCurrentKeyFrameIndex) : 현재 재생중인 트랙포지션 기준 왼쪽에 존재하는 키프레임 인덷ㄱ스 */
	else
	{
		while (fCurrentTrackPosition >= m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].fTrackPosition)
			++(*pCurrentKeyFrameIndex);

		_float		fRatio = (fCurrentTrackPosition - m_KeyFrames[(*pCurrentKeyFrameIndex)].fTrackPosition) /
			(m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].fTrackPosition - m_KeyFrames[(*pCurrentKeyFrameIndex)].fTrackPosition);

		vScale = XMVectorLerp(XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vScale), XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vScale), fRatio);
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vRotation), XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vRotation), fRatio);
		vTranslation = XMVectorSetW(XMVectorLerp(XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vTranslation), XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vTranslation), fRatio), 1.f);
	}

	TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

	Bones[m_iBoneIndex]->Set_TransformationMatrix((Bones[m_iBoneIndex]->Get_TransformationMatrix() * fAperture) + (TransformationMatrix * (1.f - fAperture)));
}

CChannel* CChannel::Create(const aiNodeAnim* pAIChannel, const vector<CBone*>& Bones)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pAIChannel, Bones)))
	{
		MSG_BOX(TEXT("Failed to Created : CChannel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CChannel* CChannel::Create(shared_ptr<CHANNEL_DATA> pAIChannel, const vector<class CBone*>& Bones)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pAIChannel, Bones)))
	{
		MSG_BOX(TEXT("Failed to Created : CChannel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CChannel::Free()
{
	__super::Free();

	m_KeyFrames.clear();
}

