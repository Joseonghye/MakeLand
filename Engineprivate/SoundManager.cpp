#include "..\public\SoundManager.h"
#include <filesystem>

IMPLEMENT_SINGLETON(SoundManager)

SoundManager::SoundManager()
{

}

HRESULT Engine::SoundManager::Ready_SoundMgr()
{
	if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
		return E_FAIL;

	AUDIO_ENGINE_FLAGS flag = AudioEngine_Default;
#ifdef _DEBUG
	flag= flag | AudioEngine_Debug;
#endif // _DEBUG
	m_pAudioEngine = new AudioEngine(flag);

	if (FAILED(LoadSoundFile()))
		return E_FAIL;

	return S_OK;
}

_int Engine::SoundManager::Update_SoundMgr()
{
	if (!m_pAudioEngine->Update())
	{
		//재생장치 X
		if (m_pAudioEngine->IsCriticalError())
		{
			return -1;
		}
	}

	if (m_bStart)
	{
		m_fVolume += 0.03f;
		if (m_fVolume >= 0.1f)
			m_bStart = false;
		m_pBGM->SetVolume(m_fVolume);
	}
	if (m_bEnd)
	{
		m_fVolume -= 0.02f;
		if (m_fVolume <= 0.0f)
		{
			m_bEnd = false;
			m_pBGM->SetVolume(0.f);
			m_bRealEnd = true;
		}
	}


	return _int();
}

void Engine::SoundManager::PlayBGM(TCHAR * pSoundKey)
{
	map<_tchar*, DirectX::SoundEffect*>::iterator iter;
	iter = find_if(m_mapSound.begin(), m_mapSound.end(), Tag_Finder(pSoundKey));
	if (iter == m_mapSound.end())
		return;

	m_pBGM = iter->second->CreateInstance();

	m_pBGM->Play(true);
	m_pBGM->SetVolume(0.f);
	m_bStart = true;
}

void Engine::SoundManager::PlaySound(_tchar * pSoundKey)
{
	map<_tchar*, DirectX::SoundEffect*>::iterator iter;
	iter = find_if(m_mapSound.begin(), m_mapSound.end(), Tag_Finder(pSoundKey));
	if (iter == m_mapSound.end())
		return;

	iter->second->Play();
}

namespace fs = std::experimental::filesystem;
HRESULT Engine::SoundManager::LoadSoundFile()
{
	fs::path p("../bin/Resources/Sound");

	for (auto& file : fs::recursive_directory_iterator(p))
	{
		string strPath = file.path().u8string();
	
		int iLength = strlen(strPath.c_str()) + 1;
		_tchar* pSoundPath = new _tchar[iLength];
		ZeroMemory(pSoundPath, sizeof(_tchar) * iLength);
		MultiByteToWideChar(CP_ACP, 0, strPath.c_str(), iLength, pSoundPath, iLength);

		SoundEffect* soundEffect = new SoundEffect(m_pAudioEngine, pSoundPath);

		char name[_MAX_FNAME];
		_splitpath_s(strPath.c_str(), nullptr, 0, nullptr, 0, name, _MAX_FNAME, nullptr, 0);

		iLength = strlen(name) + 1;
		_tchar* pSoundKey = new _tchar[iLength];
		ZeroMemory(pSoundKey, sizeof(_tchar) * iLength);
		MultiByteToWideChar(CP_ACP, 0, name, iLength, pSoundKey, iLength);

		m_mapSound.emplace(pSoundKey, soundEffect);

	}
/*
	_finddata_t fd;
	long handle = _findfirst("../bin/Resources/Sound/*.wav", &fd);

	if (handle == -1)
		return E_FAIL;

	int iResult = 0;

	char szCurPath[128] = "../bin/Resources/Sound/";
	char szFullPath[128] = "";

	while (iResult != -1)
	{	
		strcpy_s(szFullPath, szCurPath);
		strcat_s(szFullPath, fd.name);

		int iLength = strlen(szFullPath) + 1;
		TCHAR* pSoundPath = new TCHAR[iLength];
		ZeroMemory(pSoundPath, sizeof(TCHAR) * iLength);
		MultiByteToWideChar(CP_ACP, 0, szFullPath, iLength, pSoundPath, iLength);

		SoundEffect* soundEffect = new SoundEffect(m_pAudioEngine, pSoundPath);

		iLength = strlen(fd.name) + 1;

		TCHAR* pSoundKey = new TCHAR[iLength];
		ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);
		MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pSoundKey, iLength);

		m_mapSound.emplace(pSoundKey, soundEffect);

		iResult = _findnext(handle, &fd);
	}

	_findclose(handle);
	*/

	//_tchar* pSoundPath = TEXT("../bin/Resources/Sound/A.wav");
	//_tchar* pSoundKey = TEXT("A_New_World_Fool");
	//SoundEffect* soundEffect = new SoundEffect(m_pAudioEngine, pSoundPath);
	//m_mapSound.emplace(pSoundKey, soundEffect);

	// pSoundPath = TEXT("../bin/Resources/Sound/sound01.wav");
	// pSoundKey = TEXT("sound01");
	//soundEffect = new SoundEffect(m_pAudioEngine, pSoundPath);
	//m_mapSound.emplace(pSoundKey, soundEffect);

	return S_OK;
}

void Engine::SoundManager::OnSuspending()
{
	m_bEnd = true;

	if(m_bRealEnd)
		m_pAudioEngine->Suspend();

}

void Engine::SoundManager::OnResuming()
{
	m_pAudioEngine->Resume();
}

void Engine::SoundManager::Free()
{
	if (m_pAudioEngine)
		m_pAudioEngine->Suspend();

	for (auto& Mypair : m_mapSound)
	{
		delete[] Mypair.first;
		//delete Mypair.second;
	}
	m_mapSound.clear();

}
