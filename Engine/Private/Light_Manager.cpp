#include "..\Public\Light_Manager.h"
#include "Light.h"
#include "GameObject.h"

CLight_Manager::CLight_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext)
{
}

const LIGHT_DESC* CLight_Manager::Get_LightDesc(_uint iIndex) const
{
	auto	iter = m_Lights.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	if (iter == m_Lights.end())
		return nullptr;

	return (*iter)->Get_LightDesc();
}

HRESULT CLight_Manager::Initialize()
{
	return S_OK;
}

HRESULT CLight_Manager::TickEngine(_float fTimeDelta)
{
	LightDeleteCheck();

	for (auto& pLight : m_Lights) {
		if (nullptr != pLight) {
			pLight->PriorityTick(fTimeDelta);
			pLight->Tick(fTimeDelta);
			pLight->LateTick(fTimeDelta);
		}
	}

	return S_OK;
}

HRESULT CLight_Manager::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	m_iLightCount = 0;

	for (auto& pLight : m_Lights) {
		if (nullptr != pLight) {
			if (pLight->Get_LightState()) {
				m_iLightCount++;
				pLight->Lighting(pShader, pVIBuffer);
			}
		}
	}
	return S_OK;
}

void CLight_Manager::LightControl(string strLightTag, _bool bOn)
{
	for (auto& pLight : m_Lights)
	{
		if (nullptr != pLight) {
			if (strLightTag == pLight->Get_LightDesc()->szName)
				pLight->Set_LightState(bOn);
		}
	}
}

void CLight_Manager::LightControl(string strLightTag)
{
	for (auto& pLight : m_Lights)
	{
		if (nullptr != pLight) {
			if (strLightTag == pLight->Get_LightDesc()->szName)
				pLight->Set_LightState(!pLight->Get_LightState());
		}
	}
}

void CLight_Manager::LightClear()
{
	for (auto& pLight : m_Lights)
	{
		if (nullptr != pLight) {
			pLight->Set_IsDead(true);
		}
	}
}

void CLight_Manager::LightDeleteCheck()
{
	//for (auto& pLight : m_Lights)
	//{
	//	if (nullptr != pLight) {
	//		if (pLight->Get_IsDead())
	//			Safe_Release(pLight);
	//	}
	//}

	for (auto pDelete = m_Lights.begin(); pDelete != m_Lights.end();)
	{
		if ((*pDelete)->Get_IsDead()) {
			Safe_Release(*pDelete);
			pDelete = m_Lights.erase(pDelete);
		}
		else
			++pDelete;
	}
}

HRESULT CLight_Manager::Add_Light(void* LightDesc)
{
	CLight* pLight = CLight::Create(m_pDevice,m_pContext,LightDesc);
	if (nullptr == pLight)
		return E_FAIL;

	m_Lights.emplace_back(pLight);

	return S_OK;
}

CLight* CLight_Manager::Add_Light_Return(void* LightDesc)
{
	CLight* pLight = CLight::Create(m_pDevice, m_pContext, LightDesc);
	if (nullptr == pLight)
		assert(false);

	m_Lights.emplace_back(pLight);

	return pLight;
}

CLight_Manager* CLight_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLight_Manager* pInstance = new CLight_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLight_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLight_Manager::Free()
{
	__super::Free();

	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	m_Lights.clear();
}
