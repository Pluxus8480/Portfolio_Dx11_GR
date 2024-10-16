#pragma once


namespace Engine
{
	typedef struct
	{
		HWND			hWnd;
		unsigned int	iWinSizeX;
		unsigned int	iWinSizeY;
		bool			isWindowed;
	}ENGINE_DESC;

	typedef struct ENGINE_DLL VertestPT
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		static const unsigned int	iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXPOSTEX;

	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vPSize;
	}VTXPOINT;


	typedef struct ENGINE_DLL VertexPNT
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		static const unsigned int	iNumElements = { 3 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXNORTEX;

	typedef struct ENGINE_DLL VertexMesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		static const unsigned int	iNumElements = { 4 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXMESH;

	typedef struct tagVertex_AnimModel
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;
		XMUINT4			vBlendIndex;
		XMFLOAT4		vBlendWeight;

		tagVertex_AnimModel()
		{
			ZeroMemory(this, sizeof(tagVertex_AnimModel));
		}
	}VTXANIM;

	typedef struct ENGINE_DLL VertexAnimMesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		/* 이 정점은 어떤 뼈들의 상태를 받아와야하는가? 저장의 형태 : 뼈의 인덱스로 저장.*/
		/* 전체 뼈 중의 인덱스를 의미하는게 아니다. */
		/* 이 메시에 영향을 주는 뼈들 중에서의 인덱스를 지정하는거다. */
		XMUINT4			vBlendIndices;

		/* 위에서 정의한 각각의 뼈가 몇 프로나 정점에 적용되야하는지에 대한 정보를 정규화한 실수로 저장한다. */
		XMFLOAT4		vBlendWeights;

		static const unsigned int	iNumElements = { 6 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXANIMMESH;

	typedef struct
	{
		class CTexture* pTextures[AI_TEXTURE_TYPE_MAX];
	}MATERIAL_MESH;

	typedef struct PhysXColliderDesc
	{
		PhysXColliderDesc()
		{
			ZeroMemory(this, sizeof(PhysXColliderDesc));
		}

		PxConvexMesh* pConvecMesh;
		PxMaterial* pMaterial;
		PHYSXACTOR_TYPE		eActorType;
		XMVECTOR			vPosition;
		XMVECTOR			vAngles;
		PHYSXCOLLIDER_TYPE	eShape;
		_uint				iFilterType;
		XMVECTOR			vScale;
		float				fDensity;
		_bool				bTrigger;

	}PHYSXCOLLIDERDESC;

	typedef struct
	{
		float				fTrackPosition;
		XMFLOAT3			vTranslation;
		XMFLOAT3			vScale;
		XMFLOAT4			vRotation;
		float				fTime;
	}KEYFRAME;

	typedef struct tag_MeshVertextPostion
	{
		_float3 vMin;
		_float3 vMax;
		_float3 vCenter;

	} MESH_VTX_INFO;

	typedef struct tagLineIndices32
	{
		unsigned long _1, _2;
	}LINEINDICES32;

	typedef struct tagLineIndices16
	{
		unsigned short _1, _2;
	}LINEINDICES16;

	typedef struct tagFaceIndices32
	{
		unsigned long _1, _2, _3;
		int	numIndices;
	}FACEINDICES32;

	typedef struct tagFaceIndices16
	{
		unsigned short _1, _2, _3;
	}FACEINDICES16;

	typedef struct tagVertex_Position
	{
		_float3		vPosition;

		static const unsigned int	iNumElements = { 1 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXPOS;

	typedef struct tagVertex_Ground_Texture
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexUV;
		XMFLOAT3		vTangent;
	}VTXGROUND;

	typedef struct LightDesc
	{
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_SPOT, TYPE_END };
		TYPE		eType;
		_char		szName[MAX_PATH] = {};
		XMFLOAT4	vDiffuse;
		XMFLOAT4	vAmbient;
		XMFLOAT4	vSpecular;

		XMFLOAT4	vDirection;
		XMFLOAT4	vPosition;
		float		fRange;
		float		g_fSpotInnerConeAngle;
		float		g_fSpotOuterConeAngle;
		float       g_fLightFalloff;

	}LIGHT_DESC;

	typedef struct ENGINE_DLL VertexCube
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vTexcoord;

		static const unsigned int	iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXCUBE;

	typedef struct VertexMatrix
	{
		XMFLOAT4		vRight, vUp, vLook, vTranslation;
		XMFLOAT2		vLifeTime;
	}VTXMATRIX;

	typedef struct ENGINE_DLL VertexRectInstance
	{
		static const unsigned int	iNumElements = { 7 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXRECTINSTANCE;

	typedef struct ENGINE_DLL VertexPtInstance
	{
		static const unsigned int	iNumElements = { 7 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXPOINTINSTANCE;


	typedef struct ENGINE_DLL MapObjData
	{
		_uint			iObjType;
		_float4x4		matWorld;
		LightDesc		tLightDesc;
	}MAPDATA;

}
