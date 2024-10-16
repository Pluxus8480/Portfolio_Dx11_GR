#include "VIBuffer_TrailRect.h"
#include "Transform.h"

CVIBuffer_TrailRect::CVIBuffer_TrailRect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_TrailRect::CVIBuffer_TrailRect(const CVIBuffer_TrailRect& rhs)
	: CVIBuffer{ rhs }
{
}

HRESULT CVIBuffer_TrailRect::Initialize_Prototype(const TRAIL_DESC& desc)
{
	m_iMaxRect = desc.iMaxRect;
	m_iVertexStride = sizeof(VTXPOSTEX);
	m_iIndexStride = 2;
	m_iNumVertices = m_iMaxRect * 2 + 2;
	m_iNumIndices = m_iMaxRect * 6;
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// VERTEX_BUFFER 설정
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices / 2; ++i)
	{
		_float fTexc = 1.f - (_float)i / (_float)m_iMaxRect;
		pVertices[i * 2].vTexcoord = XMFLOAT2(fTexc, 0.f);
		pVertices[i * 2 + 1].vTexcoord = XMFLOAT2(fTexc, 1.f);
	}

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	// INDEX_BUFFER 설정
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);
	for (_uint i = 0; i < m_iMaxRect; ++i)
	{
		pIndices[i * 6 + 0] = 0 + i * 2;
		pIndices[i * 6 + 1] = 1 + i * 2;
		pIndices[i * 6 + 2] = 3 + i * 2;

		pIndices[i * 6 + 3] = 0 + i * 2;
		pIndices[i * 6 + 4] = 2 + i * 2;
		pIndices[i * 6 + 5] = 3 + i * 2;
	}

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	m_vecUpperPositions.resize(m_iMaxRect + 1);
	m_vecDownPositions.resize(m_iMaxRect + 1);

	return S_OK;
}

HRESULT CVIBuffer_TrailRect::Initialize(void* pArg)
{
	return S_OK;
}

void CVIBuffer_TrailRect::Start(_float4x4* pUpper, _float4x4* pDown)
{
	m_iStartingPoint = 0;
	m_bOn = true;
	m_bStart = true;
	m_fTimer = 0.f;
	m_pUpperTransform = pUpper;
	m_pDownTransform = pDown;
}

void CVIBuffer_TrailRect::Tick(_float fTimeDelta)
{
	if (!m_bOn)
		return;

	_float4x4 temp2 = *m_pUpperTransform;

	//temp2._42 -= 0.5f;

	// Load matrices into XMMATRIX
	XMMATRIX upperMatrix = XMLoadFloat4x4(&temp2);


	_float4x4 temp = *m_pDownTransform;

	//temp._42 += 0.8f;

	XMMATRIX downMatrix = XMLoadFloat4x4(&temp);



	// If m_pTarget->Get_WorldMatrix() returns an XMMATRIX or XMFLOAT4X4
	XMMATRIX worldMatrix = XMLoadFloat4x4(m_pTarget);

	// Multiply the world matrix with upper and down separately
	XMMATRIX upperResultMatrix = upperMatrix * worldMatrix;

	// Store the results back into _float4x4 matrices
	_float4x4 resultUpper;
	_float4x4 resultDown;

	XMStoreFloat4x4(&resultUpper, upperResultMatrix);
	XMStoreFloat4x4(&resultDown, downMatrix);

	m_fTimer += fTimeDelta;
	if (m_fTimer >= 0.016f)
	{
		m_fTimer -= 0.016f;
		++m_iStartingPoint;
		_float3 vUpperPos;
		vUpperPos.x = resultUpper._41;
		vUpperPos.y = resultUpper._42;
		vUpperPos.z = resultUpper._43;
		_float3 vDownPos;
		vDownPos.x = resultDown._41;
		vDownPos.y = resultDown._42;
		vDownPos.z = resultDown._43;
		if (m_iStartingPoint >= m_iMaxRect)
		{
			m_iStartingPoint = m_iMaxRect;
			m_vecUpperPositions.pop_front();
			m_vecDownPositions.pop_front();
			m_vecUpperPositions.push_back(vUpperPos);
			m_vecDownPositions.push_back(vDownPos);
		}
		else
		{
			m_vecUpperPositions[m_iStartingPoint] = vUpperPos;
			m_vecDownPositions[m_iStartingPoint] = vDownPos;
		}
		Update_Buffer();
	}

	if (m_bStart)
	{
		m_bStart = false;
		_float3 vUpperPos;
		vUpperPos.x = resultUpper._41;
		vUpperPos.y = resultUpper._42;
		vUpperPos.z = resultUpper._43;
		_float3 vDownPos;
		vDownPos.x = resultDown._41;
		vDownPos.y = resultDown._42;
		vDownPos.z = resultDown._43;
		for (_uint i = 0; i <= m_iMaxRect; ++i)
		{
			m_vecUpperPositions[i] = vUpperPos;
			m_vecDownPositions[i] = vDownPos;
		}
		SetUp_Buffer();
	}
}

void CVIBuffer_TrailRect::End()
{
	m_pUpperTransform = nullptr;
	m_pDownTransform = nullptr;
	m_bOn = false;
	m_bStart = false;
	m_fTimer = 0.f;
}

void CVIBuffer_TrailRect::Update_Buffer()
{
	D3D11_MAPPED_SUBRESOURCE SubResource{};

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPOSTEX* pVertices = static_cast<VTXPOSTEX*>(SubResource.pData);

	for (_int i = m_iStartingPoint; i >= 0; --i)
	{
		_uint idx = m_iStartingPoint - i;
		pVertices[idx * 2].vPosition = m_vecUpperPositions[i];
		pVertices[idx * 2 + 1].vPosition = m_vecDownPositions[i];
	}

	m_pContext->Unmap(m_pVB, 0);
}

void CVIBuffer_TrailRect::SetUp_Buffer()
{
	D3D11_MAPPED_SUBRESOURCE SubResource{};

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPOSTEX* pVertices = static_cast<VTXPOSTEX*>(SubResource.pData);

	for (_int i = m_iMaxRect; i >= 0; --i)
	{
		_uint idx = m_iMaxRect - i;
		pVertices[idx * 2].vPosition = m_vecUpperPositions[i];
		pVertices[idx * 2 + 1].vPosition = m_vecDownPositions[i];
	}

	m_pContext->Unmap(m_pVB, 0);
}

CVIBuffer_TrailRect* CVIBuffer_TrailRect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TRAIL_DESC& desc)
{
	CVIBuffer_TrailRect* pInstance = new CVIBuffer_TrailRect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(desc)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_TrailRect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_TrailRect::Clone(void* pArg)
{
	CVIBuffer_TrailRect* pInstance = new CVIBuffer_TrailRect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_TrailRect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_TrailRect::Free()
{
	__super::Free();
}