#pragma once

#include "Base.h"

/* �������� Ÿ�̸Ӹ� �±�(Key)�� �����Ͽ� �����Ѵ�. */
/* Ư�� Ÿ�̸Ӹ� ����ϰ� �������ش�. */

BEGIN(Engine)

class CTimer_Manager final : public CBase
{
private:
	CTimer_Manager();
	virtual ~CTimer_Manager() = default;

public:
	_float	Get_TimeDelta(const wstring& strTimerTag);

public:
	HRESULT Add_Timer(const wstring& strTimerTag);
	void Compute_TimeDelta(const wstring& strTimerTag);
	
	void Set_TimeMultiplier(_float fTimeMultiplier) { 
		m_fTimeMultiplier = fTimeMultiplier; 
		if (m_fTimeMultiplier > m_fTimeMultiplier_Max)
			m_fTimeMultiplier = m_fTimeMultiplier_Max;

		if (m_fTimeMultiplier < m_fTimeMultiplier_Min)
			m_fTimeMultiplier = m_fTimeMultiplier_Min;
	}
	_float Get_TimeMultiplier() { return m_fTimeMultiplier;}

private:
	map<const wstring, class CTimer*>			m_Timers;
	
	_float m_fTimeMultiplier = 1.f;
	_float m_fTimeMultiplier_Min = 0.f;
	_float m_fTimeMultiplier_Max = 1.f;
private:
	class CTimer* Find_Timer(const wstring& strTimerTag);

public:
	static CTimer_Manager* Create();
	virtual void Free() override;
};

END