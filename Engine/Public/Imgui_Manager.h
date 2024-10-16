#pragma once

#include "Base.h"
#include "ObjEnum.h"


BEGIN(Engine)

enum class eImgui_State
{
	IMGUI_STATE_WAIT,
	IMGUI_STATE_RENDER,
	IMGUI_STATE_SHUTDOWN,
	IMGUI_STATE_END
};

enum class eMapLoadType
{
	MAP_LOAD,
	MAP_SAVE,
	MAP_END
};

using GIZMODESC = struct tagGizmoDesc
{
	ImGuizmo::MODE CurrentGizmoMode = ImGuizmo::WORLD;
	ImGuizmo::OPERATION CurrentGizmoOperation = ImGuizmo::TRANSLATE;
	bool bUseSnap = false;
	bool boundSizing = false;
	bool boundSizingSnap = false;
	float snap[3] = { 0.1f, 0.1f, 0.1f };
	float bounds[6] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
	float boundsSnap[3] = { 0.1f, 0.1f, 0.1f };
};

class CImgui_Manager : public CBase
{
private:
	CImgui_Manager();
	virtual ~CImgui_Manager() = default;

public:
	void	Initialize(HWND hWnd, ID3D11Device* Device, ID3D11DeviceContext* pContext);
	HRESULT Imgui_ProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HRESULT Imgui_ViewInit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	void    PreTick();
	void    Tick();
	void	LateTick();

	void	Imgui_Render();
	void    Imgui_Main();

	void	Imgui_GizmoRender();
	void	Imgui_MenuBar();

	void	Imgui_ObjCreate();

private:
	void    Imgui_MapDataControl();

//Imgui Control Function
public:
	void			Imgui_SetState(eImgui_State eState) { m_eState = eState; }
	eImgui_State	Imgui_GetState() { return m_eState; }

	void 			Imgui_SetSelectObject(CGameObject* pObject) { m_pSelectObject = pObject; }
	vector<CGameObject*>* Get_Objects() { return m_vecObjects; }
	void			Clear_Objects();

public:
	void			Make_Map();
	void			Save_Map();
	void			Load_Map();

	void			Level_MapMake(_uint iMapNum);
	void	        Level_MapLoad(_uint iMapNum);
private:
	_bool m_bRotCheck = false;
	_float3 m_vRot = {0.f,0.f,0.f};

private:
	eImgui_State m_eState = eImgui_State::IMGUI_STATE_WAIT;

	eMapLoadType m_eMapLoadType = eMapLoadType::MAP_END;

	CGameObject* m_pSelectObject = nullptr;

	tagGizmoDesc m_tGizmoDesc;

	eObjectType m_eObjectType = PISTOLGUNNER;

	vector<class CGameObject*> m_vecObjects[OBJTYPE_END];
	
	vector<MapObjData> m_vecMapObjData;

	string m_strMapName = "MapData01";

	vector<string> m_vecObjectNames;
private:
	ImGui::FileBrowser fileDialog;

public:
	static CImgui_Manager* Create(HWND hWnd, ID3D11Device* Device, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
