#include "..\public\Layer.h"
#include "GameObject.h"

Layer::Layer()
{
}

Component * Engine::Layer::GetComponent(const _tchar * pComponentTag, _uint iIndex)
{
	auto	iter = m_Objects.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->GetComponent(pComponentTag);
}

list<GameObject*> Engine::Layer::GetGameObjects()
{
	return m_Objects;
}

HRESULT Engine::Layer::AddGameObject(GameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_Objects.push_back(pGameObject);
	return S_OK;
}

_int Engine::Layer::Update(_double TimeDelta)
{
	_int		iProgress = 0;

	auto& iter = m_Objects.begin();
	while (iter != m_Objects.end())
	{
		if (nullptr != (*iter))
		{
			if ((*iter)->GetDestroy())
			{ 
				Safe_Release(*iter);
				iter = m_Objects.erase(iter);
				continue;
			}
			if ((*iter)->GetActive())
				iProgress = (*iter)->Update(TimeDelta);
		}
		if (0 > iProgress)
			return -1;
		else if (iProgress == 1)
			return _int(1);

		iter++;
	}

	/*for (auto& pGameObject : m_Objects)
	{

		if (nullptr != pGameObject)
		{
			if (pGameObject->GetDestroy())

				if (pGameObject->GetActive())
					iProgress = pGameObject->Update(TimeDelta);
		}
		if (0 > iProgress)
			return -1;
		else if (iProgress == 1)
			return _int(1);
	}*/
	return _int(0);
}

_int Engine::Layer::LateUpdate(_double TimeDelta)
{
	_int		iProgress = 0;

	for (auto& pGameObject : m_Objects)
	{
		if (nullptr != pGameObject&& pGameObject->GetActive())
			iProgress = pGameObject->LateUpdate(TimeDelta);

		if (0 > iProgress)
			return -1;
	}
	return _int(0);
}

Layer * Engine::Layer::Create()
{
	Layer*	pInstance = new Layer();

	return pInstance;
}

void Engine::Layer::Free()
{
	for (auto& pGameObject : m_Objects)
		Safe_Release(pGameObject);

	m_Objects.clear();
}
