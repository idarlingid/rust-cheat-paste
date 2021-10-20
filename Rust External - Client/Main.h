#pragma once

#include <windows.h>
#include <fstream>
#include <string>
#include <vector>
#include <tlhelp32.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dwmapi.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dwmapi.lib")

HWND Wnd = NULL;
RECT GameRect = { NULL };
HWND GameWnd = NULL;
IDirect3D9Ex* pObject = NULL;
IDirect3DDevice9Ex* pDevice = NULL;
D3DPRESENT_PARAMETERS pParams = { NULL };

DWORD ScreenCenterX;
DWORD ScreenCenterY;
int Width = GetSystemMetrics(SM_CXSCREEN);
int Height = GetSystemMetrics(SM_CYSCREEN);
const MARGINS Margin = { -1 };
MSG Message = { NULL };
bool ShowMenu = false;
bool Initialised = false;

void ClearD3D() {
    if (pDevice != NULL) {
        pDevice->EndScene();
        pDevice->Release();
    }
    if (pObject != NULL) {
        pObject->Release();
    }
}

