#include "ResourceContainer.h"
#include "Texture.h"

CResource_Container::CResource_Container(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }, m_pContext { pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

void CResource_Container::Initialize()
{

}

void CResource_Container::Tick(_float fTimeDelta)
{

}

void CResource_Container::Clear()
{

}

void CResource_Container::Render()
{

}

CTexture* CResource_Container::SearchMaterial(const _wstring& pMaterialTag)
{
	auto iter = m_MaterialList.find(pMaterialTag);

	if(iter == m_MaterialList.end())
	{
		CTexture* pMaterial = CTexture::Create(m_pDevice, m_pContext, pMaterialTag);

		m_MaterialList.insert(make_pair(pMaterialTag, pMaterial));
		return pMaterial;
	}
	else {
		return iter->second;
	}
}

CResource_Container* CResource_Container::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CResource_Container* pInstance = new CResource_Container(pDevice,pContext);

	pInstance->Initialize();

	return pInstance;
}

void CResource_Container::Free()
{

	/*for (auto& Pair : m_MaterialList)
		Safe_Release(Pair.second);

	m_MaterialList.clear();*/

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

}
