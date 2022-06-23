#include "..\public\AnimChannel.h"

AnimChannel::AnimChannel()
	:m_iCurKeyFrameIndex(0)
{
}

HRESULT Engine::AnimChannel::Ready(const char * pName)
{
	strcpy_s(m_szName, pName);
	return S_OK;
}

void Engine::AnimChannel::UpdateTransformationMatrix(_double TrackPositionAcc)
{
	_vector	vScale, vRotation, vPosition;
	vScale = XMVectorZero();
	vRotation = XMVectorZero();
	vPosition = XMVectorZero();

	_uint iNumKeyFrame = m_KeyFrames.size();
	if (TrackPositionAcc < m_KeyFrames[0]->Time)
	{
		vScale = XMLoadFloat3(&m_KeyFrames[0]->vScale);
		vRotation = XMLoadFloat4(&m_KeyFrames[0]->vRotation);
		vPosition = XMLoadFloat3(&m_KeyFrames[0]->vPosition);
		vPosition = XMVectorSetW(vPosition, 1.f);
	}
	else if (TrackPositionAcc > m_KeyFrames[iNumKeyFrame-1]->Time)
	{
		vScale = XMLoadFloat3(&m_KeyFrames[iNumKeyFrame - 1]->vScale);
		vRotation = XMLoadFloat4(&m_KeyFrames[iNumKeyFrame - 1]->vRotation);
		vPosition = XMLoadFloat3(&m_KeyFrames[iNumKeyFrame - 1]->vPosition);
		vPosition = XMVectorSetW(vPosition, 1.f);
	}
	else
	{
		if (TrackPositionAcc >= m_KeyFrames[m_iCurKeyFrameIndex + 1]->Time)
			++m_iCurKeyFrameIndex;

		_float		fRatio = (TrackPositionAcc - m_KeyFrames[m_iCurKeyFrameIndex]->Time) /
			(m_KeyFrames[m_iCurKeyFrameIndex + 1]->Time - m_KeyFrames[m_iCurKeyFrameIndex]->Time);

		_vector		vSourScale, vSourRotation, vSourPosition;
		_vector		vDestScale, vDestRotation, vDestPosition;

		vSourScale = XMLoadFloat3(&m_KeyFrames[m_iCurKeyFrameIndex]->vScale);
		vSourRotation = XMLoadFloat4(&m_KeyFrames[m_iCurKeyFrameIndex]->vRotation);
		vSourPosition = XMLoadFloat3(&m_KeyFrames[m_iCurKeyFrameIndex]->vPosition);

		vDestScale = XMLoadFloat3(&m_KeyFrames[m_iCurKeyFrameIndex + 1]->vScale);
		vDestRotation = XMLoadFloat4(&m_KeyFrames[m_iCurKeyFrameIndex + 1]->vRotation);
		vDestPosition = XMLoadFloat3(&m_KeyFrames[m_iCurKeyFrameIndex + 1]->vPosition);

		vScale = XMVectorLerp(vSourScale, vDestScale, fRatio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio);
		vPosition = XMVectorLerp(vSourPosition, vDestPosition, fRatio);
		vPosition = XMVectorSetW(vPosition, 1.f);
	}
	XMStoreFloat4x4(&m_TransformationMatrix,XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition));


}

HRESULT Engine::AnimChannel::ReserveKeyFrames(_uint size)
{
	m_KeyFrames.reserve(size);
	return S_OK;
}

HRESULT Engine::AnimChannel::AddKeyFrame(KEYFRAME * pKeyFrame)
{
	m_KeyFrames.push_back(pKeyFrame);
	return S_OK;
}

AnimChannel * Engine::AnimChannel::Create(const char * pName)
{
	AnimChannel* pInstance = new AnimChannel();
	if (FAILED(pInstance->Ready(pName)))
	{
		MSGBOX("Failed to Creating AnimChannel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Engine::AnimChannel::Free()
{
	for (auto& pKeyFrame : m_KeyFrames)
		Safe_Delete(pKeyFrame);

	m_KeyFrames.clear();
}