#include "..\Public\Model.h"
#include "Mesh.h"
#include "Bone.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CModel::CModel(const CModel& rhs)
	: CComponent{ rhs }
	, m_iNumMeshes{ rhs.m_iNumMeshes }
	, m_Meshes{ rhs.m_Meshes }
	, m_iNumMaterials{ rhs.m_iNumMaterials }
	, m_Materials{ rhs.m_Materials }
	/*, m_Bones{ rhs.m_Bones }*/
	, m_iNumAnimations{ rhs.m_iNumAnimations }
	/*, m_Animations {rhs.m_Animations }*/
	, m_TransformMatrix{ rhs.m_TransformMatrix }
	, m_eModelType{ rhs.m_eModelType }
{
	for (auto& pPrototypeAnimation : rhs.m_Animations)
		m_Animations.emplace_back(pPrototypeAnimation->Clone());


	for (auto& pPrototypeBone : rhs.m_Bones)
		m_Bones.emplace_back(pPrototypeBone->Clone());

	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);

	for (auto& MeshMaterial : m_Materials)
	{
		for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
			Safe_AddRef(MeshMaterial.pTextures[i]);
	}
}

HRESULT CModel::Initialize_Prototype(MODEL_TYPE eType, const _char* pModelFilePath, _fmatrix TransformMatrix)
{
	m_pModelData = make_shared<MODEL_DATA>();
	m_pModelData->Make_ModelData(pModelFilePath, eType, TransformMatrix);

	/* aiProcess_PreTransformVertices : �� ���� �������� �޽�(Į, ��, ����, ��׽�Ÿ, �׸���)�� ���еǾ��־�. */
	/* �� ������ �޽ø� ��ġ�Ǿ��� ���´�� �� �̸� �����Ͽ� �ε��Ѵ�. */
	_uint			iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	if (eType == MODEL_TYPE::NONANIM)
		iFlag |= aiProcess_PreTransformVertices;

	Init_Model(pModelFilePath, "", 0);

	///* ��� ���� ������ �� �о aiScene�ȿ� �����ص� ���̴�. */
	//m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	//if (nullptr == m_pAIScene)
	//	return E_FAIL;


	//XMStoreFloat4x4(&m_TransformMatrix, TransformMatrix);

	///* aiScene�� �ִ� ������ ���� Ȱ���ϱ� ������ ���� ���� �����ϴ� �۾��� ��������. */
	//if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
	//	return E_FAIL;

	///* �� ���� �����ϴ� ������ �޽ø� �ε�����. */
	///* �ִ� ���� ���, ���� �޽ð� ����ϴ� ���� �ε����� �޽ÿ� �����Ѵ�. */
	//if (FAILED(Ready_Meshes()))
	//	return E_FAIL;

	///* �� ���� <-> ������ �븻, (Material) �ȼ��� ���� ���� �ݻ��ؾ�����?! �ȼ��� ȯ�汤���� ��� ��������? �ȼ��� ����ŧ�� ������ ����ؾ�����?! */
	//if (FAILED(Ready_Materials(pModelFilePath)))
	//	return E_FAIL;

	//if (FAILED(Ready_Animations()))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{


	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	/* old : ��� �޽ø� ��ȸ�ϸ鼭 �� �޽��� �����Լ��� ȣ���Ѵ�. */
	/* Curr : �޽��� ��ȸ�� Ŭ���̾�Ʈ���� ������ �ϰ� ������ �޽ÿ� ���ؼ��� �����Լ��� ȣ���Ѵ�.  */
	/* � ���׸���(Diffuse Ambient)�� � ���̴��� � ������ ������ ������ ������ �ؾ��ϴ� �۾��� �����ؾ��ߴ�. */
	/* � ���׸��������� �޽ø��� �ٸ���. CMesh::m_iMaterialIndex */

	//for (size_t i = 0; i < m_iNumMeshes; i++)
	//{
	//	/* ���̴��� �ؽ��ĸ� ������ ���۾��� Ŭ���̾�Ʈ���� ó���ϱ����� ���� ������ü�� Ŭ��� �ű���̴�. */
	//	m_Materials[m_Meshes[i]->Get_MaterialIndex()].pTextures[aiTextureType_DIFFUSE]->Bind_ShaderResource();

	//	m_Meshes[i]->Render();
	//}

	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

const _float4x4* CModel::Get_BoneMatrixPtr(const _char* pBoneName) const
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
		{
			if (false == strcmp(pBone->Get_Name(), pBoneName))
				return true;

			return false;
		});

	if (iter == m_Bones.end())
		return nullptr;

	return (*iter)->Get_CombinedTransformationFloat4x4Ptr();
}

