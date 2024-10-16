#pragma once

#include "Base.h"

BEGIN(Engine)

class CMap_Manager : public CBase
{

private:
	CMap_Manager();
	virtual ~CMap_Manager() = default;

public:
	void Initialize();

	void Make_Map();
	void Save_Map();
	void Load_Map();

public:
	static CMap_Manager* Create();
	virtual void Free() override;
};

END