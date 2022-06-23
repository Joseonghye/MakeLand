#include "..\public\Animation.h"
#include "AnimChannel.h"

Animation::Animation()
	:m_isFinished(false),
	m_PlaySpeed(0), m_Duration(0), m_TrackPositionAcc(0)
{
}

void Engine::Animation::Reset()
{
	m_isFinished = false;
	m_TrackPositionAcc = 0.0;

	for (auto& pChannel : m_Channels)
		pChannel->SetCurKeyFrameIndex(0);
}

HRESULT Engine::Animation::Ready(char * pName, _double Duration, _double PlaySpeed)
{
	strcpy_s(m_szName, pName);
	m_Duration = Duration;
	m_PlaySpeed = PlaySpeed;
	return S_OK;
}

HRESULT Engine::Animation::UpdateTransformationMatrix(_double TimeDelta)
{
	m_TrackPositionAcc += m_PlaySpeed * TimeDelta;
	if (m_TrackPositionAcc >= m_Duration)
	{
		m_isFinished = true;
		m_TrackPositionAcc = 0.0;
	}
	else
		m_isFinished = false;

	_uint iNumChannel = m_Channels.size();
	for (_uint i = 0; i < iNumChannel; ++i)
	{
	//	vector<KEYFRAME*>		KeyFrames = m_Channels[i]->GetKeyFrames();
	//	_uint	iCurrentKeyFrameIndex = m_Channels[i]->GetCurKeyFrameIndex();

	/*	_vector		vScale, vRotation, vPosition;
		vScale = XMVectorZero();
		vRotation = XMVectorZero();
		vPosition = XMVectorZero();*/

		if (true == m_isFinished)
		{
		//	iCurrentKeyFrameIndex = 0;
			m_Channels[i]->SetCurKeyFrameIndex(0);
		}
		m_Channels[i]->UpdateTransformationMatrix(m_TrackPositionAcc);

		/*_uint		iNumKeyFrame = KeyFrames.size();

		if (m_TrackPositionAcc <= KeyFrames[0]->Time)
		{
			vScale = XMLoadFloat3(&KeyFrames[0]->vScale);
			vRotation = XMLoadFloat4(&KeyFrames[0]->vRotation);
			vPosition = XMLoadFloat3(&KeyFrames[0]->vPosition);
			vPosition = XMVectorSetW(vPosition, 1.f);
		}
		else if (m_TrackPositionAcc > KeyFrames[iNumKeyFrame - 1]->Time)
		{
			vScale = XMLoadFloat3(&KeyFrames[iNumKeyFrame - 1]->vScale);
			vRotation = XMLoadFloat4(&KeyFrames[iNumKeyFrame - 1]->vRotation);
			vPosition = XMLoadFloat3(&KeyFrames[iNumKeyFrame - 1]->vPosition);
			vPosition = XMVectorSetW(vPosition, 1.f);
		}
		else
		{
			if (m_TrackPositionAcc >= KeyFrames[iCurrentKeyFrameIndex + 1]->Time)
				m_Channels[i]->SetCurKeyFrameIndex(++iCurrentKeyFrameIndex);

			_float		fRatio = (m_TrackPositionAcc - KeyFrames[iCurrentKeyFrameIndex]->Time) /
				(KeyFrames[iCurrentKeyFrameIndex + 1]->Time - KeyFrames[iCurrentKeyFrameIndex]->Time);

			_vector		vSourScale, vSourRotation, vSourPosition;
			_vector		vDestScale, vDestRotation, vDestPosition;

			vSourScale = XMLoadFloat3(&KeyFrames[iCurrentKeyFrameIndex]->vScale);
			vSourRotation = XMLoadFloat4(&KeyFrames[iCurrentKeyFrameIndex]->vRotation);
			vSourPosition = XMLoadFloat3(&KeyFrames[iCurrentKeyFrameIndex]->vPosition);

			vDestScale = XMLoadFloat3(&KeyFrames[iCurrentKeyFrameIndex + 1]->vScale);
			vDestRotation = XMLoadFloat4(&KeyFrames[iCurrentKeyFrameIndex + 1]->vRotation);
			vDestPosition = XMLoadFloat3(&KeyFrames[iCurrentKeyFrameIndex + 1]->vPosition);

			vScale = XMVectorLerp(vSourScale, vDestScale, fRatio);
			vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio);
			vPosition = XMVectorLerp(vSourPosition, vDestPosition, fRatio);
			vPosition = XMVectorSetW(vPosition, 1.f);
		}
		_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

		m_Channels[i]->SetTransformMatrix(TransformationMatrix);*/
	}

	return S_OK;
}

HRESULT Engine::Animation::AddChannel(AnimChannel * pChannel)
{
	m_Channels.push_back(pChannel);
	return S_OK;
}

Animation * Engine::Animation::Create(char* pName, _double Duration, _double PlaySpeed)
{
	Animation* pInstance = new Animation();
	if (FAILED(pInstance->Ready(pName, Duration, PlaySpeed)))
	{
		MSGBOX("Failed to Creating Animation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void Engine::Animation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);
	m_Channels.clear();
}