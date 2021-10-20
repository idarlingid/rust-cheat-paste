// dear imgui: standalone example application for DirectX 9
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
#include "utilities/utilities.hpp"

#include "encrypt-decrypt/encrypt-decrypt.hpp"

#include "menu/registrywindow.hpp"

#include "client/client.hpp"

#include "globals.hpp"
#include "syscalls/debugsecurity.h"
globals g_globals;

#include <Psapi.h>

#include <cassert>
#include "drv_mapper/intel_driver.hpp"
#include "drv_mapper/mapper.hpp"
#include <Blackbone/src/BlackBone/Process/Process.h>
#include <Blackbone/src/BlackBone/Misc/Utils.h>


#include "menu\ImGui\background.h"
#include "menu\ImGui\faprolight.hpp"

#include <signal.h>

#include "utilities/security.h"


#include "syscalls/syscalls.h"
using namespace std;
using namespace blackbone;

Process bb;
NTSTATUS _lastStatus;
#define TARGET_PROCESS xorstr_("RustClient.exe")

#include "menu\ImGui\font.h"
#include <string>
#include <random>
//#include "aes.h"
#include "RSAkey.h"
#include "xorstr2.h"


__forceinline bool inject()
{
    char request[512];
    std::string tempory_cipher_key;
    std::string tempory_iv_key;
    std::vector<std::string> vector_tempory_key;

    li(sprintf)(request, xorstr_("/api/session.php"));

    std::string unprotect_request = utilities::request_to_server(g_globals.server_side.ip, request);

    unprotect_request = aes::decrypt(unprotect_request, xorstr_("r92i5LrY0C5yqONI641qMQe3GA2mXqEt"), xorstr_("H7bbH9JfoFGLcu8z"));

    for (std::size_t pos = 0; pos < unprotect_request.size(); pos += 64)
        tempory_cipher_key = vector_tempory_key.emplace_back(unprotect_request.data() + pos, unprotect_request.data() + min(pos + 32, unprotect_request.size()));

    for (std::size_t pos = 0; pos < unprotect_request.size(); pos += 32)
        tempory_iv_key = vector_tempory_key.emplace_back(unprotect_request.data() + pos, unprotect_request.data() + min(pos + 32, unprotect_request.size()));
    std::string protect_request = aes::encrypt(unprotect_request.c_str(), tempory_cipher_key, tempory_iv_key);

    unprotect_request = aes::encrypt(unprotect_request.c_str(), xorstr_("r92i5LrY0C5yqONI641qMQe3GA2mXqEt"), xorstr_("H7bbH9JfoFGLcu8z")); // static keys
    li(sprintf)(request, xorstr_("/api/load.php?a=%s&b=%s"), unprotect_request.c_str(), protect_request.c_str());
    std::string response = utilities::request_to_server(g_globals.server_side.ip, request);
    std::vector<std::string> split_response = utilities::split_string(response.c_str(), xorstr_(";"));

    if (response == aes::encrypt(xorstr_("file_error"), g_globals.server_side.key.cipher, g_globals.server_side.key.iv))
        return NULL;

    for (int i = 0; i < response.size(); i++)
        response[i] = response[i] ^ Crypt::Key[i % (sizeof(Crypt::Key) / sizeof(char))];

    bb.mmap().MapImage(response.size(), response.data(), false, WipeHeader).status;

    client::Update();

    client::RenameAndDestroy();

    return _lastStatus;
}

auto ProcessName = "RustClient.exe";
__forceinline void subinjection()
{
    while (!(GetAsyncKeyState(VK_F2) & 0x8000));
    while (!utilities::ProcessExists(ProcessName)) {}
    _lastStatus = bb.Attach(utils::EnumProcess(TARGET_PROCESS));
    _lastStatus = inject();
}

