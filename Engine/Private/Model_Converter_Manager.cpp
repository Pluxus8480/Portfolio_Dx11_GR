#include "Model_Converter_Manager.h"

CModel_Converter_Manager::CModel_Converter_Manager()
{

}


void CModel_Converter_Manager::Initialize()
{

}

CModel_Converter_Manager* CModel_Converter_Manager::Create()
{
	CModel_Converter_Manager* pInstance = new CModel_Converter_Manager();

	pInstance->Initialize();

	return pInstance;
}

void CModel_Converter_Manager::Free()
{
	__super::Free();
	Safe_Delete(m_pMeshData);
}

void CModel_Converter_Manager::Create_MeshData(const MODEL_TYPE& In_eModelType, aiMesh* In_pAiMesh, const _fmatrix& In_TransformMatrix, MESH_VTX_INFO* In_pVertexInfo)
{

	shared_ptr<MESH_DATA> pMeshData = make_shared<MESH_DATA>();

	if (FAILED(pMeshData->Make_MeshData(In_eModelType, In_pAiMesh, In_TransformMatrix, In_pVertexInfo)))
	{
		MSG_BOX(TEXT("Failed to Create MeshData"));
		return;
	}

}

void CModel_Converter_Manager::Bake_Binary(ofstream& os)
{
	m_pMeshData->Bake_Binary(os);
}
