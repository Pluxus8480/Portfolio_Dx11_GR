#pragma once

#include <process.h>
//#include <mfapi.h>
//#include <mfplay.h>
//#include <mfreadwrite.h>

/* Ŭ���̾�Ʈ���� �����ϴ� ��� Ŭ�������� ���������� ���� ����ϴ� ���ǵ��� ��Ƶд�. */
namespace Client
{
	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;

	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_TOOL, LEVEL_LAB, LEVEL_LAST, LEVEL_END };
}

extern HWND				g_hWnd;
extern HINSTANCE		g_hInst;

using namespace Client;