# Portfolio_Dx11_GR

김민혁의 포트폴리오용 Repository 입니다.

리소스가 포함되어 있지 않아 제대로 작동하지 않을 가능성이 높으며, 코드 공개를 위한 Repository 입니다. 

참고하여 만든 작품은 GhostRunner 라는 작품입니다.

![image](https://github.com/user-attachments/assets/7b07148c-733c-4ab6-8c73-db9b03443951)

---

## 링크  
[Ghost Runner 프로젝트 데모 영상](https://www.youtube.com/watch?v=e-euy_yL7h0)

---

# Extend Library Dependancy

> PhysX 5.3  
> FMOD  
> Assimp 5.3.1  
> Imgui  
> Imguizmo
> Effects11  
> DirectXTK  
> DirectMath  

<hr/>

# DX11 모작 포트폴리오: Ghost Runner

- **프로젝트명:** DX11 모작 포트폴리오 [Ghost Runner]  
- **기간:** 2024.06 ~ 2024.08  
- **팀 프로젝트 여부:** X  
- **기술 스택:**  
  C, C++, STL, FMOD, Win API, OpenCV, DLL, DirectX 11, vcpkg, HLSL, Effects11, Assimp, ImGui, ImGuizmo, DirectXTK, DirectMath, Nvidia PhysX 5.3 , DLL
- **IDE:** Visual Studio  

---

## 목적 및 설명
DirectX 11을 이용해 제작하였고 FPS 와이어 액션 게임 *Ghost Runner*의 기본 조작 및 액션, 스테이지 구성, 보스 제작 등을 개발 전반을 진행한 개인 프로젝트입니다.  
상용 엔진을 사용하지 않고 엔진 부분을 직접 DLL로 작업하여 클라이언트와 엔진을 분리하는 방식으로 개발했습니다.

---

## 주요 구현 기능

### 1. **멀티스레딩을 통한 로딩 최적화**  
   - 게임 로딩 속도를 단축하기 위해 멀티스레딩 로드를 구현했습니다.  
   - 리소스를 별도의 스레드에서 로드하고, 메인 스레드가 초기화 작업을 병렬로 처리하도록 구성하여 로딩 시간을 크게 줄였습니다.

  <details>
    <summary>코드 구현 펼치기</summary>
  
```c++
 ----
 // Model.cpp 파일에서 모델 로딩에 사용하는 함수입니다.
  void CModel::Init_Model_Internal(shared_ptr<MODEL_DATA> pModelData, const string& szTexturePath, _uint iTimeScaleLayer)
{
  //스레드들을 일괄적으로 관리하는 벡터를 생성합니다.
	std::vector<std::thread> threads;

	m_iNumMeshes = pModelData->iNumMeshs;
	m_eModelType = pModelData->eModelType;
	XMStoreFloat4x4(&m_TransformMatrix, XMMatrixScaling(0.01f,0.01f,0.01f));

  
	if (szTexturePath.empty())
	{
  // 캐싱된 모델 데이터들은 호출 파라미터를 비우고 모델 데이터 안에 들어있는 경로 데이터를 직접 호출해서 스레드에 추가합니다. 

		char szDir[MAX_PATH];
		_splitpath_s(m_pModelData->szModelFilePath.c_str(), nullptr, 0, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
		threads.emplace_back(&CModel::Create_Materials, this, szDir);
	}
	else
	{

  //캐싱되지 않은 데이터들은 파라미터로 들어온 모델 경로 데이터를 그대로 받습니다.
		threads.emplace_back(&CModel::Create_Materials, this, szTexturePath.c_str());
	}

	Create_Bone(m_pModelData.get()->RootNode, 0);

	threads.emplace_back(&CModel::Mesh_Array, this, pModelData);

  //모든 스레드가 join 될때까지 대기합니다.
	for (auto& th : threads) {
		if (th.joinable()) {
			th.join();
		}
	}

	Create_Animations();

}
----

----

HRESULT CModel::Create_Materials(const _char* pModelFilePath)
{
	//unique_lock<shared_mutex> lock(m_MaterialMtx);

	vector<thread> Matthreads;

	/* 모델에는 여러개의 머테리얼이 존재한다. */
	/* 각 머테리얼마다 다양한 재질(Diffuse, Ambient,Specular...)을 텍스쳐로 표현한다. */
	m_iNumMaterials = m_pModelData->iNumMaterials;

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
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
      
      //머테리얼을 로드하기 전에 이미 로드되었는지 확인한 후에 중복 경로를 참조할 경우에 주소값만 넘겨서 로딩을 스킵합니다.
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
....

```
</details>

### 2. **오브젝트 풀링을 이용한 투사체 최적화**  
   - 반복적으로 생성 및 소멸되는 오브젝트를 효율적으로 관리하기 위해 오브젝트 풀링(Object Pooling) 기법을 적용했습니다.  
   - 이를 통해 메모리 할당과 해제를 최소화하고 성능을 개선했습니다.

  <details><summary>코드 구현 펼치기</summary>

    //ObjPoolManager.cpp 파일에서 오브젝트를 관리하는 CObjPoolManager 싱글톤 객체를 만들어 풀 객체를 관리하였습니다.
    
    void CObjPoolManager::BeamPooling()
    {
        //의미 없는 좌표에 객체들을 미리 다 만들어 둔 다음, Sleep 상태로 변경하여 오브젝트 레이어에 보관하여 둡니다.
        CTransform::TRANSFORM_DESC			TransformDesc{};
        TransformDesc.vPosition = _float4(9999.f, 9999.f, 9999.f, 1.f);
        TransformDesc.fSpeedPerSec = 8.f;
        TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
    
        for (_uint i = 0; i < iBulletCnt; ++i)
        {
            m_BeamList.emplace_back(dynamic_cast<CBeam*>(m_pGameInstance->Add_Clone_Return(LEVEL_STATIC, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Beam"), &TransformDesc)));
        }
    
    }
    
    CBeam* CObjPoolManager::BeamAwake(void* pArg)
    {
        //필요할 경우 Awake 함수를 호출하여 SLEEP 상태인 객체를 꺼내 파라미터를 입력하여 작동시킵니다.
        for (auto& Beam : m_BeamList)
        {
            if (Beam->Get_ContainState() == CContainedObject::CONTAIN_SLEEP)
            {
                Beam->Activate(pArg);
                return Beam;
            }
        }
    
        return nullptr;
      }
  </details>

### 3. **멀티 렌더 타겟과 Deferred 쉐이더 처리 기법**  
   - 여러 렌더 타겟과 Deferred Shading 기술을 활용하여 조명, 그림자, Glow, Blur 등의 후처리 효과를 구현했습니다.  
   - Deferred Shading 시에 더블 버퍼링 기법을 적용해 렌더 타겟 개수를 줄이고 메모리 성능을 최적화했습니다.

<details><summary>코드 구현 펼치기</summary>
```
	
	//Renderer.cpp 파일
 	//마지막 후처리 연산 코드이고, 이 파일 안에 일괄 빛 연산과 렌더 타겟, 멀티렌더타겟 등  여러 코드들이 있습니다. 
	HRESULT CRenderer::Final_Deferred()
	{
    	// 마지막 렌더링 단계로, 빛 연산이 아닌 렌즈 효과와 화면 후처리 작업을 처리합니다.

    #pragma region GlowPass
    // Glow 효과를 처리하는 렌더 패스입니다.

    // MRT(Multiple Render Target) 설정 시작 (Glow Front 타겟)
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GlowFront"))))
        return E_FAIL;

    // 이전 Glow Back 렌더 타겟을 셰이더의 g_GlowTexture로 바인딩
    if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_GlowBack"), m_pShader, "g_GlowTexture")))
        return E_FAIL;

    // 정점 버퍼(VIBuffer) 바인딩
    if (FAILED(m_pVIBuffer->Bind_Buffers()))
        return E_FAIL;

    // Glow 처리를 위한 셰이더 Pass(8번 Pass) 시작
    m_pShader->Begin(8);

    // 정점 버퍼 렌더링
    m_pVIBuffer->Render();

    // MRT 설정 종료
    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    //-----------------------------------------------------------------------------------------------------------------

    // Double Buffer 플래그를 토글합니다.
    // 동일한 렌더 타겟에 계속 그리는 것을 방지하기 위함입니다.
    m_bDoubleBuffer = !m_bDoubleBuffer;

    // Double Buffer 설정에 따라 MRT 설정
    if (m_bDoubleBuffer) {
        if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_DeferredBack"))))
            return E_FAIL;
        if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_DeferredFront"), m_pShader, "g_DeferredTexture")))
            return E_FAIL;
    } else {
        if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_DeferredFront"))))
            return E_FAIL;
        if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_DeferredBack"), m_pShader, "g_DeferredTexture")))
            return E_FAIL;
    }

    // Glow 효과 텍스처 바인딩
    if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_GlowFront"), m_pShader, "g_GlowTexture")))
        return E_FAIL;
    
    // 원본 Glow 텍스처를 바인딩하여 테두리 Glow 효과를 구현
    if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_GlowBack"), m_pShader, "g_GlowOriginalTexture")))
        return E_FAIL;

    // 정점 버퍼(VIBuffer) 바인딩
    if (FAILED(m_pVIBuffer->Bind_Buffers()))
        return E_FAIL;

    // Glow 효과 적용을 위한 셰이더 Pass(9번 Pass) 시작
    m_pShader->Begin(9);

    // 정점 버퍼 렌더링
    m_pVIBuffer->Render();

    // MRT 설정 종료
    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;
    #pragma endregion

    #pragma region GlitchPass
    // 화면에 Glitch 효과를 적용하는 렌더 패스입니다.

    if (m_pGameInstance->Get_UseGlitchLens()) {

        // Double Buffer 플래그를 토글합니다.
        m_bDoubleBuffer = !m_bDoubleBuffer;

        // Double Buffer 설정에 따라 MRT 설정
        if (m_bDoubleBuffer) {
            if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_DeferredBack"))))
                return E_FAIL;
            if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_DeferredFront"), m_pShader, "g_DeferredTexture")))
                return E_FAIL;
        } else {
            if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_DeferredFront"))))
                return E_FAIL;
            if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_DeferredBack"), m_pShader, "g_DeferredTexture")))
                return E_FAIL;
        }

        // Glitch 효과에 필요한 파라미터 바인딩
        if (FAILED(m_pShader->Bind_Float("shake_power", m_pGameInstance->Get_GlitchPower())))
            return E_FAIL;
        if (FAILED(m_pShader->Bind_Float("shake_rate", m_pGameInstance->Get_GlitchRate())))
            return E_FAIL;
        if (FAILED(m_pShader->Bind_Float("shake_speed", m_pGameInstance->Get_GlitchSpeed())))
            return E_FAIL;
        if (FAILED(m_pShader->Bind_Float("TIME", m_pGameInstance->Random_Float(0.f, 1.f))))
            return E_FAIL;
        if (FAILED(m_pShader->Bind_Float("shake_color_rate", m_pGameInstance->Get_GlitchColorRate())))
            return E_FAIL;
        if (FAILED(m_pShader->Bind_Float("red_mix_rate", m_pGameInstance->Get_RedBlockMixRate())))
            return E_FAIL;

        // Death Texture 바인딩
        m_pDeathTexture->Bind_ShaderResource(m_pShader, "g_DeathTexture", 0);

        // Glitch 효과를 위한 상태 값 설정
        _bool bUseRedBlock = m_pGameInstance->Get_UseRedBlockGlitch();
        if (FAILED(m_pShader->Bind_RawValue("g_bUseRedGlitch", &bUseRedBlock, sizeof(_bool))))
            return E_FAIL;

        _bool bUseBlackScreen = m_pGameInstance->Get_BlackScreen();
        if (FAILED(m_pShader->Bind_RawValue("g_bBlackScreen", &bUseBlackScreen, sizeof(_bool))))
            return E_FAIL;

        if (FAILED(m_pShader->Bind_Float("g_fBlackScreenRate", m_pGameInstance->Get_BlackScreenRate())))
            return E_FAIL;

        // 정점 버퍼(VIBuffer) 바인딩
        if (FAILED(m_pVIBuffer->Bind_Buffers()))
            return E_FAIL;

        // Glitch 효과 적용을 위한 셰이더 Pass(6번 Pass) 시작
        m_pShader->Begin(6);

        // 정점 버퍼 렌더링
        m_pVIBuffer->Render();

        // Glitch 텍스처 해제
        if (FAILED(m_pShader->Bind_SRV("g_DeferredTexture", nullptr)))
            return E_FAIL;

        // MRT 설정 종료
        if (FAILED(m_pGameInstance->End_MRT()))
            return E_FAIL;
    }
    #pragma endregion

    #pragma region Blur
    // Blur 효과를 적용하는 렌더 패스입니다.

    if (m_pGameInstance->Get_BlurScreen()) {

        // Double Buffer 플래그를 토글합니다.
        m_bDoubleBuffer = !m_bDoubleBuffer;

        // Double Buffer 설정에 따라 MRT 설정
        if (m_bDoubleBuffer) {
            if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_DeferredBack"))))
                return E_FAIL;
            if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_DeferredFront"), m_pShader, "g_DeferredTexture")))
                return E_FAIL;
        } else {
            if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_DeferredFront"))))
                return E_FAIL;
            if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_DeferredBack"), m_pShader, "g_DeferredTexture")))
                return E_FAIL;
        }

        // Blur 효과에 필요한 파라미터 바인딩
        if (FAILED(m_pShader->Bind_Float("g_fBlueScreenRate", m_pGameInstance->Get_BlueScreenRate())))
            return E_FAIL;
        if (FAILED(m_pShader->Bind_Float("g_BlurStrength", m_pGameInstance->Get_BlurPower())))
            return E_FAIL;

        // Blur 효과를 위한 상태 값 설정
        _bool bUseBlueScreen = m_pGameInstance->Get_BlueScreen();
        if (FAILED(m_pShader->Bind_RawValue("g_bUseBlueScreen", &bUseBlueScreen, sizeof(_bool))))
            return E_FAIL;

        _bool bUseRedScreen = m_pGameInstance->Get_RedScreen();
        if (FAILED(m_pShader->Bind_RawValue("g_bUseRedScreen", &bUseRedScreen, sizeof(_bool))))
            return E_FAIL;

        // 정점 버퍼(VIBuffer) 바인딩
        if (FAILED(m_pVIBuffer->Bind_Buffers()))
            return E_FAIL;

        // Blur 효과 적용을 위한 셰이더 Pass(7번 Pass) 시작
        m_pShader->Begin(7);

        // 정점 버퍼 렌더링
        m_pVIBuffer->Render();

        // MRT 설정 종료
        if (FAILED(m_pGameInstance->End_MRT()))
            return E_FAIL;
    }
    #pragma endregion

    // 최종 Deferred Texture를 셰이더에 바인딩
    if (m_bDoubleBuffer) {
        if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_DeferredBack"), m_pShader, "g_DeferredTexture")))
            return E_FAIL;
    } else {
        if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_DeferredFront"), m_pShader, "g_DeferredTexture")))
            return E_FAIL;
    }

    // 정점 버퍼(VIBuffer) 바인딩
    if (FAILED(m_pVIBuffer->Bind_Buffers()))
        return E_FAIL;

    // 최종 합성 처리를 위한 셰이더 Pass(5번 Pass) 시작
    m_pShader->Begin(5);

    // 정점 버퍼 렌더링
    m_pVIBuffer->Render();

    return S_OK;
	}

</details>

![시퀀스 01_3](https://github.com/user-attachments/assets/e8135a88-427e-4cbb-9402-20f100eed3a4)

### 4. **PhysX와 직접 구현한 물리 엔진**  
   - NVIDIA PhysX를 활용하여 복잡한 메쉬 기반 충돌 처리를 구현했습니다.  
   - 직접 설계한 물리 엔진으로 캐릭터와 환경 간의 상호작용 요소를 추가했습니다.
     
<details><summary>코드 구현 펼치기</summary>
```
	
	void CCamera_TPV::PriorityTick(_float fTimeDelta)
	{
		//Camera_HeadTilt_Reset(fTimeDelta);
		Camera_Roll(fTimeDelta);
	}
	
	void CCamera_TPV::Tick(_float fTimeDelta)
	{
 		//강체 컴포넌트에 델타타임을 지속적으로 업데이트함으로써 속력 감속을 조절합니다.
		m_pRigidbody->Update(fTimeDelta);
		Key_Input(fTimeDelta);
	
	}
	
	void CCamera_TPV::LateTick(_float fTimeDelta)
	{
 		//PhysX 시뮬레이트에서 물리 충돌이 일어난 회전을 제외한 좌표값만 참조받아 좌표 갱신을 하여 메쉬 충돌을 구현하였습니다. 
		m_pPhysXCollider->Synchronize_Transform_Position(m_pTransformCom);
		//이 이후에 카메라 포지션 기반 이펙트 넣는건 오케이(피젝스 포지션으로 다시 돌아가서 그냥 막 넣어도 됨)
		if(PLAYER->Get_IsSlide() == true)
			m_pTransformCom->Go_Down(PLAYER->Get_SlideOffset());
	
		Fall_Check();
		Camera_Effect(fTimeDelta);
	
	}
</details>

### 5. **메쉬 데이터 직렬화를 통한 최적화**  
   - 메쉬 데이터를 이진 직렬화하여 파일 크기를 줄이고, 로드 시간을 단축했습니다.  
   - 대규모 스테이지 로딩 시 지연 시간을 효과적으로 감소시켰습니다.

<details><summary>코드 구현 펼치기</summary>
```
	
	//ModelData.cpp 파일
 
	const static _bool g_bUseAssimp = false;
	
	// MODEL_DATA 클래스에서 모델 데이터를 생성하는 함수입니다.
	// 모델 데이터를 Assimp를 통해 로드하거나, 바이너리 파일로부터 로드합니다.
	HRESULT MODEL_DATA::Make_ModelData(const char* szFilePath, const MODEL_TYPE& In_eModelType, _fmatrix In_TransformMatrix, _bool bAnimZero)
	{
	    // 모델 타입 저장
	    eModelType = In_eModelType;

    // 모델의 변환 행렬 저장
    XMStoreFloat4x4(&TransformMatrix, In_TransformMatrix);

    // 모델 파일 경로 저장
    szModelFilePath = szFilePath;

    // Assimp를 사용하여 모델 데이터를 로드하는 경우
    if (true == g_bUseAssimp)
    {
        if (FAILED(Load_FromAssimp(bAnimZero))) // Assimp를 통해 모델 로드
            return E_FAIL;

        return S_OK;
    }

    // 바이너리 파일 경로 설정
    string szBinFilePath;

    // 경로 및 파일명 추출
    char szDir[MAX_PATH] = "";
    char szFileName[MAX_PATH] = "";

    _splitpath_s(szModelFilePath.c_str(), nullptr, 0, szDir, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);

    // 바이너리 파일 경로 구성
    szBinFilePath = szDir;
    szBinFilePath += szFileName;
    szBinFilePath += ".bin";
    szModelFileName = szFileName;

    // 바이너리 파일 읽기 시도
    ifstream is(szBinFilePath, std::ios::binary);

    // 바이너리 파일이 열리지 않거나 Assimp를 사용하려는 경우
    if (!is.is_open() || g_bUseAssimp)
    {
        is.close(); // 파일 스트림 닫기
        if (FAILED(Load_FromAssimp(bAnimZero))) // Assimp를 통해 모델 로드
            return E_FAIL;
    }
    else
    {
        is.close(); // 파일 스트림 닫기
        Load_FromBinary(); // 바이너리 파일로부터 모델 로드
    }

	#ifdef _DEBUG
	    // 디버그 모드에서 애니메이션 로그를 생성합니다.
	    if (MODEL_TYPE::ANIM == eModelType)
	    {
	        string szDebugFileName;

        // 디버그 파일 경로 설정
        szDebugFileName = "../bin/Debugs/";
        szDebugFileName += szModelFileName;
        szDebugFileName += ".txt";

        // 디버그 로그 파일 출력 스트림 생성
        ofstream fout(szDebugFileName);

        // 디버그 로그 기록 (애니메이션, 노드, 메시 데이터)
        Debug_AnimationLog(fout);
        RootNode->Debug_NodeLog(fout);
        Debug_MeshLog(fout);

        fout.close(); // 디버그 로그 파일 닫기
    }
	#endif // _DEBUG

    return S_OK; // 성공적으로 모델 데이터를 생성했음을 반환
	}

	// MODEL_DATA 클래스에서 바이너리 파일로 데이터를 저장(Bake)하는 함수입니다.
	void MODEL_DATA::Bake_Binary()
	{
	    string szBinFilePath; // 바이너리 파일 경로를 저장할 변수

    // 디렉터리와 파일명 추출을 위한 배열
    char szDir[MAX_PATH] = "";
    char szFileName[MAX_PATH] = "";

    // szModelFilePath에서 디렉터리와 파일명을 추출
    _splitpath_s(szModelFilePath.c_str(), nullptr, 0, szDir, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);

    // 바이너리 파일 경로 생성
    szBinFilePath = szDir;
    szBinFilePath += szFileName;
    szBinFilePath += ".bin"; // 확장자를 .bin으로 설정

    // 바이너리 파일 출력 스트림 생성
    ofstream os(szBinFilePath, ios::binary);

	#ifdef _DEBUG
	    // 디버그 모드에서 파일이 정상적으로 열렸는지 확인
	    assert(os.is_open());
	#endif // _DEBUG

    // 모델 데이터를 바이너리 파일에 순차적으로 기록
    write_typed_data(os, TransformMatrix);  // 모델의 변환 행렬 기록
    write_typed_data(os, iNumMeshs);        // 메시 개수 기록
    write_typed_data(os, iNumMaterials);    // 재질(Material) 개수 기록
    write_typed_data(os, iNumAnimations);   // 애니메이션 개수 기록
    write_typed_data(os, eModelType);       // 모델 타입 기록
    write_typed_data(os, VertexInfo);       // 정점(Vertex) 정보 기록

    // 루트 노드의 데이터를 바이너리로 기록
    RootNode->Bake_Binary(os);

    // 각 재질 데이터를 바이너리로 기록
    for (_uint i = 0; i < iNumMaterials; i++)
    {
        Material_Datas[i]->Bake_Binary(os);
    }

    // 각 메시 데이터를 바이너리로 기록
    for (_uint i = 0; i < iNumMeshs; i++)
    {
        Mesh_Datas[i]->Bake_Binary(os);
    }

    // 각 애니메이션 데이터를 바이너리로 기록
    for (_uint i = 0; i < iNumAnimations; i++)
    {
        Animation_Datas[i]->Bake_Binary(os);
    }

    // 파일 스트림 닫기
    os.close();
	}



</details>

### 6. **몬스터 상태 제어 및 관리**  
   - 상태 머신(State Machine)을 설계하여 몬스터의 이동, 공격, 피격, 사망 상태를 체계적으로 관리했습니다.  
   - 다양한 행동 전환을 자연스럽게 구현하며, 난이도 조절 기능을 추가했습니다.
<details><summary>코드 구현 펼치기</summary>
```

 	// Samurai.cpp
	// CSamurai 클래스의 상태 머신(State Machine) 함수입니다.
	// 이 함수는 사무라이 객체의 상태를 관리하며, 각 상태에 따라 적절한 동작을 실행합니다.
	void CSamurai::State_Machine(_float fTimeDelta)
	{
	    // 사무라이의 첫 번째 파츠(예: 칼)를 활성화합니다.
	    dynamic_cast<CKatana*>(m_vecParts[0])->Set_On(true);

    // 키 입력(eKeyCode::R)으로 초기 상태로 리셋
    if (GAMEINSTANCE->GetKeyDown(eKeyCode::R)) {
        // PhysX 콜라이더의 위치를 초기화
        m_pPhysXCollider->Get_Controller()->setPosition(m_vInitPos);

        // 사무라이 상태를 '대기(SAMURAI_IDLE)' 상태로 설정
        m_eSamuraiState = SAMURAI_IDLE;

        // 대시 상태를 '대시 종료(DASH_END)'로 설정
        m_eDashState = DASH_END;

        // 사무라이의 임시 사망 상태를 해제
        Set_TempDead(false);
    }

    // 현재 사무라이 상태에 따라 적절한 동작을 실행
    switch (m_eSamuraiState)
    {
        case SAMURAI_IDLE:
            // 대기 상태 처리
            State_Idle(fTimeDelta);
            break;
        case SAMURAI_SEARCH:
            // 검색 상태 처리
            State_Search(fTimeDelta);
            break;
        case SAMURAI_ATTACK:
            // 공격 상태 처리
            State_Attack(fTimeDelta);
            break;
        case SAMURAI_GUARD:
            // 방어 상태 처리
            State_Guard(fTimeDelta);
            break;
        case SAMURAI_DASH:
            // 대시 상태 처리
            State_Dash(fTimeDelta);
            break;
        case SAMURAI_STUN:
            // 스턴 상태 처리
            State_Stun(fTimeDelta);
            break;
        case SAMURAI_DIE:
            // 사망 상태 처리
            State_Die(fTimeDelta);
            break;
    }
	}


</details>

![시퀀스 01_5](https://github.com/user-attachments/assets/4afc3583-748b-415c-a53e-b3cf472a3603)

### 7. **ImGui 기반 맵 에디터**  
   - ImGui를 활용한 맵 에디터를 개발하여 스테이지 오브젝트 배치와 조명 설정(정광원, 방향성 광원)을 직관적으로 조정할 수 있었습니다.

### 8. **사운드 랜덤 재생 시스템**  
   - 사운드 랜덤 재생 시스템을 구축하여 플레이어 몰입도를 강화했습니다.  
   - 유사한 사운드를 무작위로 재생해 반복 효과음을 방지했습니다.

<details><summary>코드 구현 펼치기</summary>
```
	
	//Random 라이브러리를 이용한 
	_int CMath_Manager::Random_Int(_int _iMin, _int _iMax)
	{
		std::random_device RD;
		std::mt19937 gen(RD());
	
		std::uniform_int_distribution<_int> dis(_iMin, _iMax);
	
		return dis(gen);
	}
	//이런 코드로 랜덤 구현

	switch (GAMEINSTANCE->Random_Int(1, 7)) {
	case 1:
		m_pGameInstance->Play(TEXT("Sound_Player_Katana_Parry_01"), false);
		m_pGameInstance->SetVolume(TEXT("Sound_Player_Katana_Parry_01"), 0.25f);
		break;
	case 2:
		m_pGameInstance->Play(TEXT("Sound_Player_Katana_Parry_02"), false);
		m_pGameInstance->SetVolume(TEXT("Sound_Player_Katana_Parry_02"), 0.25f);
		break;
	case 3:
		m_pGameInstance->Play(TEXT("Sound_Player_Katana_Parry_03"), false);
		m_pGameInstance->SetVolume(TEXT("Sound_Player_Katana_Parry_03"), 0.25f);
		break;
	case 4:
		m_pGameInstance->Play(TEXT("Sound_Player_Katana_Parry_04"), false);
		m_pGameInstance->SetVolume(TEXT("Sound_Player_Katana_Parry_04"), 0.25f);
		break;
	case 5:
		m_pGameInstance->Play(TEXT("Sound_Player_Katana_Parry_05"), false);
		m_pGameInstance->SetVolume(TEXT("Sound_Player_Katana_Parry_05"), 0.25f);
		break;
	case 6:
		m_pGameInstance->Play(TEXT("Sound_Player_Katana_Parry_06"), false);
		m_pGameInstance->SetVolume(TEXT("Sound_Player_Katana_Parry_06"), 0.25f);
		break;
	case 7:
		m_pGameInstance->Play(TEXT("Sound_Player_Katana_Parry_07"), false);
		m_pGameInstance->SetVolume(TEXT("Sound_Player_Katana_Parry_07"), 0.25f);
		break;
	}
</details>

### 9. **플레이어 제어**  
   - PhysX CCT에서 측면 충돌을 파악해 벽타기에 맞는 애니메이션과 카메라 회전을 구현하였습니다.

<details><summary>코드 구현 펼치기</summary>
```

 	//Camera_TPV.cpp
	// 플레이어가 벽과 충돌 중이고, 'A' 키가 눌렸을 때 실행되는 동작
 	//PhysX 충돌 콜백 객체에 측면 충돌 여부 확인
	if (true == m_pPhysXCollider->Get_CollisionSides() && GAMEINSTANCE->GetKey(eKeyCode::A)) {

    // 롤 방향을 왼쪽(-1)으로 설정
    m_iRollDirection = -1;

    // Rigidbody 상태를 WIRE_END로 설정 (자유로운 물리 상태로 전환)
    m_pRigidbody->Set_Grab(CRigidbody::WIRE_END);

    // 플레이어 상태를 WIRE_END로 설정
    PLAYER->Set_WireState(CPlayer_Manager::WIRE_END);

    // 카메라 롤이 활성화되어 있는 경우
    if (m_bCameraRoll == true) {
        // 현재 롤 방향에 따라 카메라 회전을 설정 (헤드 롤 적용)
        m_pTransformCom->Head_Roll(-1.f * m_fRollAngle * m_iRollDirection);
    }

    // 롤 각도 증가: 제한 각도(m_fRollAngleLimit)를 초과하지 않도록 처리
    if (m_fRollAngle < m_fRollAngleLimit)
        m_fRollAngle += fTimeDelta * 3.f; // 각도를 시간 델타에 따라 증가
    else
        m_fRollAngle = m_fRollAngleLimit; // 제한 각도로 설정

    // 최종 롤 각도를 적용하여 카메라 회전
    m_pTransformCom->Head_Roll(m_fRollAngle * m_iRollDirection);

    // 플레이어 상태를 이동(PLAYER_MOVE) 상태로 설정
    PLAYER->Set_PlayerState(CPlayer_Manager::PLAYER_MOVE);

    // 벽 달리기 상태를 왼쪽 벽(WALLRUN_LEFT)으로 설정
    PLAYER->Set_WallRunState(CPlayer_Manager::WALLRUN_LEFT);

    // 카메라 롤 활성화 플래그 설정
    m_bCameraRoll = true;
	}
 
</details>

![시퀀스 01_3](https://github.com/user-attachments/assets/e8135a88-427e-4cbb-9402-20f100eed3a4)

   - 대쉬, 해킹, 원거리 검기 공격 등 스킬을 구현하였습니다.

<details><summary>코드 구현 펼치기</summary>
```

 	//PistolGunner.cpp 파일
	//PistolGunner 객체에서 플레이어 카메라 기준 레이캐스팅 실행
	void CPistolGunner::Collision_RayHit()
	{
		//해킹 기술이 사용 가능한지 체크 후 통과
		if (GAMEINSTANCE->GetKeyDown(eKeyCode::Q) && PLAYER->Get_Skill_Index() == CPlayer_Manager::SKILL_MINDCONTROL && PLAYER->Get_SkillUsable()) {
			m_pGameInstance->Play(TEXT("Sound_Player_Skill_Full"), false);
			m_pGameInstance->SetVolume(TEXT("Sound_Player_Skill_Full"), 1.f);
			Set_Controlled(true);
			PLAYER->Set_SkillGaugeZero();
		}
	}
 
	//
 	if (m_bControlled == true) {
	if(m_pTarget == nullptr)
		m_pTarget = ENEMYMANAGER->Get_NearEnemy(this, 50.f);
	}
 
</details>

![검기](https://github.com/user-attachments/assets/b1c62f24-8b34-4f52-8059-37e3ec49ddbd)
![해킹](https://github.com/user-attachments/assets/a0c1f62b-71f3-4550-bb1b-93e92f1130ae)

   - 불릿 타임 기능을 추가하여 투사체를 피하거나 패링할 수 있게 설계했습니다.

![시퀀스 01_6](https://github.com/user-attachments/assets/01c21d0e-a7b0-4655-afdb-74d081b617fc)
  
   - 점프와 같은 물리는 점프 시작과 점프 루프, 착지와 같이 여러 분기처리를 하고 애니메이션을 연동하는 예외적인 부분도 전부 처리하였습니다.
   - 점프, 슬라이딩 등 다양한 물리 계산 및 Quake식 가속을 활용해 속도감을 높였습니다.  
   - 소드 트레일 효과를 통해 검 휘두를 때 궤적을 표현했습니다.

---

## 트러블슈팅

### 1. **맵 로딩 최적화**  
- 맵 자체에 오브젝트가 너무 많고 리소스의 퀄리티도 너무 높아, 로딩이 지나치게 오래 걸리는 문제가 있었습니다. (약 30분…)

한번 할 때마다 로딩을 30분씩 하는 광경을 도저히 볼 수가 없어서, 저는 일단 메쉬 데이터를 전부 이진 직렬화하는 과정을 거치고
시간을 재 보았을 때. 무려 15분이나 감소하는 성과를 얻었지만, 15분이 감소해도 15분이 남았기에 더욱 최적화를 해야 했습니다.

그 후 저는 멀티스레딩 기법을 이용해, 내부에서 여러 머테리얼들을 부르는 과정이나, 본을 노드화 시켜 생성하는 여러 과정들을 멀티스레딩 방식으로 처리하고, 성능이 조금 떨어지지만 뮤텍스를 이용해 메모리 중복 접근으로 인한 오류를 방지했습니다.

이 방식까지 사용 했을 때도 10분정도 감소되어 5분정도의 로딩 시간이 소요되었는데, 그 후 중복 텍스쳐의 로딩을 방지하기 위해, 텍스쳐의 이름들을 전부 고유하게 처리하여 중복 텍스쳐의 로딩을 방지하고, 기존에 있던 리소스 컨테이너에서 주소값만 참조하는 방식으로 변경하자, 기존 30분 이상 걸리던 로딩을 40초까지 줄이는 데에 성공하였습니다. 

그 이상의 내부 로직을 멀티스레딩을 이용해 최적화 작업을 시도하였는데, 컨텍스트 스위칭 코스트가 더 높게 잡히는 일이 일어나 역으로 속도가 더 내려가거나, 의도치 않은 결과가 나오는 일이 벌어져 마지막으로 30분이 걸리던 로딩 시간은 40초대 로딩으로 마무리하였습니다.

---

요약 
- **이진 직렬화**: 메쉬 데이터를 직렬화하여 로딩 시간을 15분으로 단축.  
- **멀티스레딩**: 본 생성, 머티리얼 로딩을 병렬로 처리하고 뮤텍스로 오류를 방지하며 로딩 시간을 5분으로 단축.  
- **중복 텍스처 방지**: 텍스처 이름을 고유화하고 주소 참조 방식을 변경해 최종 로딩 시간을 40초로 단축.

---

### 2. **와이어 액션 구현**  
와이어 액션에도 생각보다 생각할 거리가 여러 부분 있었습니다. 기본적으로 와이어 액션을 하는 경우 와이어의 방향으로 직선 운동을 하는 액션과, 
와이어는 대략적인 방향 벡터만 제시하고 타잔처럼 와이어의 원점을 스쳐 지나가는 액션이 있는데, 제가 필요한 것은 후자였습니다. 

전자의 경우는 구현하기는 굉장히 쉬웠지만, 후자의 경우에는 와이어 트레일 같은 트레일버퍼 쉐이딩은 쉽다고 생각될 정도의 물리였습니다. 

저는 다른 부분은 양보하더라도, 이런 무브먼트 부분에 관해서는 제가 할 수 있는 최대한의 정성을 써서 만드는 편입니다. 

와이어 액션은 와이어를 작동하는 순간 플레이어 내부의
Wire State를 작동시킵니다. Wire State는 3가지 상태로
출발 가속, 원점 도착, 감속 도착 이라는 상태로 나누었습니다.
Y축이 가속에 끼치는 영향은 최대한 줄이면서, 완전한 포물선 운동이 아니라 내가 중간에 세부적인 움직임이 가능해야 했습니다.
중간에 대쉬와 같은 상태를 끊어버리는 행위를 한다면 
Wire State는 강제 종료되고 가속은 초기화됩니다. 

![시퀀스 01_4](https://github.com/user-attachments/assets/e48d189b-a3e4-4b1b-9464-6c08e940be74)

가속 물리조차도 기본적인 뛰기나 점프, 슬라이딩과 와이어 액션중에 전부 다른 물리 공식을 사용했기에 정말 쉽지 않은 작업이었다고 생각합니다.

---

요약 

- 기본 직선 운동 대신 포물선 운동과 중간 대쉬를 결합한 복잡한 물리 동작을 구현.  
- Wire State를 출발 가속, 원점 도착, 감속 도착으로 나누어 세밀한 움직임 제어를 가능하게 했습니다.  
- 점프, 슬라이딩과의 물리 공식 차이를 극복하며 사실적인 와이어 액션을 구현했습니다.

---

## 후기
DirectX 11로 작업하면서 만든 첫 개인 포트폴리오입니다.

화려한 쉐이더와 배경, 네온사인과 와이어 불릿타임 액션이라는 재미있어 보이는 것만 들어가 있어서 작업하는 내내 즐거웠지만,

사실 개발 기간 중에 마지막 한달 전, 저는 제 실수로 외장 하드를 분실해 코드와 리소스가 한번 소실되어서 다시 작업하게 된 작품입니다…

코드는 그래도 구버전이 커밋을 해서 조금 남아있었지만, 리소스까지 Github에 올리기에는 너무 무거워 안 올리고 있었던 불찰 때문에 제가 원래 원했던 목표치까지 도달하지 못했다는 아쉬움이 좀 많이 남았습니다.

그래도 제가 제 게임을 플레이하면서도 꽤 재미있었다고 생각이 들 정도였기에, 물리 액션 자체는 정말 공을 많이 들인 결과가 나와 주어서 아쉬움이 남았지만 그래도 만족스러운 결과였다고 생각합니다.


