#include "Imgui_Manager.h"
#include "Light.h"
#include "GameInstance.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);

extern IMGUI_IMPL_API bool ImGui_ImplDX11_Init(ID3D11Device* device, 
	ID3D11DeviceContext* device_context);

//ImGui::FileBrowser fileDialog(ImGuiFileBrowserFlags_SelectDirectory);

CImgui_Manager::CImgui_Manager()
{

}


void CImgui_Manager::Initialize(HWND hWnd, ID3D11Device* Device, ID3D11DeviceContext* pContext)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(Device, pContext);

	ImGui::StyleColorsDark();
	
	m_vecObjectNames.emplace_back("Light");
	m_vecObjectNames.emplace_back("PIstolGunner");
	m_vecObjectNames.emplace_back("Uzi");
	m_vecObjectNames.emplace_back("Waver");
	m_vecObjectNames.emplace_back("Wire");

	fileDialog = ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory);
}

HRESULT CImgui_Manager::Imgui_ProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);

	return S_OK;
}

HRESULT CImgui_Manager::Imgui_ViewInit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	ImGui_ImplDX11_Init(pDevice, pContext);

	return S_OK;
}

void CImgui_Manager::PreTick()
{
	if (GAMEINSTANCE->GetKeyDown(eKeyCode::P)) {

		if (m_tGizmoDesc.CurrentGizmoOperation == ImGuizmo::TRANSLATE)
			m_tGizmoDesc.CurrentGizmoOperation = ImGuizmo::ROTATE;
		else if (m_tGizmoDesc.CurrentGizmoOperation == ImGuizmo::ROTATE)
			m_tGizmoDesc.CurrentGizmoOperation = ImGuizmo::SCALE;
		else if (m_tGizmoDesc.CurrentGizmoOperation == ImGuizmo::SCALE)
			m_tGizmoDesc.CurrentGizmoOperation = ImGuizmo::TRANSLATE;
	}
}

void CImgui_Manager::Tick()
{
}

void CImgui_Manager::LateTick()
{
}

void CImgui_Manager::Imgui_Render()
{

	static bool window = true;
	ImGui::SetNextWindowSize(ImVec2(474, 589));

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGuizmo::BeginFrame();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::SetOrthographic(false);

	if (ImGui::Begin("MapData", &window, ImGuiWindowFlags_MenuBar)) {

		Imgui_MenuBar();
	
		ImGui::SetCursorPos(ImVec2(19, 34));
		ImGui::Button("Object", ImVec2(50, 19)); //remove size argument (ImVec2) to auto-resize

		ImGui::SetCursorPos(ImVec2(74.5, 34));
		ImGui::Button("Light", ImVec2(50, 19)); //remove size argument (ImVec2) to auto-resize

		ImGui::SetCursorPos(ImVec2(122.5, 34));
		ImGui::Button("Model", ImVec2(50, 19)); //remove size argument (ImVec2) to auto-resize

		ImGui::SetCursorPos(ImVec2(12, 63));
		ImGui::BeginChild(13, ImVec2(448, 501), true);

		ImGui::SetCursorPos(ImVec2(6, 9.5));
		ImGui::PushItemWidth(435);

		if (ImGui::BeginCombo("##", m_vecObjectNames[m_eObjectType].data(), ImGuiComboFlags_None)) {
			for (int n = 0; n < m_vecObjectNames.size(); n++) {
				if (ImGui::Selectable(m_vecObjectNames[n].data(), n == m_eObjectType)) {
					m_eObjectType = static_cast<eObjectType>(n);
					m_pSelectObject = nullptr;
				}
			}
			ImGui::EndCombo();
		}


		ImGui::PopItemWidth();

		ImGui::SetCursorPos(ImVec2(7, 35.5));
		if (ImGui::Button("Create", ImVec2(214, 19))) {
			Imgui_ObjCreate();
		}

		ImGui::SetCursorPos(ImVec2(227, 35.5));
		if (ImGui::Button("Delete", ImVec2(214, 19))) {
			if (m_pSelectObject != nullptr) {
				m_pSelectObject->Set_IsDead(true);

				for (_uint i = 0; i < m_vecObjects[m_eObjectType].size(); ++i) {
					if (m_vecObjects[m_eObjectType][i] == m_pSelectObject) {
						m_vecObjects[m_eObjectType].erase(m_vecObjects[m_eObjectType].begin() + i);
						break;
					}
				}
				m_pSelectObject = m_vecObjects[m_eObjectType].empty() ? nullptr : m_vecObjects[m_eObjectType].back();
			}

		}

		ImGui::SetCursorPos(ImVec2(5, 67));
		ImGui::PushItemWidth(438);
		static int item_current36 = 0;


		if (ImGui::BeginListBox("##", ImVec2(428, 300))) {
			for (int i = 0; i < m_vecObjects[m_eObjectType].size(); ++i) {
				if (ImGui::Selectable((m_vecObjects[m_eObjectType][i]->Get_SubTag()+=to_string(i)).c_str(), item_current36 == i)) {
					item_current36 = i;
					Imgui_SetSelectObject(m_vecObjects[m_eObjectType][item_current36]);
				}
			}
			ImGui::EndListBox();
		}


		ImGui::PopItemWidth();

		if (m_pSelectObject != nullptr) {
			Imgui_GizmoRender();
		}

		ImGui::EndChild();
	}

	ImGui::End();

	fileDialog.Display();

	if (fileDialog.HasSelected())
	{
		Imgui_MapDataControl();
		m_eMapLoadType = eMapLoadType::MAP_END;
		fileDialog.ClearSelected();
	}


	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	/*if (GAMEINSTANCE->GetKeyDown(eKeyCode::K)) {
		m_vecObjects[PISTOLGUNNER].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("PistolGunner"), TEXT("Prototype_GameObject_PistolGunner")));
		Imgui_SetSelectObject(m_vecObjects[PISTOLGUNNER].back());
	}

	if (GAMEINSTANCE->GetKeyDown(eKeyCode::L)) {
		m_pSelectObject = nullptr;
		Clear_Objects();
	}*/
}