const _vector CModel::Get_BonePosition(const _char* pBoneName, _vector Pos) const
{
	/*auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
		{
			if (false == strcmp(pBone->Get_Name(), pBoneName))
				return true;

			return false;
		});

	if (iter == m_Bones.end())
		return;*/
	return _vector();
}

void CModel::Play_Animation(_float fTimeDelta)
{
	/* 1. �� �ִϸ��̼��� ����ϴ� ������ ���� �ð��� �´� �ִϸ��̼��� ���´�� ���� ����(TransformationMatrix)�� �������ش�. */
	if (m_eModelType == MODEL_TYPE::ANIM) {
		

		if (m_Animations.size() != 0) {

			if (m_Animations[m_iCurrentAnimIndex]->Get_Aperture() > 0.f) {
				m_Animations[m_iCurrentAnimIndex]->Update_Transformation_Aperture(m_Bones);

				m_Animations[m_iCurrentAnimIndex]->Set_Aperture(m_Animations[m_iCurrentAnimIndex]->Get_Aperture() - fTimeDelta * 10.f);
			}
			else {
				m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrix(fTimeDelta, m_Bones, m_isLoop);
				m_Animations[m_iCurrentAnimIndex]->Set_Aperture(0.f);
			}
		}
	}
	/* 2. ��� ���� �ٽ� ��ȸ�ϸ鼭 CombinedTransformationMatrix�� �������ֳ�. */
	/*CombinedTransformationMatrix = ���� TransformationMatrix * �θ��� CombinedTransformationMatrix */
	for (auto& pBone : m_Bones)
	{
		pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformMatrix));
	}

}

void CModel::Init_Model(const char* sModelKey, const string& szTexturePath, _uint iTimeScaleLayer)
{
	//Reset_Model();

	//m_pModelData = GAMEINSTANCE->Get_ModelFromKey(sModelKey);

	if (!m_pModelData)
		assert(false);

	m_szModelKey = sModelKey;

	Init_Model_Internal(m_pModelData, szTexturePath, iTimeScaleLayer);

}

HRESULT CModel::Bind_Material(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eMaterialType)
{
	if (m_Materials[m_Meshes[iMeshIndex]->Get_MaterialIndex()].pTextures[eMaterialType] == nullptr)
		return S_OK;

	return m_Materials[m_Meshes[iMeshIndex]->Get_MaterialIndex()].pTextures[eMaterialType]->Bind_ShaderResource(pShader, pConstantName);
}

void CModel::Init_Model_Internal(shared_ptr<MODEL_DATA> pModelData, const string& szTexturePath, _uint iTimeScaleLayer)
{
	std::vector<std::thread> threads;

	m_iNumMeshes = pModelData->iNumMeshs;
	m_eModelType = pModelData->eModelType;
	XMStoreFloat4x4(&m_TransformMatrix, XMMatrixScaling(0.01f,0.01f,0.01f));
	//XMStoreFloat4x4(&m_TransformMatrix, XMLoadFloat4x4(&pModelData->TransformMatrix));
	//XMStoreFloat4x4(&m_TransformMatrix, XMMatrixIdentity());

	if (szTexturePath.empty())
	{
		char szDir[MAX_PATH];
		_splitpath_s(m_pModelData->szModelFilePath.c_str(), nullptr, 0, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
		threads.emplace_back(&CModel::Create_Materials, this, szDir);
		//Create_Materials(szDir);
	}
	else
	{
		threads.emplace_back(&CModel::Create_Materials, this, szTexturePath.c_str());
		//Create_Materials(szTexturePath.c_str());
	}

	//threads.emplace_back(&CModel::Create_Bone, this, m_pModelData.get()->RootNode, 0);
	Create_Bone(m_pModelData.get()->RootNode, 0);

	/*for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, pModelData->Mesh_Datas[i], m_Bones, XMLoadFloat4x4(&m_TransformMatrix));
		if (nullptr == pMesh)
			return;

		m_Meshes.emplace_back(pMesh);
	}*/
	threads.emplace_back(&CModel::Mesh_Array, this, pModelData);

	//std::sort(m_pBoneNodes.begin(), m_pBoneNodes.end(), [](weak_ptr<CBoneNode> pSour, weak_ptr<CBoneNode> pDest)
	//	{
	//		return pSour.lock()->Get_Depth() < pDest.lock()->Get_Depth();
	//	});

	for (auto& th : threads) {
		if (th.joinable()) {
			th.join();
		}
	}

	//threads.emplace_back(&CModel::Create_Animations, this);
	Create_Animations();



}

HRESULT CModel::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
	/* ���̴��� ������ �� ��� �迭����. */
	_float4x4	BoneMatrices[512];

	/* ��Ĺ迭�� �޽ÿ��� ������ �ִ� ������ ��ĸ� �������ϹǷ� �޽ÿ��� ����������! */
	m_Meshes[iMeshIndex]->SetUp_BoneMatrices(BoneMatrices, m_Bones);

	return pShader->Bind_Matrices(pConstantName, BoneMatrices, 512);
}

