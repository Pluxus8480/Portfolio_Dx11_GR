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

	/* aiProcess_PreTransformVertices : 내 모델은 여러개의 메시(칼, 몸, 가방, 모닝스타, 그림자)로 구분되어있어. */
	/* 이 각각의 메시를 배치되야할 상태대로 다 미리 셋팅하여 로드한다. */
	_uint			iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	if (eType == MODEL_TYPE::NONANIM)
		iFlag |= aiProcess_PreTransformVertices;

	Init_Model(pModelFilePath, "", 0);

	///* 모든 모델의 정보를 다 읽어서 aiScene안에 저장해둔 것이다. */
	//m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	//if (nullptr == m_pAIScene)
	//	return E_FAIL;


	//XMStoreFloat4x4(&m_TransformMatrix, TransformMatrix);

	///* aiScene에 있는 정보를 내가 활용하기 좋도록 수정 정리 보관하는 작업을 수행하자. */
	//if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
	//	return E_FAIL;

	///* 이 모델을 구성하는 각각의 메시를 로드하자. */
	///* 애님 모델일 경우, 현재 메시가 사용하는 뼈의 인덱스를 메시에 보관한다. */
	//if (FAILED(Ready_Meshes()))
	//	return E_FAIL;

	///* 빛 정보 <-> 정점의 노말, (Material) 픽셀당 무슨 색을 반사해야할지?! 픽셀당 환경광원을 어떻게 설정할지? 픽셀당 스펙큘러 연산을 어떻게해야할지?! */
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
	/* old : 모든 메시를 순회하면서 각 메시의 렌더함수를 호출한다. */
	/* Curr : 메시의 순회를 클라이언트에서 수행을 하고 지정한 메시에 대해서만 렌더함수를 호출한다.  */
	/* 어떤 머테리얼(Diffuse Ambient)을 어떤 쉐이더에 어떤 변수에 전달할 건지를 결정을 해야하는 작업을 수행해야했다. */
	/* 어떤 머테리얼인지가 메시마다 다르다. CMesh::m_iMaterialIndex */

	//for (size_t i = 0; i < m_iNumMeshes; i++)
	//{
	//	/* 쉐이더에 텍스쳐를 던지느 ㄴ작업을 클라이언트에서 처리하기위해 위의 루프자체를 클라로 옮긴것이다. */
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
	/* 1. 이 애니메이션이 사용하는 뼈들을 현재 시간에 맞는 애니메이션의 상태대로 뼈의 상태(TransformationMatrix)를 갱신해준다. */
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
	/* 2. 모든 뼈를 다시 순회하면서 CombinedTransformationMatrix를 갱신해주낟. */
	/*CombinedTransformationMatrix = 나의 TransformationMatrix * 부모의 CombinedTransformationMatrix */
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
	/* 쉐이더에 던져야 할 행렬 배열정보. */
	_float4x4	BoneMatrices[512];

	/* 행렬배열은 메시에게 영향을 주는 뼈들의 행렬만 가져야하므로 메시에게 저장좀해줘! */
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
	/* 메시의 갯수를 저장하낟. */
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
	/* 모델에는 여러개의 머테리얼이 존재하낟. */
	/* 각 머테리얼마다 다양한 재질(Diffuse, Ambient,Specular...)을 텍스쳐로 표현한다. */
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		//Matthreads.emplace_back(&CModel::Material_Array, this, pModelFilePath, (int)i);
		aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];

		MATERIAL_MESH			MeshMaterial = {};

		for (size_t j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			/* 얻어온 경로에서 신용할 수 있는 건 파일의 이름과 확장자. */
			aiString		strTexturePath;

			/* 내가 지정한 텍스쳐의 경로를 얻어주낟.*/
			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strTexturePath)))
				continue;

			_char		szFullPath[MAX_PATH] = {};
			/* 모델파일경로로부터 뜯어온 문자열. */
			_char		szDrive[MAX_PATH] = {};
			_char		szDir[MAX_PATH] = {};

			/* 모델파일(aiScene)로부터 뜯어온 문자열. */
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
		/* 모델파일경로로부터 뜯어온 문자열. */
		_char		szDrive[MAX_PATH] = {};
		_char		szDir[MAX_PATH] = {};

		/* 모델파일(aiScene)로부터 뜯어온 문자열. */
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

	/* 모델에는 여러개의 머테리얼이 존재하낟. */
	/* 각 머테리얼마다 다양한 재질(Diffuse, Ambient,Specular...)을 텍스쳐로 표현한다. */
	m_iNumMaterials = m_pModelData->iNumMaterials;

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		//Matthreads.emplace_back(&CModel::Material_Array, this, pModelFilePath, (int)i);
		MATERIAL_DATA* pMaterial = m_pModelData->Material_Datas[i].get();

		MATERIAL_MESH			MeshMaterial = {};

		for (size_t j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
		{

			_char		szFullPath[MAX_PATH] = {};
			/* 모델파일경로로부터 뜯어온 문자열. */
			_char		szDrive[MAX_PATH] = {};
			_char		szDir[MAX_PATH] = {};

			/* 모델파일(aiScene)로부터 뜯어온 문자열. */
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