void CImgui_Manager::Imgui_Main()
{
}

void CImgui_Manager::Imgui_GizmoRender()
{
	if(m_pSelectObject == nullptr)
		return;

	CTransform* pTerrainTransform = dynamic_cast<CTransform*>(m_pSelectObject->Find_Component(L"Com_Transform"));
	_float4x4 WorldMat, ViewMat, ProjMat;
	_float4  vPos;
	static _float3 vRot = { 0.f,0.f,0.f };
	_float3 vScale;

	WorldMat = *pTerrainTransform->Get_WorldFloat4x4();
	ViewMat = *GAMEINSTANCE->Get_Transform_Float4x4(CCamera_Manager::D3DTS_VIEW);
	ProjMat = *GAMEINSTANCE->Get_Transform_Float4x4(CCamera_Manager::D3DTS_PROJ);

	_bool flag = false;

	ImGuizmo::DecomposeMatrixToComponents(&WorldMat._11, &vPos.x, &vRot.x, &vScale.x);
	ImGui::PushItemWidth(200.f);			// 인풋 창 크기 설정하는거
	flag = ImGui::InputFloat3("Trans", &vPos.x);
	if(flag){
		pTerrainTransform->Set_Position(vPos);
	}	// 이동
	ImGui::PushItemWidth(200.f);

	flag = ImGui::InputFloat3("Rotat", &vRot.x);
	if(flag){
		pTerrainTransform->Rotation_Qt(vRot);
	}

	//회전
	ImGui::PushItemWidth(200.f);

	flag = ImGui::InputFloat3("Scale", &vScale.x);
	if(flag){
		if(vScale.x != 0.f && vScale.y != 0.f && vScale.z != 0.f)
			pTerrainTransform->Set_Scale(vScale);
	}// 크기

	ImGuizmo::RecomposeMatrixFromComponents(&vPos.x, &vRot.x, &vScale.x, &WorldMat._11);

	if (ImGuizmo::Manipulate(&ViewMat._11, &ProjMat._11			// 뷰, 투영행렬
		, m_tGizmoDesc.CurrentGizmoOperation			// Tr, Rt, Sc
		, m_tGizmoDesc.CurrentGizmoMode				// WORLD, LOCAL
		, (_float*)&WorldMat					// 객체의 월드 행렬
		, NULL							// 그냥 NULL 고정하자
		, m_tGizmoDesc.bUseSnap ? &m_tGizmoDesc.snap[0] : NULL	// 위의 틱당 단위 설정들
		, m_tGizmoDesc.boundSizing ? m_tGizmoDesc.bounds : NULL
		, m_tGizmoDesc.boundSizingSnap ? m_tGizmoDesc.boundsSnap : NULL))
	{
		m_pSelectObject->Set_WorldMatrix(WorldMat);
	}

	if (m_eObjectType == LIGHT) {
		CLight* pLight = dynamic_cast<CLight*>(m_pSelectObject);

		ImGui::PushItemWidth(200.f);			// 인풋 창 크기 설정하는거
		ImGui::InputFloat4("Diffuse", pLight->Get_LightDiffusePtr());

		ImGui::InputFloat4("Ambient", pLight->Get_LightAmbientPtr());

		ImGui::InputFloat4("Specular", pLight->Get_LightSpecularPtr());

		ImGui::InputFloat("Range", pLight->Get_LightRangePtr());
	}


}

