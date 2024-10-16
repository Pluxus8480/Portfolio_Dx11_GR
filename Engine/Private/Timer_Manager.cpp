#include "..\Public\Timer_Manager.h"
#include "Timer.h"

CTimer_Manager::CTimer_Manager()
{
}

_float CTimer_Manager::Get_TimeDelta(const wstring & strTimerTag)
{
	CTimer*	pTimer = Find_Timer(strTimerTag);

	if (nullptr == pTimer)
		return 0.f;

	return pTimer->Get_TimeDelta();
}

HRESULT CTimer_Manager::Add_Timer(const wstring & strTimerTag)
{
	if (nullptr != Find_Timer(strTimerTag))
		return E_FAIL;

	CTimer*		pTimer = CTimer::Create();
	if (nullptr == pTimer)
		return E_FAIL;

	m_Timers.emplace(strTimerTag, pTimer);	

	return S_OK;
}

void CTimer_Manager::Compute_TimeDelta(const wstring & strTimerTag)
{
	CTimer*	pTimer = Find_Timer(strTimerTag);

	if (nullptr == pTimer)
		return;

	pTimer->Update();
}

CTimer * CTimer_Manager::Find_Timer(const wstring & strTimerTag)
{
	auto	iter = m_Timers.find(strTimerTag);
	if (iter == m_Timers.end())
		return nullptr;	

	return iter->second;	
}

CTimer_Manager * CTimer_Manager::Create()
{
	return new CTimer_Manager();
}

void CTimer_Manager::Free()
{
	for (auto& Pair : m_Timers)	
		Safe_Release(Pair.second);

	m_Timers.clear();	
}
