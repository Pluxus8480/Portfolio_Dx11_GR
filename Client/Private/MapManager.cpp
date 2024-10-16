//#include "MapManager.h"
//#include "GameObject.h"
//#include "Transform.h"
//#include "GameInstance.h"
//#include "VIBuffer.h"
//
//
//wstring CMap_Manager::strLayers[OBJTYPE_END] = { 
//	L"Monster", 
//	L"Wall" 
//};
//
//wstring CMap_Manager::strPrototypeTags[OBJTYPE_END] = { 
//	L"Prototype_Monster", 
//	L"Prototype_Wall" 
//};
//
//
//HRESULT CMap_Manager::Save_Map(vector<CGameObject*>* vecObjects)
//{
//	_tchar szName[MAX_PATH] = {};
//
//	OPENFILENAME ofn = {};
//
//	ofn.lStructSize = sizeof(OPENFILENAME);
//	ofn.hwndOwner = g_hWnd;
//	ofn.lpstrFile = szName;
//	ofn.nMaxFile = sizeof(szName);
//	ofn.lpstrFilter = L".dat";
//	ofn.lpstrInitialDir = L"../Client/Bin/Resources/DataFiles/";
//	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
//
//	if (GetSaveFileName(&ofn))
//	{
//		HANDLE		hFile = CreateFile(szName, // 파일 경로와 이름을 명시
//			GENERIC_WRITE, // 파일 접근 모드(GENERIC_WRITE : 쓰기 전용, GENERIC_READ : 읽기 전용)
//			NULL, // 공유 방식, 파일이 개방된 상태에서 다른 프로세스가 파일 개방을 하는 것을 허가할 것인가(NULL 공유하지 않음)
//			NULL, // 보안 속성, NULL 인 경우 기본 보안 상태
//			CREATE_ALWAYS,	// 생성 방식, (CREATE_ALWAYS : 파일이 없다면 생성, 있다면 덮어쓰기, OPEN_EXISTING : 파일이 있을 경우에만 열기)
//			FILE_ATTRIBUTE_NORMAL, // 파일 속성(읽기 전용, 숨김 파일 등등),FILE_ATTRIBUTE_NORMAL : 특수 속성이 없는 일반 파일 형태 
//			NULL); // 생성될 파일의 속성을 제공할 템플릿 파일 ( 사용하지 않기 때문에 NULL)a
//
//		if (INVALID_HANDLE_VALUE == hFile)
//		{
//			assert(false);
//			return E_FAIL;
//		}
//
//		DWORD dwByte(0);
//
//		for (_uint i = 0; i < OBJTYPE_END; ++i)
//		{
//			_uint iVecSize = (_uint)vecObjects[i].size();
//			WriteFile(hFile, &iVecSize, sizeof(_uint), &dwByte, nullptr);
//
//			wstring strLayer = strLayers[i];
//			_uint iLayerLength = (_uint)strLayer.size();
//			wstring strPrototypeTag = strPrototypeTags[i];
//			_uint iPrototypeTagLength = (_uint)strPrototypeTag.size();
//
//			for (CGameObject*& pObj : vecObjects[i])
//			{
//				_float4x4 worldMatrix = pObj->Get_Transform()->Get_WorldFloatOrigin4x4();
//
//				WriteFile(hFile, &worldMatrix, sizeof(_float4x4), &dwByte, nullptr);
//				WriteFile(hFile, &iLayerLength, sizeof(_uint), &dwByte, nullptr);
//				WriteFile(hFile, strLayer.c_str(), sizeof(_tchar) * iLayerLength, &dwByte, nullptr);
//				WriteFile(hFile, &iPrototypeTagLength, sizeof(_uint), &dwByte, nullptr);
//				WriteFile(hFile, strPrototypeTag.c_str(), sizeof(_tchar) * iPrototypeTagLength, &dwByte, nullptr);
//
//			}
//		}
//
//		CloseHandle(hFile);
//	}
//
//
//
//	return S_OK;
//}
//
//
//HRESULT CMap_Manager::Load_Map(vector<class CGameObject*>* vecObjects)
//{
//	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
//	//pGameInstance->Clear_Objects(LEVEL_TOOL);
//
//	if (nullptr == pGameInstance->Add_Clone_Return(LEVEL_TOOL, L"Player", TEXT("Prototype_Player")))
//		return E_FAIL;
//
//	_tchar szName[MAX_PATH] = {};
//
//	OPENFILENAME ofn = {};
//
//	ofn.lStructSize = sizeof(OPENFILENAME);
//	ofn.hwndOwner = g_hWnd;
//	ofn.lpstrFile = szName;
//	ofn.nMaxFile = sizeof(szName);
//	ofn.lpstrFilter = L"ALL\0*.*\0*.dat";
//	ofn.lpstrInitialDir = L"../Client/Bin/Resources/DataFiles/";
//	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
//
//	if (GetOpenFileName(&ofn))
//	{
//		HANDLE		hFile = CreateFile(szName, // 파일 경로와 이름을 명시
//			GENERIC_READ, // 파일 접근 모드(GENERIC_WRITE : 쓰기 전용, GENERIC_READ : 읽기 전용)
//			NULL, // 공유 방식, 파일이 개방된 상태에서 다른 프로세스가 파일 개방을 하는 것을 허가할 것인가(NULL 공유하지 않음)
//			NULL, // 보안 속성, NULL 인 경우 기본 보안 상태
//			OPEN_EXISTING,	// 생성 방식, (CREATE_ALWAYS : 파일이 없다면 생성, 있다면 덮어쓰기, OPEN_EXISTING : 파일이 있을 경우에만 열기)
//			FILE_ATTRIBUTE_NORMAL, // 파일 속성(읽기 전용, 숨김 파일 등등),FILE_ATTRIBUTE_NORMAL : 특수 속성이 없는 일반 파일 형태 
//			NULL); // 생성될 파일의 속성을 제공할 템플릿 파일 ( 사용하지 않기 때문에 NULL)
//
//		if (INVALID_HANDLE_VALUE == hFile)
//		{
//			assert(false);
//			return E_FAIL;
//		}
//
//		for (_uint i = 0; i < (_uint)OBJTYPE_END; ++i)
//			vecObjects[i].clear();
//
//		DWORD dwByte(0);
//		for (_uint i = 0; i < (_uint)OBJTYPE_END; ++i)
//		{
//			_uint iVecSize = 0;
//			int iReadFlag = (int)ReadFile(hFile, &iVecSize, sizeof(_uint), &dwByte, nullptr);
//
//			vecObjects[i].reserve(iVecSize);
//			for (_uint j = 0; j < iVecSize; ++j)
//			{
//				_float4x4 worldMatrix = {};
//				_uint iTextureIndex = 0;
//				_uint iLayerStrLength = 0;
//				_tchar szLayer[MAX_PATH] = {};
//				_uint iPrototypeTagLength = 0;
//				_tchar szPrototypeTag[MAX_PATH] = {};
//				_float3 vColliderOffset;
//				_float3 vColliderScale;
//
//				iReadFlag += (int)ReadFile(hFile, &worldMatrix, sizeof(_float4x4), &dwByte, nullptr);
//				iReadFlag += (int)ReadFile(hFile, &iLayerStrLength, sizeof(_uint), &dwByte, nullptr);
//				iReadFlag += (int)ReadFile(hFile, szLayer, sizeof(_tchar) * iLayerStrLength, &dwByte, nullptr);
//				iReadFlag += (int)ReadFile(hFile, &iPrototypeTagLength, sizeof(_uint), &dwByte, nullptr);
//				iReadFlag += (int)ReadFile(hFile, szPrototypeTag, sizeof(_tchar) * iPrototypeTagLength, &dwByte, nullptr);
//
//				if (iReadFlag != 6)
//					return E_FAIL;
//
//				auto pObj = CGameInstance::Get_Instance()->Add_Clone_Return(LEVEL_TOOL, szLayer, szPrototypeTag);
//				pObj->Get_Transform()->Set_WorldMatrix(worldMatrix);
//
//				vecObjects[i].push_back(pObj);
//				iReadFlag = 1;
//			}
//		}
//
//		CloseHandle(hFile);
//	}
//
//	return S_OK;
//}