void CImgui_Manager::Imgui_MenuBar()
{

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open Map"))
			{
				fileDialog = ImGui::FileBrowser();
				fileDialog.Open();
				m_eMapLoadType = eMapLoadType::MAP_LOAD;
			}

			if (ImGui::MenuItem("Save Map"))
			{
				fileDialog = ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory);
				fileDialog.Open();
				m_eMapLoadType = eMapLoadType::MAP_SAVE;
				//Make_Map();
			}

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

}

void CImgui_Manager::Imgui_ObjCreate()
{
	CTransform::TRANSFORM_DESC tDesc{};
	XMStoreFloat4(&tDesc.vPosition,GAMEINSTANCE->Get_CamPosition_Vector());

	/*LIGHT_DESC			LightDesc{};
	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	XMStoreFloat4(&LightDesc.vPosition, GAMEINSTANCE->Get_CamPosition_Vector());
	LightDesc.fRange = 20.f;
	strcpy_s(LightDesc.szName, "Light_Point");
	LightDesc.vDiffuse = _float4(1.0f, 0.f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.1f, 0.3f, 0.1f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;*/

	LIGHT_DESC			LightDesc{};
	LightDesc.eType = LIGHT_DESC::TYPE_SPOT;
	XMStoreFloat4(&LightDesc.vPosition, GAMEINSTANCE->Get_CamPosition_Vector());
	LightDesc.fRange = 20.f;
	strcpy_s(LightDesc.szName, "Light_Point");
	LightDesc.vDiffuse = _float4(1.0f, 0.f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.1f, 0.3f, 0.1f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse; 
	LightDesc.g_fSpotInnerConeAngle = 0.f;
	LightDesc.g_fSpotOuterConeAngle = 0.7f;
	LightDesc.g_fLightFalloff = 0.01f;

	switch (m_eObjectType)
	{
		case LIGHT :{
			m_vecObjects[LIGHT].push_back(GAMEINSTANCE->Add_Light_Return( &LightDesc));
			Imgui_SetSelectObject(m_vecObjects[LIGHT].back());
			break;
		}
		case PISTOLGUNNER: {
			m_vecObjects[PISTOLGUNNER].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("PistolGunner"), TEXT("Prototype_GameObject_PistolGunner"), &tDesc));
			Imgui_SetSelectObject(m_vecObjects[PISTOLGUNNER].back());
			break;
		}
		case UZIGUNNER: {
			m_vecObjects[UZIGUNNER].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("Uzi"), TEXT("Prototype_GameObject_Uzi"), &tDesc));
			Imgui_SetSelectObject(m_vecObjects[UZIGUNNER].back());
			break;
		}
		case WAVER: {
			m_vecObjects[WAVER].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("Waver"), TEXT("Prototype_GameObject_Waver"), &tDesc));
			Imgui_SetSelectObject(m_vecObjects[WAVER].back());
			break;
		}
		case WIRE: {
			m_vecObjects[WIRE].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("Wire"), TEXT("Prototype_GameObject_Grappling_Point"), &tDesc));
		}
		default:
			break;

	}
}

