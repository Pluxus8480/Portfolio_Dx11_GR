#include "..\Public\Layer.h"
#include "GameObject.h"

CLayer::CLayer()
{
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects.emplace_back(pGameObject);

	return S_OK;
}

void CLayer::PriorityTick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->PriorityTick(fTimeDelta);
	}
}

void CLayer::Tick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Tick(fTimeDelta);
	}
}

void CLayer::LateTick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->LateTick(fTimeDelta);
	}
}

void CLayer::DeleteCheck()
{

	for (auto pDelete = m_GameObjects.begin(); pDelete != m_GameObjects.end();)
	{
		if ((*pDelete)->Get_IsDead()) {
			Safe_Release(*pDelete);
			pDelete = m_GameObjects.erase(pDelete);
		}
		else
			++pDelete;
	}
}

void CLayer::Prepare_Clear()
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Set_IsDead(true);
	}
}

CComponent * CLayer::Find_Component(const wstring & strComponentTag, _uint iIndex)
{
	if (iIndex >= m_GameObjects.size())
		return nullptr;

	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	if (iter == m_GameObjects.end())
		return nullptr;

	return (*iter)->Find_Component(strComponentTag);	
}

CGameObject* CLayer::Find_GameObject(_uint iIndex)
{
	if (iIndex >= m_GameObjects.size())
		return nullptr;

	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	if (iter == m_GameObjects.end())
		return nullptr;

	return *iter;
}

CLayer * CLayer::Create()
{
	return new CLayer();
}

void CLayer::Free()
{
	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);

	m_GameObjects.clear();
	
}
