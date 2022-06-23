#include "..\public\ComponentMgr.h"
#include "Component.h"

IMPLEMENT_SINGLETON(ComponentMgr)

Engine::ComponentMgr::ComponentMgr()
{
}

HRESULT Engine::ComponentMgr::ReserveManager(_uint iNumScenes)
{
	if (nullptr != m_pComponents)
		return E_FAIL;

	m_pComponents = new COMPONENTS[iNumScenes];

	m_iNumScene = iNumScenes;

	return S_OK;
}

HRESULT Engine::ComponentMgr::AddPrototype(_uint iSceneIndex, const _tchar * pPrototypeTag, Component * pPrototype)
{
	if (nullptr == pPrototype ||
		nullptr != FindPrototypeComponent(iSceneIndex, pPrototypeTag) ||
		iSceneIndex >= m_iNumScene)
		return E_FAIL;

	m_pComponents[iSceneIndex].insert(COMPONENTS::value_type(pPrototypeTag, pPrototype));

	return S_OK;
}

Component * Engine::ComponentMgr::CloneComponent(_uint iSceneIndex, const _tchar * pPrototypeTag, void * pArg)
{
	Component*	pPrototype = FindPrototypeComponent(iSceneIndex, pPrototypeTag);

	if (nullptr == pPrototype)
		return nullptr;

	Component*	pComponent = pPrototype->Clone(pArg);
	if (nullptr == pComponent)
		return nullptr;

	return pComponent;
}

Component * Engine::ComponentMgr::FindPrototypeComponent(_uint iSceneIndex,const _tchar * pComponentTag)
{
	auto	iter = find_if(m_pComponents[iSceneIndex].begin(), m_pComponents[iSceneIndex].end(), Tag_Finder(pComponentTag));
	if (iter == m_pComponents[iSceneIndex].end())
		return nullptr;

	return iter->second;
}

void Engine::ComponentMgr::Free()
{
	for (_uint i = 0; i < m_iNumScene; ++i)
	{
		for (auto& Pair : m_pComponents[i])
			Safe_Release(Pair.second);

		m_pComponents[i].clear();
	}

	Safe_Delete_Array(m_pComponents);
}
