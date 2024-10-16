#pragma once

#pragma warning (disable : 4005)
#pragma warning (disable : 4828)
#pragma warning (disable : 4251)
#define _CRT_SECURE_NO_WARNINGS

#include <d3d11.h>

#include <Engine_Option.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <d3dcompiler.h>

#pragma region Imgui
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "ImGuizmo.h"
#include <imfilebrowser.h>
#pragma	endregion

#pragma region assimp
#include <assimp\scene.h>
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#pragma	endregion



#include <Fx11\d3dx11effect.h>
#include <directxtk\VertexTypes.h>
#include <directxtk\PrimitiveBatch.h>
#include <directxtk\Effects.h>
#include <directxtk\DDSTextureLoader.h>
#include <directxtk\WICTextureLoader.h>
#include <directxtk\\SpriteBatch.h>
#include <directxtk\\SpriteFont.h>
#include <directxtk\ScreenGrab.h>

#pragma region PhysX
#include <PxPhysicsAPI.h>
#include <PxPhysics.h>
using namespace physx;
#pragma endregion

using namespace DirectX;
using namespace std;

#include <map>
#include <list>
#include <vector>
#include <algorithm>
#include <string>
#include <random>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <deque>

#ifdef _UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

#include <iostream>
#include <fstream>
#include <filesystem>

#include "Engine_Macro.h"
#include "Engine_Enum.h"
#include "Engine_Function.h"
#include "Engine_Typedef.h"
#include "Engine_Struct.h"

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include "dxgidebug.h"

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 

#endif // _DEBUG

#else // _DEBUG
#define DBG_NEW new

#endif // _DEBUG

#include "BoneData.h"
#include "MeshData.h"
#include "ModelData.h"

using namespace Engine;

