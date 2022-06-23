#include "..\public\ObjectMgr.h"
#include "GameObject.h"
#include "Layer.h"

IMPLEMENT_SINGLETON(ObjectMgr)

ObjectMgr::ObjectMgr()
{
}

Component * Engine::ObjectMgr::GetComponent(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iIndex)
{
	Layer*		pLayer = FindLayer(iLevelIndex, pLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->GetComponent(pComponentTag, iIndex);
}

list<class GameObject*> Engine::ObjectMgr::GetGameObjectsToLayer(_uint iLevelIndex, const _tchar * pLayerTag)
{
	Layer*		pLayer = FindLayer(iLevelIndex, pLayerTag);
	if (nullptr == pLayer)
		return list<class GameObject*>();

	return pLayer->GetGameObjects();
}

HRESULT Engine::ObjectMgr::ReserveManager(_uint iNumScenes)
{
	if (nullptr != m_pLayers)
		return E_FAIL;

	m_pLayers = new LAYERS[iNumScenes];
	m_iNumScenes = iNumScenes;

	return S_OK;
}

HRESULT Engine::ObjectMgr::AddPrototype(const _tchar * pPrototypeTag, GameObject * pPrototype)
{
	if (pPrototype == nullptr ||
		nullptr != FindPrototype(pPrototypeTag))
		return E_FAIL;

	m_Prototype.insert(PROTOTYPES::value_type(pPrototypeTag, pPrototype));

	return S_OK;
}

HRESULT Engine::ObjectMgr::AddGameObjectToLayer(_uint iSceneIndex, const _tchar * pLayerTag, const _tchar * pPrototypeTag, void * pArg, GameObject** pOut)
{
	if (iSceneIndex >= m_iNumScenes)
		return E_FAIL;

	// 원형 존재 확인
	GameObject*	pPrototype = FindPrototype(pPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	// 복사본 생성
	GameObject*	pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return E_FAIL;

	//레이어 존재 확인
	auto	iter = find_if(m_pLayers[iSceneIndex].begin(), m_pLayers[iSceneIndex].end(), Tag_Finder(pLayerTag));

	//레이어가 없는 경우 함께 추가
	if (iter == m_pLayers[iSceneIndex].end())
	{
		Layer*		pLayer = Layer::Create();
		if (nullptr == pLayer)
			return E_FAIL;

		if (FAILED(pLayer->AddGameObject(pGameObject)))
			return E_FAIL;

		m_pLayers[iSceneIndex].insert(LAYERS::value_type(pLayerTag, pLayer));
	}
	else //있는 경우 존재하는 레이어에 객체만 추가 
		iter->second->AddGameObject(pGameObject);

	if(pOut != nullptr)
		*pOut = pGameObject;

	return S_OK;
}

_int Engine::ObjectMgr::Update(_double TimeDelta)
{
	_int		iProgress = 0;

	for (_uint i = 0; i < m_iNumScenes; ++i)
	{
		for (auto& Pair : m_pLayers[i]) 
		{
			iProgress = Pair.second->Update(TimeDelta);

			if (0 > iProgress)
				return -1;
			else if (iProgress == 1)
				return _int();
		}
	}

	return _int();
}

_int Engine::ObjectMgr::LateUpdate(_double TimeDelta)
{
	_int		iProgress = 0;

	for (_uint i = 0; i < m_iNumScenes; ++i)
	{
		for (auto& Pair : m_pLayers[i])
			iProgress = Pair.second->LateUpdate(TimeDelta);

		if (0 > iProgress)
			return -1;
	}

	return _int();
}

HRESULT Engine::ObjectMgr::ClearLayer(_uint iScenelIndex)
{
	if (iScenelIndex > m_iNumScenes)
		return E_FAIL;

	for (auto& Pair : m_pLayers[iScenelIndex])
		Safe_Release(Pair.second);

	m_pLayers[iScenelIndex].clear();

	return S_OK;
}

GameObject * Engine::ObjectMgr::FindPrototype(const _tchar * pPrototypeTag)
{
	auto	iter = find_if(m_Prototype.begin(), m_Prototype.end(), Tag_Finder(pPrototypeTag));
	if (iter == m_Prototype.end())
		return nullptr;

	return iter->second;
}

Layer * Engine::ObjectMgr::FindLayer(_uint iLevelIndex, const _tchar * pLayerTag)
{
	auto	iter = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), Tag_Finder(pLayerTag));
	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void Engine::ObjectMgr::Free()
{
	for (_uint i = 0; i < m_iNumScenes; ++i)
	{
		for (auto& Pair : m_pLayers[i])
			Safe_Release(Pair.second);

		m_pLayers[i].clear();
	}
	Safe_Delete_Array(m_pLayers);

	for (auto& Pair : m_Prototype)
		Safe_Release(Pair.second);

	m_Prototype.clear();
}