__forceinline void Session()
{
    this_thread::sleep_for(chrono::seconds(240));
    sec::Logger(xorstr_("Session clossed because expired."), 0);
    sec::shutdown();
}   
__forceinline int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    static bool Rust1Day = false;
    static bool Rust7Days = false;
    static bool Rust15Days = false;
    static bool Rust30Days = false;
    HANDLE handle_mutex = li(OpenMutexA)(MUTEX_ALL_ACCESS, 0, xorstr_("CVhnIjfUtxUvddaNk67f6v9t9BH1U9Qr3mZZQWRm2AClp6Bwu8"));
    if (!handle_mutex)
        handle_mutex = li(CreateMutexA)(0, 0, xorstr_("CVhnIjfUtxUvddaNk67f6v9t9BH1U9Qr3mZZQWRm2AClp6Bwu8"));
    else
        return 0;

    DebugSelf();

    g_syscalls.init();

    std::thread sec_thread(debugsecurity::thread);
    sec_thread.detach();

    if (sec::TestSign())
    {
        li(MessageBoxA)(NULL, xorstr_("Your system under Test Signing mode, disable this, before launching loader."), utilities::get_random_string(16).c_str(), MB_SYSTEMMODAL | MB_OK);
        sec::shutdown();
    }
    if (utilities::ProcessExists(xorstr_("Alkad.exe")))
    {
        sec::killProcessByName(xorstr_("Alkad.exe"));
        sec::killProcessByName(xorstr_("RustClient.exe"));
        li(MessageBoxA)(NULL, xorstr_("Gamewer and Rust clossed for security purposses."), utilities::get_random_string(16).c_str(), MB_SYSTEMMODAL | MB_OK);
    }