void CImgui_Manager::Imgui_MapDataControl()
{
	switch (m_eMapLoadType)
	{
	case eMapLoadType::MAP_LOAD:
		Load_Map();
		break;
	case eMapLoadType::MAP_SAVE:
		Save_Map();
		break;
	case eMapLoadType::MAP_END:
		break;
	default:
		break;
	}
}

void CImgui_Manager::Clear_Objects()
{
	for (_uint i = 0; i < OBJTYPE_END; ++i) {
		for(_uint j = 0; j < m_vecObjects[i].size(); ++j)
			m_vecObjects[i][j]->Set_IsDead(true);

		m_vecObjects[i].clear();
	}
}

void CImgui_Manager::Make_Map()
{
	for (_uint i = 0; i < m_vecMapObjData.size(); i++) {
		switch (m_vecMapObjData[i].iObjType)
		{
		case LIGHT:
			m_vecObjects[LIGHT].push_back(GAMEINSTANCE->Add_Light_Return(&m_vecMapObjData[i].tLightDesc));
			m_vecObjects[LIGHT].back()->Set_WorldMatrix(m_vecMapObjData[i].matWorld);
			break;
		case PISTOLGUNNER:
			m_vecObjects[PISTOLGUNNER].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("PistolGunner"), TEXT("Prototype_GameObject_PistolGunner")));
			m_vecObjects[PISTOLGUNNER].back()->Set_WorldMatrix(m_vecMapObjData[i].matWorld);
			break;
		case UZIGUNNER:
			m_vecObjects[UZIGUNNER].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("Uzi"), TEXT("Prototype_GameObject_Uzi")));
			m_vecObjects[UZIGUNNER].back()->Set_WorldMatrix(m_vecMapObjData[i].matWorld);
			break;
		case WAVER:
			m_vecObjects[WAVER].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("Waver"), TEXT("Prototype_GameObject_Waver")));
			m_vecObjects[WAVER].back()->Set_WorldMatrix(m_vecMapObjData[i].matWorld);
			break;
		case WIRE:
			m_vecObjects[WIRE].push_back(CGameInstance::Get_Instance()->Add_Clone_Return(4, TEXT("Wire"), TEXT("Prototype_GameObject_Grappling_Point")));
			m_vecObjects[WIRE].back()->Set_WorldMatrix(m_vecMapObjData[i].matWorld);
			break;
		default:
			break;
		}
	}

	m_vecMapObjData.clear();
}

void CImgui_Manager::Save_Map()
{
	for (_uint i = 0; i < OBJTYPE_END; ++i)
	{
		for (_uint j = 0; j < m_vecObjects[i].size(); ++j)
		{
			if (i != LIGHT) {
				MapObjData tData;
				tData.iObjType = i;
				tData.matWorld = m_vecObjects[i][j]->Get_Transform()->Get_WorldFloatOrigin4x4();
				tData.tLightDesc = {};

				m_vecMapObjData.push_back(tData);
			}
			else {
				MapObjData tData;
				tData.iObjType = i;
				tData.matWorld = m_vecObjects[i][j]->Get_Transform()->Get_WorldFloatOrigin4x4();
				tData.tLightDesc = *(dynamic_cast<CLight*>(m_vecObjects[i][j])->Get_LightDesc());

				m_vecMapObjData.push_back(tData);
			}
		}
	}

	string szBinFilePath;

	szBinFilePath += fileDialog.GetSelected().string();
	szBinFilePath += "\\";
	szBinFilePath += m_strMapName;
	szBinFilePath += ".bin";

	// 이진 파일 생성
	std::ofstream os(szBinFilePath, std::ios::binary);

#ifdef _DEBUG
	// 파일 생성 실패 시 오류 발생
	assert(os.is_open());
#endif // _DEBUG

	for (_uint i = 0; i < m_vecMapObjData.size(); ++i)
	{
		write_typed_data(os, m_vecMapObjData[i]);
	}

	os.close();

	m_vecMapObjData.clear();
}

