#pragma once

#include "VIBuffer.h"
#include "Model.h"

BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}
	PxTriangleMesh* Get_TriangleMesh() {
		return m_pTriangleMesh;
	}

	void Cooking_TriangleMesh();

public:
	virtual HRESULT Initialize_Prototype(MODEL_TYPE eModelType, const aiMesh* pAIMesh, vector<class CBone*>& Bones, _fmatrix TransformMatrix);
	virtual HRESULT Initialize_Prototype(MODEL_TYPE eModelType, shared_ptr<MESH_DATA> pAIMesh, vector<class CBone*>& Bones, _fmatrix TransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;

	void SetUp_BoneMatrices(_float4x4* pMatrices, const vector<class CBone*>& Bones);

private:

	_char			m_szName[MAX_PATH] = {};
	_uint			m_iMaterialIndex = {};
	_uint			m_iNumBones = {};

	/* 모델 전체의 뼈들 중, 이 메시에 영향을 주는 뼈의 인덱스를 저장했다. */
	vector<_uint>	m_Bones;
	vector<_float4x4>	m_OffsetMatrices;

	vector<PxVec3> vertices;
	vector<PxU32> indices;

	PxTriangleMesh* m_pTriangleMesh = nullptr;

private:
	HRESULT Ready_VIBuffer_For_NonAnim(const aiMesh* pAIMesh, _fmatrix TransformMatrix);
	HRESULT Ready_VIBuffer_For_Anim(const aiMesh* pAIMesh, vector<class CBone*>& Bones);

	HRESULT Ready_VIBuffer_For_NonAnim(shared_ptr<MESH_DATA> pAIMesh, _fmatrix TransformMatrix);
	HRESULT Ready_VIBuffer_For_Anim(shared_ptr<MESH_DATA> pAIMesh, vector<class CBone*>& Bones);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL_TYPE eModelType, const aiMesh* pAIMesh, vector<class CBone*>& Bones, _fmatrix TransformMatrix);
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL_TYPE eModelType, shared_ptr<MESH_DATA> pAIMesh, vector<class CBone*>& Bones, _fmatrix TransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END