/*    if (utilities::ProcessExists(xorstr_("RustClient.exe")))
    {
        sec::killProcessByName(xorstr_("RustClient.exe"));
        li(MessageBoxA)(NULL, xorstr_("Rust clossed for security purposses."), utilities::get_random_string(16).c_str(), MB_SYSTEMMODAL | MB_OK);
    }*/

    if (sec::IsDebuggersInstalledStart())
    {
        sec::shutdown();
    }

    std::thread first(sec::Session);
    first.detach();

    std::thread second(debugsecurity::thread);
    second.detach();

    std::thread third(sec::ST);
    third.detach();

    size_t input_key = 256; g_globals.client_side.data.key.resize(input_key);
    memset(g_globals.client_side.data.key.data(), 0, g_globals.client_side.data.key.size());

    size_t len;
    string savedCreditsPath; savedCreditsPath += xorstr_("./data.bin");
    ifstream inputFile(savedCreditsPath);
    if (inputFile.is_open())
    {
        string line1;
        getline(inputFile, line1);
        if (line1.length() > 1)
        {
            g_globals.client_side.data.key = aes::decrypt(line1, xorstr_("r92i5LrY0C5yqONI641qMQe3GA2mXqEt"), xorstr_("H7bbH9JfoFGLcu8z"));
        }
        inputFile.close();
    }

    if (client::authentication() != xorstr_("success"))
        sec::shutdown();

    client::valid_version();

    if (g_globals.server_side.version == g_globals.client_side.version && g_globals.server_side.status == xorstr_("Enabled"))
    {
        std::thread banthread(client::BanThread);
        banthread.detach();

        LPCTSTR lpzClass = utilities::get_random_string(16).c_str();

        if (!RegMyWindowClass(hInstance, lpzClass))
            return 1;

        RECT screen_rect;
        GetWindowRect(GetDesktopWindow(), &screen_rect);
        int x = screen_rect.right / 2.f - 150, y = screen_rect.bottom / 2.f - 75;
        HWND hWnd = CreateWindow(lpzClass, lpzClass, WS_POPUP, x, y, g_globals.client_side.window_settings.width, g_globals.client_side.window_settings.height, NULL, NULL, hInstance, NULL);
        if (!hWnd) return 2;
        LPDIRECT3D9 pD3D;
        if ((pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL) 
        {
            UnregisterClass(lpzClass, hInstance);
        }
        ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
        g_d3dpp.Windowed = TRUE;
        g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
        g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
        g_d3dpp.EnableAutoDepthStencil = TRUE;
        g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
        g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
        if (pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0) 
        {
            pD3D->Release();
            UnregisterClass(lpzClass, hInstance);
            return 1;
        }
        SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
        SetLayeredWindowAttributes(hWnd, RGB(255, 0x00, 0x00), 155, LWA_COLORKEY);
        ShowWindow(hWnd, SW_SHOWDEFAULT);
        UpdateWindow(hWnd);

        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        ImFontConfig font_cfg{};
        font_cfg.FontDataOwnedByAtlas = false;
        io.Fonts->AddFontFromFileTTF(xorstr_("C:/windows/fonts/calibri.ttf"), 24.f, &font_cfg, io.Fonts->GetGlyphRangesCyrillic());
        static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
        ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
        io.Fonts->AddFontFromMemoryTTF(&faprolight, sizeof faprolight, 36.f, &icons_config, icon_ranges);
        little = io.Fonts->AddFontFromFileTTF(xorstr_("C:/windows/fonts/calibri.ttf"), 14.f, &font_cfg, io.Fonts->GetGlyphRangesCyrillic());
        medium = io.Fonts->AddFontFromFileTTF(xorstr_("C:/windows/fonts/calibri.ttf"), 15.f, &font_cfg, io.Fonts->GetGlyphRangesCyrillic());

        if (back == nullptr)
            D3DXCreateTextureFromFileInMemoryEx(g_pd3dDevice, &background, sizeof(background), 1024, 1024, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &back);

        ImGui_ImplWin32_Init(hWnd);
        ImGui_ImplDX9_Init(g_pd3dDevice);
        ImGui::StyleColorsDark();
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        MSG msg;
        ZeroMemory(&msg, sizeof(msg));
        while (msg.message != WM_QUIT)
        {
            if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
                continue;
            }

            ImGui_ImplDX9_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            ImGui::GetStyle().AntiAliasedFill = 1.f;
            ImGui::GetStyle().AntiAliasedLines = 1.f;
            ImGui::GetStyle().CircleSegmentMaxError = 48;
            ImGui::GetStyle().CurveTessellationTol = 1.f;

            // FLAGS
            auto window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings;

            // ATRIBUTES
            ImGui::SetNextWindowSize({ 600, 350 });
            ImGui::SetNextWindowPos({ 0,0 });

            // STYLES
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0,0 });
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 16);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
            ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 1);

            // COLORS
            ImGui::PushStyleColor(ImGuiCol_WindowBg, { 255 / 255.f,255 / 255.f,255 / 255.f,255 / 255.f });
            ImGui::PushStyleColor(ImGuiCol_Text, { 152 / 255.f, 148 / 255.f, 149 / 255.f,255.f / 255.f });
            ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, { 35.f / 255.f,35.f / 255.f,35.f / 255.f,0 / 255.f });

            ImGui::Begin(xorstr_("Another Window"), nullptr, window_flags);
            {
                position = ImGui::GetWindowPos();
                draw = ImGui::GetWindowDrawList();

                draw->AddImage(back, { position.x, position.y }, { position.x + 600 ,position.y + 400 });

                ImDrawList* draw = ImGui::GetWindowDrawList();
                ImVec2 pos = ImGui::GetWindowPos();

                draw->AddRectFilled({ pos.x,pos.y }, { pos.x + 730,pos.y + 50 }, ImColor(19, 22, 26), 6.f, ImDrawCornerFlags_Top);
                draw->AddRectFilled({ pos.x ,pos.y + 20 }, { pos.x + 600,pos.y + 360 }, ImColor(25, 30, 35), 6.f, ImDrawCornerFlags_All);
                draw->AddRectFilled({ pos.x ,pos.y + 360 }, { pos.x + 600,pos.y + 360 }, ImColor(45, 50, 55), 6.f, ImDrawCornerFlags_Bot);
     
                ImGui::PushFont(little);
                draw->AddText({ position.x + 5 , position.y + 2 }, ImColor(255, 0 ,255, 200), xorstr_("steepcheat.pw"));
 /*               draw->AddText({ position.x + 460 - ImGui::CalcTextSize(xorstr_("session:")).x / 2, position.y + 2 }, ImColor(200, 200, 200, 200), xorstr_("session:"));
                draw->AddText({ position.x + 540 - ImGui::CalcTextSize(g_globals.server_side.key.iv.c_str()).x / 2, position.y + 2 }, ImColor(200, 200, 200, 200), g_globals.server_side.key.iv.c_str());*/
                ImGui::PopFont();

                ImGui::SetCursorPos({ 0, 20 });
                    

                if (tab == 0)
                {
                    draw->AddText({ position.x + 295 - ImGui::CalcTextSize(xorstr_("Key Activation")).x / 2, position.y + 80 }, ImColor(255, 255, 255), xorstr_("Key Activation"));
                    draw->AddText({ position.x + 300 - ImGui::CalcTextSize(ICON_FA_BOMB).x / 2, position.y + 50 }, ImColor(255, 255, 255), xorstr_(ICON_FA_BOMB));
                    ImGui::SetCursorPos({ 100, 130 });
                    ImGui::InputText(xorstr_("##Key"), g_globals.client_side.data.key.data(), input_key, ImGuiInputTextFlags_Password);
                    ImGui::SetCursorPos({ 140, 190 });

                    if (ImGui::Button(xorstr_("Submit"), ImVec2(315, 30)))
                    {
                        const auto activation = client::activation();

                        if (activation == aes::encrypt(xorstr_("banned"), g_globals.server_side.key.cipher, g_globals.server_side.key.iv))
                            banned = true;

                        else if (activation == aes::encrypt(xorstr_("invalid_key"), g_globals.server_side.key.cipher, g_globals.server_side.key.iv))
                            activation_invalid_key = true;

                        else if (activation == aes::encrypt(xorstr_("unknown_cheat"), g_globals.server_side.key.cipher, g_globals.server_side.key.iv))
                            activation_unknown_cheat = true;

                        else if (activation == aes::encrypt(xorstr_("expired_subscribe"), g_globals.server_side.key.cipher, g_globals.server_side.key.iv))
                            activation_expired_subscribe = true;

                        else if (activation == aes::encrypt(xorstr_("data_error"), g_globals.server_side.key.cipher, g_globals.server_side.key.iv))
                            activation_data_error = true;

                        else
                        {
                            g_globals.client_side.data.token = activation;
                            activation_success = true;
                            if (banned != true)
                            {
                                string savelogLog; savelogLog += xorstr_("./data.bin");
                                string logDir; logDir += xorstr_("./");
                                if (!utils::dirExists(logDir))
                                {
                                    CreateDirectory(logDir.c_str(), NULL);
                                }

                                ofstream outputFile(savelogLog);
                                if (outputFile.is_open())
                                {
                                    string logOut = aes::encrypt(g_globals.client_side.data.key.data(), xorstr_("r92i5LrY0C5yqONI641qMQe3GA2mXqEt"), xorstr_("H7bbH9JfoFGLcu8z")); logOut += xorstr_("\n");
                                    outputFile << logOut;
                                    outputFile.close();
                                }
                            }
                        }
                    }
                    ImGui::SetCursorPos({ 140, 240 });
                    if (ImGui::Button(xorstr_("Exit"), ImVec2(315, 30)))
                    {
                        sec::shutdown();
                    }
                    if (banned == true)
                    {
                        bool notifydone = false;
                        notification::start(xorstr_("Banned."), &notifydone);
                        if (notifydone)
                            banned = false;
                    }
                    if (activation_invalid_key == true)
                    {
                        bool notifydone = false;
                        notification::start(xorstr_("Invalid key."), &notifydone);
                        if (notifydone)
                        activation_invalid_key = false;
                    }
                    if (activation_unknown_cheat == true)
                    {
                        bool notifydone = false;
                        notification::start(xorstr_("Unknown cheat."), &notifydone);
                        if (notifydone)
                            activation_unknown_cheat = false;
                    }
                    if (activation_expired_subscribe == true)
                    {
                        bool notifydone = false;
                        notification::start(xorstr_("Key subscription expired."), &notifydone);
                        if (notifydone)
                            activation_expired_subscribe = false;
                    }
                    if (activation_data_error == true)
                    {
                        bool notifydone = false;
                        notification::start(xorstr_("Hardware Error."), &notifydone);
                        if (notifydone)
                            activation_data_error = false;
                    }
                    if (activation_success == true && activation_invalid_key != true && banned != true && activation_unknown_cheat != true && activation_expired_subscribe != true)
                    {
                        bool notifydone = false;
                        notification::start(xorstr_("Key successfully activated. Wait please."), &notifydone);
                        if (notifydone)
                        {
                            activation_success = false, tab = 1;
                        }
                    }
                }
                g_globals.client_side.data.structure_cheat = aes::decrypt(g_globals.client_side.data.token, g_globals.server_side.key.cipher, g_globals.server_side.key.iv);
                const auto split_cheat_structure = utilities::split_string(g_globals.client_side.data.structure_cheat, xorstr_(";"));

                if (tab == 1)
                {
                    static int subtab = 0;
                    draw->AddText({ position.x + 297 - ImGui::CalcTextSize(xorstr_("User Page")).x / 2, position.y + 70 }, ImColor(255, 255, 255), xorstr_("User Page"));
                    draw->AddText({ position.x + 302 - ImGui::CalcTextSize(ICON_FA_USER).x / 2, position.y + 40 }, ImColor(255, 0, 255), xorstr_(ICON_FA_USER));
                    ImGui::PushFont(little);
                    if (subtab == 0)
                    {
                        if (Rust1Day || Rust7Days || Rust15Days || Rust30Days)
                        {
                            draw->AddText({ position.x + 287 - ImGui::CalcTextSize(xorstr_("Subscription: ")).x / 2, position.y + 110 }, ImColor(255, 255, 255), xorstr_("Subscription: "));
                            draw->AddText({ position.x + 337 - ImGui::CalcTextSize(xorstr_("Rust")).x / 2, position.y + 110 }, ImColor(128, 255, 0), xorstr_("Rust"));

                            draw->AddText({ position.x + 267 - ImGui::CalcTextSize(xorstr_("Gamewer d: ")).x / 2, position.y + 130 }, ImColor(255, 255, 255), xorstr_("GameWer: "));
                            draw->AddText({ position.x + 328 - ImGui::CalcTextSize(split_cheat_structure[1].c_str()).x / 2, position.y + 130 }, ImColor(128, 255, 0), split_cheat_structure[1].c_str());
                            ImGui::SetCursorPos({ 140, 190 });
                            if (ImGui::Button(xorstr_("Start Now"), ImVec2(315, 30)))
                            {
                                ShowWindow(hWnd, SW_HIDE);
                                std::thread load(subinjection);
                                li(MessageBoxA)(NULL, xorstr_("Now start the game. And press F2."), utilities::get_random_string(16).c_str(), MB_SYSTEMMODAL | MB_OK);
                                load.detach();
                            }
                        }
                        draw->AddText({ position.x + 230 - ImGui::CalcTextSize(xorstr_("Expires: ")).x / 2, position.y + 150 }, ImColor(128, 255, 0), xorstr_("Expires in: "));
                        draw->AddText({ position.x + 328 - ImGui::CalcTextSize(split_cheat_structure[2].c_str()).x / 2, position.y + 150 }, ImColor(255, 255, 255), split_cheat_structure[2].c_str());
                        static bool hidewindow = false;
                        ImGui::SetCursorPos({ 140, 270 });
                        if (ImGui::Button(xorstr_("Log Out"), ImVec2(315, 30)))
                        {
                            tab = 0;
                        }
                        ImGui::SetCursorPos({ 140, 230 });
                        if (ImGui::Button(xorstr_("VK"), ImVec2(155, 30)))
                        {
                            ShellExecute(NULL, xorstr_("open"), xorstr_("https://vk.com/steepcheat"), NULL, NULL, SW_SHOWNORMAL);
                        }
                        ImGui::SetCursorPos({ 300, 230 });
                        if (ImGui::Button(xorstr_("Discord"), ImVec2(155, 30)))
                        {
                            ShellExecute(NULL, xorstr_("open"), xorstr_("https://discord.gg/cNUEbkt"), NULL, NULL, SW_SHOWNORMAL);
                        }
                    }
                    ImGui::PopFont();
                }
            
                for (int i = 0; i < (int)split_cheat_structure.size(); i++)
                {
                    if ((int)split_cheat_structure.size() == 3)
                    {
                        if (split_cheat_structure[0] == xorstr_("Rust 1 Day"))
                        {
                            if (split_cheat_structure[1] == xorstr_("detected"))
                            {
                                li(MessageBoxA)(NULL, xorstr_("Cheat is currently detected, sorry for the inconvenience."), utilities::get_random_string(16).c_str(), MB_SYSTEMMODAL | MB_ICONERROR);
                                return 0xDEAD;
                            }
                            if (split_cheat_structure[1] == xorstr_("on update"))
                            {
                                li(MessageBoxA)(NULL, xorstr_("Cheat is currently on update, sorry for the inconvenience."), utilities::get_random_string(16).c_str(), MB_SYSTEMMODAL | MB_ICONERROR);
                                return 0xDEAD;
                            }
                            else if (split_cheat_structure[1] == xorstr_("disabled"))
                            {
                                li(MessageBoxA)(NULL, xorstr_("Cheat is currently disabled, sorry for the inconvenience."), utilities::get_random_string(16).c_str(), MB_SYSTEMMODAL | MB_ICONERROR);
                                return 0xDEAD;
                            }
                            else if (split_cheat_structure[1] == xorstr_("undetected"))
                            {
                                Rust1Day = true;
                                Rust7Days = false;
                                Rust15Days = false;
                                Rust30Days = false;
                            }
                            else
                            {
                                li(MessageBoxA)(NULL, xorstr_("Unknown response"), utilities::get_random_string(16).c_str(), MB_SYSTEMMODAL | MB_ICONERROR);
                                return 0xDEAD;
                            }
                        }
                        if (split_cheat_structure[0] == xorstr_("Rust 7 Days"))
                        {
                            if (split_cheat_structure[1] == xorstr_("detected"))
                            {
                                li(MessageBoxA)(NULL, xorstr_("Cheat is currently detected, sorry for the inconvenience."), utilities::get_random_string(16).c_str(), MB_SYSTEMMODAL | MB_ICONERROR);
                                return 0xDEAD;
                            }
                            if (split_cheat_structure[1] == xorstr_("on update"))
                            {
                                li(MessageBoxA)(NULL, xorstr_("Cheat is currently on update, sorry for the inconvenience."), utilities::get_random_string(16).c_str(), MB_SYSTEMMODAL | MB_ICONERROR);
                                return 0xDEAD;
                            }
                            else if (split_cheat_structure[1] == xorstr_("disabled"))
                            {
                                li(MessageBoxA)(NULL, xorstr_("Cheat is currently disabled, sorry for the inconvenience."), utilities::get_random_string(16).c_str(), MB_SYSTEMMODAL | MB_ICONERROR);
                                return 0xDEAD;
                            }
                            else if (split_cheat_structure[1] == xorstr_("undetected"))
                            {
                                Rust1Day = false;
                                Rust7Days = true;
                                Rust15Days = false;
                                Rust30Days = false;
                            }
                            else
                            {
                                li(MessageBoxA)(NULL, xorstr_("Unknown response"), utilities::get_random_string(16).c_str(), MB_SYSTEMMODAL | MB_ICONERROR);
                                return 0xDEAD;
                            }
                        }
                        if (split_cheat_structure[0] == xorstr_("Rust 15 Days"))
                        {
                            if (split_cheat_structure[1] == xorstr_("detected"))
                            {
                                li(MessageBoxA)(NULL, xorstr_("Cheat is currently detected, sorry for the inconvenience."), utilities::get_random_string(16).c_str(), MB_SYSTEMMODAL | MB_ICONERROR);
                                return 0xDEAD;
                            }
                            if (split_cheat_structure[1] == xorstr_("on update"))
                            {
                                li(MessageBoxA)(NULL, xorstr_("Cheat is currently on update, sorry for the inconvenience."), utilities::get_random_string(16).c_str(), MB_SYSTEMMODAL | MB_ICONERROR);
                                return 0xDEAD;
                            }
                            else if (split_cheat_structure[1] == xorstr_("disabled"))
                            {
                                li(MessageBoxA)(NULL, xorstr_("Cheat is currently disabled, sorry for the inconvenience."), utilities::get_random_string(16).c_str(), MB_SYSTEMMODAL | MB_ICONERROR);
                                return 0xDEAD;
                            }
                            else if (split_cheat_structure[1] == xorstr_("undetected"))
                            {
                                Rust1Day = false;
                                Rust7Days = false;
                                Rust15Days = true;
                                Rust30Days = false;
                            }
                            else
                            {
                                li(MessageBoxA)(NULL, xorstr_("Unknown response"), utilities::get_random_string(16).c_str(), MB_SYSTEMMODAL | MB_ICONERROR);
                                return 0xDEAD;
                            }
                        }
                        if (split_cheat_structure[0] == xorstr_("Rust 30 days"))
                        {
                            if (split_cheat_structure[1] == xorstr_("detected"))
                            {
                                li(MessageBoxA)(NULL, xorstr_("Cheat is currently detected, sorry for the inconvenience."), utilities::get_random_string(16).c_str(), MB_SYSTEMMODAL | MB_ICONERROR);
                                return 0xDEAD;
                            }
                            if (split_cheat_structure[1] == xorstr_("on update"))
                            {
                                li(MessageBoxA)(NULL, xorstr_("Cheat is currently on update, sorry for the inconvenience."), utilities::get_random_string(16).c_str(), MB_SYSTEMMODAL | MB_ICONERROR);
                                return 0xDEAD;
                            }
                            else if (split_cheat_structure[1] == xorstr_("disabled"))
                            {
                                li(MessageBoxA)(NULL, xorstr_("Cheat is currently disabled, sorry for the inconvenience."), utilities::get_random_string(16).c_str(), MB_SYSTEMMODAL | MB_ICONERROR);
                                return 0xDEAD;
                            }
                            else if (split_cheat_structure[1] == xorstr_("undetected"))
                            {
                                Rust1Day = false;
                                Rust7Days = false;
                                Rust15Days = false;
                                Rust30Days = true;
                            }
                            else
                            {
                                li(MessageBoxA)(NULL, xorstr_("Unknown response"), utilities::get_random_string(16).c_str(), MB_SYSTEMMODAL | MB_ICONERROR);
                                return 0xDEAD;
                            }
                        }
                    }
                    else
                    {
                        return 0xDEAD;
                    }
                }
            }

            ImGui::End();

            // STYLES
            ImGui::PopStyleVar(4);

            // COLORS
            ImGui::PopStyleColor(3);

            ImGui::EndFrame();
            g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, false);
            g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
            g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
            D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * 255.0f), (int)(clear_color.y * 255.0f), (int)(clear_color.z * 255.0f), (int)(clear_color.w * 255.0f));
            g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
            if (g_pd3dDevice->BeginScene() >= 0)
            {
                ImGui::Render();
                ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
                g_pd3dDevice->EndScene();
            }
            HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

            // Handle loss of D3D9 device
        }

        ImGui_ImplDX9_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        CleanupDeviceD3D();
        DestroyWindow(hWnd);
        //if (g_pd3dDevice) g_pd3dDevice->Release();
        //if (pD3D) pD3D->Release();
        UnregisterClass(lpzClass, hInstance);

        return 0;
    }
}

// Helper functions


// Win32 message handler
