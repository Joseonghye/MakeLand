#include "..\public\SceneMgr.h"
#include "Scene.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(SceneMgr)

SceneMgr::SceneMgr()
	: m_pCurScene(nullptr),m_iCurrentSceneID(0)
{
}

HRESULT Engine::SceneMgr::OpenScene(_uint iSceneID, _uint iNextID, Scene * pOpenScene)
{
	//if (m_pScenes.size() <= iSceneID)
	//{
	//	if (nullptr == pOpenScene)
	//		return E_FAIL;
	//	else
	//		m_pScenes.push_back(pOpenScene);
	//}
	
	if (nullptr == pOpenScene)
		return E_FAIL;

	if(FAILED(Clear()))
		return E_FAIL;

	Safe_Release(m_pCurScene);

	m_pCurScene = pOpenScene;
	m_iCurrentSceneID = iSceneID;
	//m_pScenes[m_iCurrentSceneID]->SetNextSceneID(iNextID);

	return S_OK;
}

_int Engine::SceneMgr::Update(_double TimeDelta)
{
	if (nullptr == m_pCurScene)
		return -1;

//	return 	m_pScenes[m_iCurrentSceneID]->Update(TimeDelta);
	return m_pCurScene->Update(TimeDelta);
}

HRESULT Engine::SceneMgr::Render()
{
	if (nullptr == m_pCurScene)
		return E_FAIL;

//	return 	m_pScenes[m_iCurrentSceneID]->Render();
	return m_pCurScene->Render();
}

HRESULT Engine::SceneMgr::Clear()
{
	GameInstance*		pGameInstance = GET_INSTANCE(GameInstance);

	if (FAILED(pGameInstance->Clear_Engine(m_iCurrentSceneID)))
		return E_FAIL;

	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

void Engine::SceneMgr::Free()
{
	Safe_Release(m_pCurScene);

	//for (int i = 0; i < m_pScenes.size(); ++i)
	//{
	//	Safe_Release(m_pScenes[m_iCurrentSceneID]);
	//}
	//m_pScenes.clear();
}