void CModel::Reset_Model()
{
	m_pModelData.reset();
	m_szModelKey.clear();

	m_Meshes.clear();
	m_Materials.clear();
	m_Bones.clear();
	m_Animations.clear();

	m_iCurrentAnimIndex = 0;
	m_iNumAnimations = 0;
	m_iNumMaterials = 0;
}

HRESULT CModel::Ready_Meshes()
{
	/* �޽��� ������ �����ϳ�. */
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, m_pAIScene->mMeshes[i], m_Bones, XMLoadFloat4x4(&m_TransformMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.emplace_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const _char* pModelFilePath)
{
	//std::vector<std::thread> Matthreads;
	/* �𵨿��� �������� ���׸����� �����ϳ�. */
	/* �� ���׸��󸶴� �پ��� ����(Diffuse, Ambient,Specular...)�� �ؽ��ķ� ǥ���Ѵ�. */
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		//Matthreads.emplace_back(&CModel::Material_Array, this, pModelFilePath, (int)i);
		aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];

		MATERIAL_MESH			MeshMaterial = {};

		for (size_t j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			/* ���� ��ο��� �ſ��� �� �ִ� �� ������ �̸��� Ȯ����. */
			aiString		strTexturePath;

			/* ���� ������ �ؽ����� ��θ� ����ֳ�.*/
			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strTexturePath)))
				continue;

			_char		szFullPath[MAX_PATH] = {};
			/* �����ϰ�ηκ��� ���� ���ڿ�. */
			_char		szDrive[MAX_PATH] = {};
			_char		szDir[MAX_PATH] = {};

			/* ������(aiScene)�κ��� ���� ���ڿ�. */
			_char		szFileName[MAX_PATH] = {};
			_char		szExt[MAX_PATH] = {};

			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
			_splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			strcpy_s(szFullPath, szDrive);
			strcat_s(szFullPath, szDir);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);

			_tchar		szPerfectPath[MAX_PATH] = {};

			MultiByteToWideChar(CP_ACP, 0, szFullPath, (_int)strlen(szFullPath), szPerfectPath, MAX_PATH);

			MeshMaterial.pTextures[j] = CTexture::Create(m_pDevice, m_pContext, szPerfectPath, 1);
			if (nullptr == MeshMaterial.pTextures[j])
				return E_FAIL;
		}

		m_Materials.emplace_back(MeshMaterial);
	}

	/*for (auto& th : Matthreads) {
		if (th.joinable()) {
			th.join();
		}
	}*/

	return S_OK;
}

HRESULT CModel::Ready_Bones(const aiNode* pNode, _int iParentIndex)
{
	CBone* pBone = CBone::Create(pNode, iParentIndex);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.emplace_back(pBone);

	_uint		iNumChildren = pNode->mNumChildren;

	_int		iNumBones = (_int)m_Bones.size();

	for (size_t i = 0; i < iNumChildren; i++)
	{
		Ready_Bones(pNode->mChildren[i], iNumBones - 1);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], m_Bones);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.emplace_back(pAnimation);
	}

	return S_OK;
}

void CModel::Mesh_Array(shared_ptr<MODEL_DATA> pModelData)
{
	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, pModelData->Mesh_Datas[i], m_Bones, XMLoadFloat4x4(&m_TransformMatrix));
		if (nullptr == pMesh)
			return;

		m_Meshes.emplace_back(pMesh);
	}
}

