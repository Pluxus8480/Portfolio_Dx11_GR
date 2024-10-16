#include "Map_Manager.h"

CMap_Manager::CMap_Manager()
{

}

void CMap_Manager::Initialize()
{

}

CMap_Manager* CMap_Manager::Create()
{
	CMap_Manager* pInstance = new CMap_Manager();

	pInstance->Initialize();

	return pInstance;
}

void CMap_Manager::Free()
{
	__super::Free();
}
