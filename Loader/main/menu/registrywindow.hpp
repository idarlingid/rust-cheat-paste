#pragma once
#include <iostream>
#include <windows.h>
#include <thread>
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_internal.h"

#include "ImGui/imgui_internal.h"
#include "../globals.hpp"

#include <d3dx9.h>
#include <d3d9.h>
#include <cmath>
#include <random>
#include "ImGui/hashes.h"
#include "ImGui/imgui_rotate.hpp"
#include "ImGui/font.h"
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib,"dxguid.lib")

#define rgba_to_float(r,g,b,a) (float)r/255.0f, (float)g/255.0f, (float)b/255.0f, (float)a/255.0f

ImFont* Normal = nullptr;
ImFont* Main = nullptr;
ImFont* Notification = nullptr;
ImFont* Medium = nullptr;
ImFont* MyFont = nullptr;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp;

static int alpha = 0;
static int alpha2 = 0;
static int timer = 0;
static int tab = 0;
static bool banned = false;
static bool activation_invalid_key = false;
static bool activation_unknown_cheat = false;
static bool activation_expired_subscribe = false;
static bool activation_data_error = false;
static bool injection = false;
static bool activation_success = false;
static bool logout = false;
extern ImVec2 position;
extern ImDrawList* draw;

bool reverse = false;
int offset = 0;
bool show_popup = false;
ImVec2 position;
ImDrawList* draw;
static LPDIRECT3D9 g_pD3D = NULL;
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();

HWND MainWindow;
IDirect3DTexture9* tImage = nullptr;
IDirect3DTexture9* back = nullptr;
ATOM RegMyWindowClass(HINSTANCE, LPCTSTR);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
ATOM RegMyWindowClass(HINSTANCE hInst, LPCTSTR lpzClassName)
{
    WNDCLASS wcWindowClass = { 0 };
    wcWindowClass.lpfnWndProc = (WNDPROC)WndProc;
    wcWindowClass.style = CS_HREDRAW | CS_VREDRAW;
    wcWindowClass.hInstance = hInst;
    wcWindowClass.lpszClassName = lpzClassName;
    wcWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcWindowClass.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE;
    return RegisterClass(&wcWindowClass);
}

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}
void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            ImGui_ImplDX9_InvalidateDeviceObjects();
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
            if (hr == D3DERR_INVALIDCALL) IM_ASSERT(0);
            ImGui_ImplDX9_CreateDeviceObjects();
        }
        return NULL;

    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) return NULL;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return NULL;
    case WM_NCHITTEST:
        POINT pt;
        RECT windowPos;
        GetWindowRect(hWnd, &windowPos);
        static RECT r1;
        r1.left = 0;
        r1.right = windowPos.right - windowPos.left;
        r1.top = 0;
        r1.bottom = 40;
        GetCursorPos(&pt);
        ScreenToClient(hWnd, &pt);
        LRESULT result = DefWindowProc(hWnd, msg, wParam, lParam);
        if ((int)r1.bottom > (int)pt.y)
        {
            if (result == HTCLIENT) result = HTCAPTION;
        }
        return result;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}
namespace notification
{
    long getMils()
    {
        auto duration = std::chrono::system_clock::now().time_since_epoch();

        return std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    }

    void start(const char* text, bool* done)
    {
        if (!done)
            show_popup = true;

        if (&show_popup)
        {
            if (timer < 6000)
            {
                if (alpha < 240)
                    alpha = alpha + 5;

                if (alpha2 < 255)
                    alpha2 = alpha2 + 8;
            }

            if (timer < 6000)
                timer = timer + 50;

            if (timer > 6000)
                timer = 6000;

            if (timer >= 6000)
            {
                if (alpha > 0)
                    alpha = alpha - 5;

                if (alpha2 > 0) 
                    alpha2 = alpha2 - 8;

                if (alpha <= 0 && alpha2 <= 0)
                {
                    alpha = 0;
                    timer = -1;
                    show_popup = false;
                    *done = true;
                }
            }
            if (timer <= 6000 && alpha > 0 && alpha2 > 0)
            {
                draw->AddRectFilled({ position.x,position.y }, { position.x + 600, position.y + 350 }, ImColor(0, 0, 0, alpha));
                ImGui::PushFont(medium);
                ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x / 2 - ImGui::CalcTextSize(text).x / 2, ImGui::GetWindowSize().y / 2 - ImGui::CalcTextSize(text).y / 2 + 22));
                draw->AddText({ ImGui::GetWindowSize().x / 2 - ImGui::CalcTextSize(text).x / 2, ImGui::GetWindowSize().y / 2 - ImGui::CalcTextSize(text).y / 2 }, ImColor(255, 255, 255, alpha2), text);
                ImGui::PopFont();
            }        
        }
    }
}