#include "Ui_Manager.h"
#include "Ui.h"


CUi_Manager::CUi_Manager(ENGINE_DESC sEngine_Desc, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:m_iWinSizeX{ sEngine_Desc.iWinSizeX },
	m_iWinSizeY{ sEngine_Desc.iWinSizeY },
	m_pDevice{ pDevice },
	m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CUi_Manager::Add_Ui_LifePrototype(const wstring& Ui_LifePrototypeTag, CUi* Ui_LifePrototype)
{
	if (m_Ui_LifePrototypes.end() != m_Ui_LifePrototypes.find(Ui_LifePrototypeTag))
		return E_FAIL;

	m_Ui_LifePrototypes.emplace(Ui_LifePrototypeTag, Ui_LifePrototype);

	return S_OK;
}

HRESULT CUi_Manager::Add_Ui_Active(const wstring& Ui_ActiveTag, eUiRenderType UiRenderType, CUi* Ui_Active)
{
	if (UiRenderType == eUiRenderType::Render_NonBlend)
	{
		if (m_Ui_Active.end() != m_Ui_Active.find(Ui_ActiveTag))
			return E_FAIL;

		m_Ui_Active.emplace(Ui_ActiveTag, Ui_Active);
	}
	else
	{
		if (m_Ui_ActiveBlend.end() != m_Ui_ActiveBlend.find(Ui_ActiveTag))
			return E_FAIL;

		m_Ui_ActiveBlend.emplace(Ui_ActiveTag, Ui_Active);
	}
	return S_OK;
}

CUi* CUi_Manager::Get_ActiveUi_TagSearch(const wstring& Ui_ActiveTag)
{
	if (m_Ui_Active.end() != m_Ui_Active.find(Ui_ActiveTag))
		return m_Ui_Active[Ui_ActiveTag];

	if (m_Ui_ActiveBlend.end() != m_Ui_ActiveBlend.find(Ui_ActiveTag))
		return m_Ui_ActiveBlend[Ui_ActiveTag];

	return nullptr;
}



HRESULT CUi_Manager::Add_Ui_LifeClone(const wstring& Ui_LifePrototypeTag, eUiRenderType UiRenderType, void* pArg)
{
	auto pLife = m_Ui_LifePrototypes.find(Ui_LifePrototypeTag);
	if (m_Ui_LifePrototypes.end() == pLife)
		return E_FAIL;


	CUi* pUi_Life = (pLife->second)->Clone(pArg);
	if (nullptr == pUi_Life)
		return E_FAIL;


	if (UiRenderType == eUiRenderType::Render_NonBlend)
	{
		m_Ui_Lifelist.emplace_back(pUi_Life);
	}


	else if (UiRenderType == eUiRenderType::Render_Blend)
	{
		m_Ui_LifeBlendlist.emplace_back(pUi_Life);
	}


	return S_OK;
}

void CUi_Manager::PriorityTick(_float fTimeDelta)
{
	auto iter = m_Ui_Lifelist.begin();
	for (; iter != m_Ui_Lifelist.end(); )
	{
		if ((*iter)->Is_Dead())
		{
			Safe_Release(*iter);
			iter = m_Ui_Lifelist.erase(iter);
		}
		else
		{
			(*iter)->PriorityTick(fTimeDelta);
			++iter;
		}
	}

	auto Blenditer = m_Ui_LifeBlendlist.begin();
	for (; Blenditer != m_Ui_LifeBlendlist.end(); )
	{
		if ((*Blenditer)->Is_Dead())
		{
			Safe_Release(*Blenditer);
			Blenditer = m_Ui_LifeBlendlist.erase(Blenditer);
		}
		else
		{
			(*Blenditer)->PriorityTick(fTimeDelta);
			++Blenditer;
		}
	}

	for (auto& iter : m_Ui_Active) {
		if (iter.second->Get_Active())
			iter.second->PriorityTick(fTimeDelta);
	}

	for (auto& iter : m_Ui_ActiveBlend) {
		if (iter.second->Get_Active())
			iter.second->PriorityTick(fTimeDelta);
	}
}

void CUi_Manager::Tick(_float fTimeDelta)
{
	auto iter = m_Ui_Lifelist.begin();
	for (; iter != m_Ui_Lifelist.end(); )
	{
		if ((*iter)->Is_Dead())
		{
			Safe_Release(*iter);
			iter = m_Ui_Lifelist.erase(iter);
		}
		else
		{
			(*iter)->Tick(fTimeDelta);
			++iter;
		}
	}

	auto Blenditer = m_Ui_LifeBlendlist.begin();
	for (; Blenditer != m_Ui_LifeBlendlist.end(); )
	{
		if ((*Blenditer)->Is_Dead())
		{
			Safe_Release(*Blenditer);
			Blenditer = m_Ui_LifeBlendlist.erase(Blenditer);
		}
		else
		{
			(*Blenditer)->Tick(fTimeDelta);
			++Blenditer;
		}
	}

	for (auto& iter : m_Ui_Active) {
		if (iter.second->Get_Active())
			iter.second->Tick(fTimeDelta);
	}

	for (auto& iter : m_Ui_ActiveBlend) {
		if (iter.second->Get_Active())
			iter.second->Tick(fTimeDelta);
	}
}

void CUi_Manager::LateTick(_float fTimeDelta)
{
	auto iter = m_Ui_Lifelist.begin();
	for (; iter != m_Ui_Lifelist.end(); )
	{
		if ((*iter)->Is_Dead())
		{
			Safe_Release(*iter);
			iter = m_Ui_Lifelist.erase(iter);
		}
		else
		{
			(*iter)->LateTick(fTimeDelta);
			++iter;
		}
	}

	auto Blenditer = m_Ui_LifeBlendlist.begin();
	for (; Blenditer != m_Ui_LifeBlendlist.end(); )
	{
		if ((*Blenditer)->Is_Dead())
		{
			Safe_Release(*Blenditer);
			Blenditer = m_Ui_LifeBlendlist.erase(Blenditer);
		}
		else
		{
			(*Blenditer)->LateTick(fTimeDelta);
			++Blenditer;
		}
	}
	for (auto& iter : m_Ui_Active) {
		if (iter.second->Get_Active())
			iter.second->LateTick(fTimeDelta);
	}

	for (auto& iter : m_Ui_ActiveBlend) {
		if (iter.second->Get_Active())
			iter.second->LateTick(fTimeDelta);
	}
}

void CUi_Manager::Ui_Render_Begin()
{
	XMStoreFloat4x4(&m_ViewMatrix, DirectX::XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, DirectX::XMMatrixIdentity());

	////D3DXMatrixOrthoLH(&m_ProjMatrix, (_float)m_iWinSizeX, (_float)m_iWinSizeY, 0.0f, 100.f);
	//////m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//////m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	//////m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 254);
	//////m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	//////m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);
	////m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
	////m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);
}

HRESULT CUi_Manager::Ui_Render()
{
	//for (auto& Clone : m_Ui_Lifelist)
	//	Clone->Render();

	//for (auto& iter : m_Ui_Active) {
	//	if (iter.second->Get_Active() == true)
	//		iter.second->Render();
	//}

	////m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	////m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	////m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND,
	////	D3DBLEND_SRCALPHA);
	////m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND,
	////	D3DBLEND_INVSRCALPHA);

	//for (auto& BlendClone : m_Ui_LifeBlendlist)
	//	BlendClone->Render();

	//for (auto& iter : m_Ui_ActiveBlend) {
	//	if (iter.second->Get_Active() == true)
	//		iter.second->Render();
	//}
	/*m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);*/
	return S_OK;
}

void CUi_Manager::Ui_Render_End()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}