void CModel::Material_Array(const _char* pModelFilePath, int i)
{
	unique_lock<shared_mutex> lock(m_Mtx);

	MATERIAL_DATA* pMaterial = m_pModelData->Material_Datas[i].get();

	MATERIAL_MESH			MeshMaterial = {};

	for (size_t j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
	{

		_char		szFullPath[MAX_PATH] = {};
		/* �����ϰ�ηκ��� ���� ���ڿ�. */
		_char		szDrive[MAX_PATH] = {};
		_char		szDir[MAX_PATH] = {};

		/* ������(aiScene)�κ��� ���� ���ڿ�. */
		_char		szFileName[MAX_PATH] = {};
		_char		szExt[MAX_PATH] = {};

		_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
		_splitpath_s(pMaterial->szTextureName[j].data(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

		if (szFileName[0] == '\0')
			continue;

		strcpy_s(szFullPath, szDrive);
		strcat_s(szFullPath, szDir);
		strcat_s(szFullPath, szFileName);
		strcat_s(szFullPath, szExt);

		_tchar		szPerfectPath[MAX_PATH] = {};

		MultiByteToWideChar(CP_ACP, 0, szFullPath, (_int)strlen(szFullPath), szPerfectPath, MAX_PATH);

		//MeshMaterial.pTextures[j] = CTexture::Create(m_pDevice, m_pContext, szPerfectPath, 1);
		MeshMaterial.pTextures[j] = GAMEINSTANCE->Auto_Search_Texture(szPerfectPath);

		if (nullptr == MeshMaterial.pTextures[j])
			continue;
	}
	m_Materials.emplace_back(MeshMaterial);

}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL_TYPE eType, const _char* pModelFilePath, _fmatrix TransformMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, TransformMatrix)))
	{
		MSG_BOX(TEXT("Failed to Created : CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);
	m_Animations.clear();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);

	m_Bones.clear();

	for (auto& MeshMaterial : m_Materials)
	{
		for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
			Safe_Release(MeshMaterial.pTextures[i]);
	}
	m_Materials.clear();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);

	m_Meshes.clear();
}

HRESULT CModel::Create_Materials(const _char* pModelFilePath)
{
	//unique_lock<shared_mutex> lock(m_MaterialMtx);

	vector<thread> Matthreads;

	/* �𵨿��� �������� ���׸����� �����ϳ�. */
	/* �� ���׸��󸶴� �پ��� ����(Diffuse, Ambient,Specular...)�� �ؽ��ķ� ǥ���Ѵ�. */
	m_iNumMaterials = m_pModelData->iNumMaterials;

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		//Matthreads.emplace_back(&CModel::Material_Array, this, pModelFilePath, (int)i);
		MATERIAL_DATA* pMaterial = m_pModelData->Material_Datas[i].get();

		MATERIAL_MESH			MeshMaterial = {};

		for (size_t j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
		{

			_char		szFullPath[MAX_PATH] = {};
			/* �����ϰ�ηκ��� ���� ���ڿ�. */
			_char		szDrive[MAX_PATH] = {};
			_char		szDir[MAX_PATH] = {};

			/* ������(aiScene)�κ��� ���� ���ڿ�. */
			_char		szFileName[MAX_PATH] = {};
			_char		szExt[MAX_PATH] = {};
			_char		strTga[] = ".tga";
			
			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
			_splitpath_s(pMaterial->szTextureName[j].data(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			if(szFileName[0] == '\0')
				continue;

			if(strcmp(szExt, strTga) == false) {
				strcpy_s(szExt, ".dds");
			}

			strcpy_s(szFullPath, szDrive);
			strcat_s(szFullPath, szDir);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);

			_tchar		szPerfectPath[MAX_PATH] = {};

			MultiByteToWideChar(CP_ACP, 0, szFullPath, (_int)strlen(szFullPath), szPerfectPath, MAX_PATH);

			//MeshMaterial.pTextures[j] = CTexture::Create(m_pDevice, m_pContext, szPerfectPath, 1);
			MeshMaterial.pTextures[j] = GAMEINSTANCE->Auto_Search_Texture(szPerfectPath);
			
			if (nullptr == MeshMaterial.pTextures[j])
				return E_FAIL;
		}

		m_Materials.emplace_back(MeshMaterial);
	}

	for (auto& th : Matthreads) {
		if (th.joinable()) {
			th.join();
		}
	}

	return S_OK;
}

HRESULT CModel::Bind_Buffers(_uint iMeshIndex)
{
	return m_Meshes[iMeshIndex]->Bind_Buffers();
}


HRESULT CModel::Create_Animations()
{
	m_iNumAnimations = m_pModelData->iNumAnimations;

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create(m_pModelData->Animation_Datas[i], m_Bones);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.emplace_back(pAnimation);
	}

	return S_OK;
}

HRESULT CModel::Create_Bone(shared_ptr<NODE_DATA> pNode, _int iParentIndex)
{
	CBone* pBone = CBone::Create(pNode, iParentIndex);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.emplace_back(pBone);

	_uint		iNumChildren = pNode->iNumChildren;

	_int		iNumBones = (_int)m_Bones.size();

	for (size_t i = 0; i < iNumChildren; i++)
	{
		Create_Bone(pNode->pChildren[i], iNumBones - 1);
	}

	return S_OK;


}
