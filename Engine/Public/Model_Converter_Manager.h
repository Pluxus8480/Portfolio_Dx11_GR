#pragma once

#include "Base.h"

BEGIN(Engine)

class CModel_Converter_Manager : public CBase
{

private:
	CModel_Converter_Manager();
	virtual ~CModel_Converter_Manager() = default;

public:
	void	Initialize();

public:
	static CModel_Converter_Manager* Create();
	virtual void Free() override;

public:
	//void LoadModel(const std::wstring& _strFilePath, const std::wstring& _strFileName);
	//void SaveModel(const std::wstring& _strFilePath, const std::wstring& _strFileName);
	void Create_MeshData(const MODEL_TYPE& In_eModelType, aiMesh* In_pAiMesh, const _fmatrix& In_TransformMatrix, MESH_VTX_INFO* In_pVertexInfo);

	void Bake_Binary(ofstream& os);

public:
	void Clear_MeshData() {	Safe_Delete( m_pMeshData); }
	void Init_MeshData() { m_pMeshData = new MESH_DATA; }
public:
	//void Read_Cache_list();
	//void Write_Cache_list();

public:
	MESH_DATA* GetMeshData() { return m_pMeshData; }

private:
	MESH_DATA* m_pMeshData;
	MESH_DATA m_MeshData;
};

END