void CUi_Manager::Set_WinSize(_uint iWinSizeX, _uint iWinSizeY)
{
	m_iWinSizeX = iWinSizeX;
	m_iWinSizeY = iWinSizeY;
}

void CUi_Manager::Set_Ui_ActiveState(const wstring& Ui_ActiveTag, bool _isActive)
{
	auto iter = m_Ui_Active.find(Ui_ActiveTag);

	if (m_Ui_Active.end() != iter)
	{
		iter->second->Set_Active(_isActive);
	}

	auto Blenditer = m_Ui_ActiveBlend.find(Ui_ActiveTag);
	if (m_Ui_ActiveBlend.end() != Blenditer)
	{
		Blenditer->second->Set_Active(_isActive);
	}

}

CUi* CUi_Manager::Get_ActiveBlendUI(const wstring& strKey)
{
	auto it = m_Ui_ActiveBlend.find(strKey);
	if (it == m_Ui_ActiveBlend.end())
		return nullptr;

	return it->second;
}

CUi* CUi_Manager::Get_ActiveNonBlendUI(const wstring& strKey)
{
	auto it = m_Ui_Active.find(strKey);
	if (it == m_Ui_Active.end())
		return nullptr;

	return it->second;
}

bool CUi_Manager::Get_Ui_ActiveState(const wstring& Ui_ActiveTag)
{
	auto iter = m_Ui_Active.find(Ui_ActiveTag);
	if (m_Ui_Active.end() != iter)
	{
		return iter->second->Get_Active();
	}

	auto Blenditer = m_Ui_ActiveBlend.find(Ui_ActiveTag);
	if (m_Ui_ActiveBlend.end() != Blenditer)
	{
		return Blenditer->second->Get_Active();
	}

	return false;
}

void CUi_Manager::Set_Ui_ActiveTextureIndex(const wstring& Ui_ActiveTag, int _iTextureIndex)
{
	auto iter = m_Ui_Active.find(Ui_ActiveTag);
	if (m_Ui_Active.end() != iter)
	{
		iter->second->Set_Texture_Index(_iTextureIndex);
	}

	auto Blenditer = m_Ui_ActiveBlend.find(Ui_ActiveTag);
	if (m_Ui_ActiveBlend.end() != Blenditer)
	{
		Blenditer->second->Set_Texture_Index(_iTextureIndex);
	}
}

int CUi_Manager::Get_Ui_ActiveTextureIndex(const wstring& Ui_ActiveTag)
{

	auto iter = m_Ui_Active.find(Ui_ActiveTag);
	if (m_Ui_Active.end() != iter)
	{
		return iter->second->Get_Texture_Index();
	}

	auto Blenditer = m_Ui_ActiveBlend.find(Ui_ActiveTag);
	if (m_Ui_ActiveBlend.end() != Blenditer)
	{
		return Blenditer->second->Get_Texture_Index();
	}

	return 0;
}

CUi_Manager* CUi_Manager::Create(ENGINE_DESC sEngine_Desc, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return new CUi_Manager(sEngine_Desc, pDevice, pContext);
}

void CUi_Manager::Free()
{
	for (auto& Clone : m_Ui_Lifelist)
		Safe_Release(Clone);
	m_Ui_Lifelist.clear();

	for (auto& BlendClone : m_Ui_LifeBlendlist)
		Safe_Release(BlendClone);
	m_Ui_LifeBlendlist.clear();


	for (auto& Pair : m_Ui_Active)
		Safe_Release(Pair.second);
	m_Ui_Active.clear();


	for (auto& Pair : m_Ui_ActiveBlend)
		Safe_Release(Pair.second);
	m_Ui_ActiveBlend.clear();

	for (auto& Pair : m_Ui_LifePrototypes)
		Safe_Release(Pair.second);
	m_Ui_LifePrototypes.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