void CImgui_Manager::Load_Map()
{
	string szBinFilePath;

	szBinFilePath += fileDialog.GetSelected().string();

	// 이진 파일 생성
	std::ifstream is(szBinFilePath, std::ios::binary);

	#ifdef _DEBUG
	// 파일 생성 실패 시 오류 발생
		assert(is.is_open());
	#endif // _DEBUG

		while (is.peek() != EOF) {
			MapObjData data;
			read_typed_data(is, data);
			m_vecMapObjData.push_back(data);
		}

		Make_Map();

		//m_vecMapObjData.clear();

	is.close();
}

void CImgui_Manager::Level_MapMake(_uint iMapNum)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	for (_uint i = 0; i < m_vecMapObjData.size(); i++) {
		switch (m_vecMapObjData[i].iObjType)
		{
		case LIGHT: {
			TransformDesc.vPosition = (_float4)&m_vecMapObjData[i].matWorld._41;
			auto obj = GAMEINSTANCE->Add_Light_Return(&m_vecMapObjData[i].tLightDesc);
			obj->Set_WorldMatrix(m_vecMapObjData[i].matWorld);
			break;}
		case PISTOLGUNNER: {
			TransformDesc.vPosition = (_float4)&m_vecMapObjData[i].matWorld._41;
			auto obj = CGameInstance::Get_Instance()->Add_Clone_Return(iMapNum, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_PistolGunner"),&TransformDesc);
			obj->Set_WorldMatrix(m_vecMapObjData[i].matWorld);
			break; }
		case UZIGUNNER: {
			TransformDesc.vPosition = (_float4)&m_vecMapObjData[i].matWorld._41;
			auto obj = CGameInstance::Get_Instance()->Add_Clone_Return(iMapNum, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Uzi"), &TransformDesc);
			obj->Set_WorldMatrix(m_vecMapObjData[i].matWorld);	
			break; }
		case WAVER: {
			TransformDesc.vPosition = (_float4)&m_vecMapObjData[i].matWorld._41;
			auto obj = CGameInstance::Get_Instance()->Add_Clone_Return(iMapNum, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Waver"),&TransformDesc);
			obj->Set_WorldMatrix(m_vecMapObjData[i].matWorld);
			break; }
		case WIRE: {
			TransformDesc.vPosition = (_float4)&m_vecMapObjData[i].matWorld._41;
			auto obj = CGameInstance::Get_Instance()->Add_Clone_Return(iMapNum, TEXT("Layer_MapObject"), TEXT("Prototype_GameObject_Grappling_Point"),&TransformDesc);
			obj->Set_WorldMatrix(m_vecMapObjData[i].matWorld);
			break; }
		default:
			break;
		}
	}

	m_vecMapObjData.clear();
}

void CImgui_Manager::Level_MapLoad(_uint _iMapNum)
{
	string szBinFilePath;
	_uint iMapNum = _iMapNum;

	szBinFilePath += "E:\\Github\\Dx11_Framework\\Binary\\Map\\";
	if(iMapNum == 3) szBinFilePath += "MapData01.bin";
	if(iMapNum == 5) szBinFilePath += "MapData02.bin";
	if (iMapNum == 52) { 
		szBinFilePath += "MapData02_2.bin"; 
		iMapNum = 5;
	}
	if (iMapNum == 6) szBinFilePath += "MapData03.bin";

	// 이진 파일 생성
	std::ifstream is(szBinFilePath, std::ios::binary);

#ifdef _DEBUG
	// 파일 생성 실패 시 오류 발생
	assert(is.is_open());
#endif // _DEBUG

	while (is.peek() != EOF) {
		MapObjData data;
		read_typed_data(is, data);
		m_vecMapObjData.push_back(data);
	}

	Level_MapMake(iMapNum);

	is.close();

}

CImgui_Manager* CImgui_Manager::Create(HWND hWnd, ID3D11Device* Device, ID3D11DeviceContext* pContext)
{
	CImgui_Manager* pInstance = new CImgui_Manager();

	pInstance->Initialize(hWnd, Device, pContext);

	return pInstance;
}

void CImgui_Manager::Free()
{
	__super::Free();

	for(_uint i = 0; i < OBJTYPE_END; ++i)
		m_vecObjects[i].clear();

	for(_uint i = 0; i < m_vecObjectNames.size(); ++i)
		m_vecObjectNames.clear();

	m_vecMapObjData.clear();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
