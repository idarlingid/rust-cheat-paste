
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include <Windows.h>
#include <fstream>
#include <string>
// DirectX
#include <d3d9.h>



#pragma comment(lib, "WinINet.lib")

#include "WndProc.h"
#include "AES/AES.h"
#include "AES/md5.h"
#include "Utilities/skCrypt.h"

#include "WebRequest/WebRequest.h"
#include "Utilities/Utilities.h"
#include "global.h"

//#pragma comment(lib, "BlackBone.lib")
#define def_en(str) aes::encrypt(str, (string)_("r09y7LrY1C4yqONI641qMQe7GA5mQvdf"), (string)_("H1ggF9foFGLerr8q"))
#define def_de(str) aes::decrypt(str, (string)_("r09y7LrY1C4yqONI641qMQe7GA5mQvdf"), (string)_("H1ggF9foFGLerr8q"))
#define LNAME _(L"SteepCheat")
#define NAME _("SteepCheat")
#define WINDOW_HEIGHT 200
#define WINDOW_WIDTH 300


void AES() {
	/*auto key = aes::encrypt("973DMK9MS", "r09y7LrY1C4yqONI641qMQe7GA5mQvdf", "H1ggF9foFGLerr8q");
std::cout << key;*/
//auto dec = aes::decrypt("pa4JDzKffF/qI8133uryMWQXmhlMv4pXYDB8mbzJgQ==", "r09y7LrY1C4yqONI641qMQe7GA5mQvdf", "H1ggF9foFGLerr8q");
//std::cout << dec;
}

//int RunCheatFromMemory();
//#include "rawDriver.h"
#include "rawCheat.h"

// Win32 message handler

int RunExeFromMemory(void* pe) {

	IMAGE_DOS_HEADER* DOSHeader;
	IMAGE_NT_HEADERS64* NtHeader;
	IMAGE_SECTION_HEADER* SectionHeader;

	PROCESS_INFORMATION PI;
	STARTUPINFOA SI;
	ZeroMemory(&PI, sizeof(PI));
	ZeroMemory(&SI, sizeof(SI));


	void* pImageBase;

	char currentFilePath[1024];

	DOSHeader = PIMAGE_DOS_HEADER(pe);
	NtHeader = PIMAGE_NT_HEADERS64(DWORD64(pe) + DOSHeader->e_lfanew);

	if (NtHeader->Signature == IMAGE_NT_SIGNATURE) {

		GetModuleFileNameA(NULL, currentFilePath, MAX_PATH);
		//create process
		if (CreateProcessA(currentFilePath, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &SI, &PI)) {

			CONTEXT* CTX;
			CTX = LPCONTEXT(VirtualAlloc(NULL, sizeof(CTX), MEM_COMMIT, PAGE_READWRITE));
			CTX->ContextFlags = CONTEXT_FULL;


			UINT64 imageBase = 0;
			if (GetThreadContext(PI.hThread, LPCONTEXT(CTX))) {
				pImageBase = VirtualAllocEx(
					PI.hProcess,
					LPVOID(NtHeader->OptionalHeader.ImageBase),
					NtHeader->OptionalHeader.SizeOfImage,
					MEM_COMMIT | MEM_RESERVE,
					PAGE_EXECUTE_READWRITE
				);


				WriteProcessMemory(PI.hProcess, pImageBase, pe, NtHeader->OptionalHeader.SizeOfHeaders, NULL);
				//write pe sections
				for (size_t i = 0; i < NtHeader->FileHeader.NumberOfSections; i++)
				{
					SectionHeader = PIMAGE_SECTION_HEADER(DWORD64(pe) + DOSHeader->e_lfanew + 264 + (i * 40));

					WriteProcessMemory(
						PI.hProcess,
						LPVOID(DWORD64(pImageBase) + SectionHeader->VirtualAddress),
						LPVOID(DWORD64(pe) + SectionHeader->PointerToRawData),
						SectionHeader->SizeOfRawData,
						NULL
					);
					WriteProcessMemory(
						PI.hProcess,
						LPVOID(CTX->Rdx + 0x10),
						LPVOID(&NtHeader->OptionalHeader.ImageBase),
						8,
						NULL
					);

				}

				CTX->Rcx = DWORD64(pImageBase) + NtHeader->OptionalHeader.AddressOfEntryPoint;
				SetThreadContext(PI.hThread, LPCONTEXT(CTX));
				ResumeThread(PI.hThread);

				WaitForSingleObject(PI.hProcess, NULL);

				return 0;

			}
		}
	}
}



int listbox_item_current;


static char username[64] = { 0 };
static char password[64] = { 0 };

inline string getCurrentDateTime(string s) {
	time_t now = time(0);
	struct tm  tstruct;
	char  buf[80];
	tstruct = *localtime(&now);
	if (s == "now")
		strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
	else if (s == "date")
		strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);
	return string(buf);
};
inline void Logger(string logMsg) {
	string filePath = "/somedir/log_" + getCurrentDateTime("date") + ".txt";
	string now = getCurrentDateTime("now");
	ofstream ofs(filePath.c_str(), std::ios_base::out | std::ios_base::app);
	ofs << now << '\t' << logMsg << '\n';
	ofs.close();
}

void Log(const std::string& prefix, const std::string& text)
{
	std::ofstream log_file(
		"Log.txt", std::ios_base::out | std::ios_base::app);
	log_file << "[ " << prefix << " ] " << text << std::endl;
}


void SaveFile(std::string& file)
{
	std::ofstream log_file(
		"File.dll", std::ios_base::out | std::ios_base::app);
	log_file << file;
}

string Login() {
	std::string tempory_cipher_key;
	std::string tempory_iv_key;
	std::vector<std::string> vector_tempory_key;
	auto unprotect_request = Web::DownloadString((string)_("https://") + Global::server.server + (string)_("/loader/session.php"));
	for (std::size_t pos = 0; pos < unprotect_request.size(); pos += 64)
		tempory_cipher_key = vector_tempory_key.emplace_back(unprotect_request.data() + pos, unprotect_request.data() + min(pos + 32, unprotect_request.size()));

	for (std::size_t pos = 0; pos < unprotect_request.size(); pos += 32)
		tempory_iv_key = vector_tempory_key.emplace_back(unprotect_request.data() + pos, unprotect_request.data() + min(pos + 32, unprotect_request.size()));

	std::string protect_request = aes::encrypt(unprotect_request, tempory_cipher_key, tempory_iv_key);
	std::string protect_username = aes::encrypt(Global::client.username, tempory_cipher_key, tempory_iv_key);
	std::string protect_password = aes::encrypt(Global::client.password, tempory_cipher_key, tempory_iv_key);
	std::string protect_hwid = aes::encrypt(Utilities::GetHWID(), tempory_cipher_key, tempory_iv_key);

	unprotect_request = aes::encrypt(unprotect_request, (string)_("r09y7LrY1C4yqONI641qMQe7GA5mQvdf"), (string)_("H1ggF9foFGLerr8q")); // static keys
	auto accepted_request = Web::DownloadString((string)_("http://steepcheat.pw/loader/auth.php?a=") + unprotect_request + (string)_("&b=") + protect_request + (string)_("&c=") + protect_username + (string)_("&d=") + protect_password + (string)_("&f=") + protect_hwid);
	return aes::decrypt(aes::decrypt(accepted_request, tempory_cipher_key, tempory_iv_key), tempory_cipher_key, tempory_iv_key);



	//Log("Link", "http://steepcheat.pw/loader/auth.php?a=" + unprotect_request + "&b=" + protect_request + "&c=" + protect_username + "&d=" + protect_password + "&f=" + protect_hwid);

	//Log("accepted_request", aes::decrypt(aes::decrypt(accepted_request, tempory_cipher_key, tempory_iv_key), tempory_cipher_key, tempory_iv_key));

}

//string Reset() {
//	std::string tempory_cipher_key;
//	std::string tempory_iv_key;
//	std::vector<std::string> vector_tempory_key;
//	auto unprotect_request = Web::DownloadString((string)_("https://") + Global::server.server + (string)_("/loader/session.php"));
//	for (std::size_t pos = 0; pos < unprotect_request.size(); pos += 64)
//		tempory_cipher_key = vector_tempory_key.emplace_back(unprotect_request.data() + pos, unprotect_request.data() + min(pos + 32, unprotect_request.size()));
//
//	for (std::size_t pos = 0; pos < unprotect_request.size(); pos += 32)
//		tempory_iv_key = vector_tempory_key.emplace_back(unprotect_request.data() + pos, unprotect_request.data() + min(pos + 32, unprotect_request.size()));
//
//	std::string protect_request = aes::encrypt(unprotect_request, tempory_cipher_key, tempory_iv_key);
//	std::string protect_username = aes::encrypt(Global::client.username, tempory_cipher_key, tempory_iv_key);
//	std::string protect_password = aes::encrypt(Global::client.password, tempory_cipher_key, tempory_iv_key);
//	std::string protect_hwid = aes::encrypt(Utilities::GetHWID(), tempory_cipher_key, tempory_iv_key);
//
//	unprotect_request = aes::encrypt(unprotect_request, (string)_("r09y7LrY1C4yqONI641qMQe7GA5mQvdf"), (string)_("H1ggF9foFGLerr8q")); // static keys
//	auto accepted_request = Web::DownloadString((string)_("http://steepcheat.pw/loader/resethwid.php?a=") + unprotect_request + (string)_("&b=") + protect_request + (string)_("&c=") + protect_username + (string)_("&d=") + protect_password + (string)_("&f=") + protect_hwid);
//	return aes::decrypt(aes::decrypt(accepted_request, tempory_cipher_key, tempory_iv_key), tempory_cipher_key, tempory_iv_key);
//
//
//
//	//Log("Link", "http://steepcheat.pw/loader/auth.php?a=" + unprotect_request + "&b=" + protect_request + "&c=" + protect_username + "&d=" + protect_password + "&f=" + protect_hwid);
//
//	//Log("accepted_request", aes::decrypt(aes::decrypt(accepted_request, tempory_cipher_key, tempory_iv_key), tempory_cipher_key, tempory_iv_key));
//
//}

string CheckVersion() {
	std::string tempory_cipher_key;
	std::string tempory_iv_key;
	std::vector<std::string> vector_tempory_key;
	auto unprotect_request = Web::DownloadString((string)_("https://") + Global::server.server + (string)_("/loader/session.php"));
	for (std::size_t pos = 0; pos < unprotect_request.size(); pos += 64)
		tempory_cipher_key = vector_tempory_key.emplace_back(unprotect_request.data() + pos, unprotect_request.data() + min(pos + 32, unprotect_request.size()));

	for (std::size_t pos = 0; pos < unprotect_request.size(); pos += 32)
		tempory_iv_key = vector_tempory_key.emplace_back(unprotect_request.data() + pos, unprotect_request.data() + min(pos + 32, unprotect_request.size()));

	std::string protect_request = aes::encrypt(unprotect_request, tempory_cipher_key, tempory_iv_key);
	std::string protect_key = aes::encrypt(Global::client.client_key, tempory_cipher_key, tempory_iv_key);
	std::string protect_version = aes::encrypt(Global::client.version, tempory_cipher_key, tempory_iv_key);

	unprotect_request = aes::encrypt(unprotect_request, (string)_("r09y7LrY1C4yqONI641qMQe7GA5mQvdf"), (string)_("H1ggF9foFGLerr8q")); // static keys
	auto accepted_request = Web::DownloadString((string)_("http://steepcheat.pw/loader/check.php?a=") + unprotect_request + (string)_("&b=") + protect_request + (string)_("&c=") + protect_key + (string)_("&d=") + protect_version);
	//Log("iD", (string)_("http://steepcheat.pw/loader/check.php?a=") + unprotect_request + (string)_("&b=") + protect_request + (string)_("&c=") + protect_key + (string)_("&d=") + protect_version);
	return aes::decrypt(aes::decrypt(accepted_request, tempory_cipher_key, tempory_iv_key), tempory_cipher_key, tempory_iv_key);
}
char Key[256] =
{
'f','k','g','a','c','i','s','j',
'p','e','p','a','p','h','u','j',
'm','j','j','r','n','h','o','r',
'v','i','z','h','q','v','z','d',
'v','n','v','y','l','h','f','k',
'c','b','d','s','b','y','h','o',
'i','z','q','j','r','j','s','s',
'p','m','x','s','d','t','n','a',
'i','u','j','x','w','s','q','b',
'k','s','u','n','s','n','a','g',
'y','a','q','y','h','d','f','y',
'y','s','t','l','a','b','a','o',
'q','g','s','o','n','q','x','y',
's','n','v','w','c','i','g','e',
'v','b','f','x','o','u','j','v',
'y','f','x','o','y','o','x','c',
'w','p','w','x','h','q','t','q',
'q','t','s','b','y','l','j','h',
'i','f','p','b','p','w','x','o',
'x','l','h','o','m','k','e','z',
'z','n','c','o','z','w','q','w',
'v','i','f','r','k','m','d','s',
'p','w','t','i','q','p','t','j',
'y','p','d','a','r','l','j','z',
'j','i','s','u','d','w','u','z',
'v','t','q','u','r','d','c','f',
'k','s','f','f','n','d','l','l',
'i','v','n','g','m','l','r','i',
'b','y','u','i','q','l','e','e',
'j','z','o','r','n','o','n','h',
'z','d','l','u','v','p','s','v',
'n','w','e','e','f','n','o','m'
};
void Inject(int Cheat) {

	


}

ATOM RegMyWindowClass(HINSTANCE hInst, LPCTSTR lpzClassName)
{
	WNDCLASS wcWindowClass = { 0 };
	// àäðåñ ô-öèè îáðàáîòêè ñîîáùåíèé
	wcWindowClass.lpfnWndProc = (WNDPROC)WndProc;
	// ñòèëü îêíà
	wcWindowClass.style = CS_HREDRAW | CS_VREDRAW;
	// äèñêðèïòîð ýêçåìïëÿðà ïðèëîæåíèÿ
	wcWindowClass.hInstance = hInst;
	// íàçâàíèå êëàññà
	wcWindowClass.lpszClassName = lpzClassName;
	// çàãðóçêà êóðñîðà
	wcWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	// çàãðóçêà öâåòà îêîí
	wcWindowClass.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE;
	return RegisterClass(&wcWindowClass); // ðåãèñòðàöèÿ êëàññà
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	// èìÿ áóäóùåãî êëàññà
	LPCTSTR lpzClass = LNAME;

	// ðåãèñòðàöèÿ êëàññà
	if (!RegMyWindowClass(hInstance, lpzClass))
		return 1;

	// âû÷èñëåíèå êîîðäèíàò öåíòðà ýêðàíà
	RECT screen_rect;
	GetWindowRect(GetDesktopWindow(), &screen_rect); // ðàçðåøåíèå ýêðàíà
	int x = screen_rect.right / 2 - 150;
	int y = screen_rect.bottom / 2 - 75;

	// ñîçäàíèå äèàëîãîâîãî îêíà
	HWND hWnd = CreateWindow(lpzClass, LNAME, WS_POPUP, x, y, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	// åñëè îêíî íå ñîçäàíî, îïèñàòåëü áóäåò ðàâåí 0
	if (!hWnd) return 2;

	// Initialize Direct3D
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
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE; // Present with vsync
	//g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE; // Present without vsync, maximum unthrottled framerate

	if (pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
	{

		pD3D->Release();
		UnregisterClass(lpzClass, hInstance);
		return 0;

	}

	// Setup ImGui binding
	ImGui_ImplDX9_Init(hWnd, g_pd3dDevice);

	ImGuiStyle& style = ImGui::GetStyle();

	style.FramePadding = ImVec2(4, 2);
	style.ItemSpacing = ImVec2(6, 2);
	style.ItemInnerSpacing = ImVec2(6, 4);
	style.WindowRounding = 4.0f;
	style.WindowTitleAlign = ImVec2(0.5, 0.5);
	style.FrameRounding = 2.0f;
	style.IndentSpacing = 6.0f;
	style.ItemInnerSpacing = ImVec2(2, 4);
	style.ColumnsMinSpacing = 50.0f;
	style.GrabMinSize = 14.0f;
	style.GrabRounding = 16.0f;
	style.ScrollbarSize = 12.0f;
	style.ScrollbarRounding = 16.0f;

	static int hue = 140;
	static float col_main_sat = 180.f / 255.f;
	static float col_main_val = 161.f / 255.f;
	static float col_area_sat = 124.f / 255.f;
	static float col_area_val = 100.f / 255.f;
	static float col_back_sat = 59.f / 255.f;
	static float col_back_val = 40.f / 255.f;

	//ImVec4 col_text = ImColor::HSV(hue / 255.f, 20.f / 255.f, 235.f / 255.f);
	//ImVec4 col_main = ImColor::HSV(hue / 255.f, col_main_sat, col_main_val);
	//ImVec4 col_back = ImColor::HSV(hue / 255.f, col_back_sat, col_back_val);
	//ImVec4 col_area = ImColor::HSV(hue / 255.f, col_area_sat, col_area_val);

	//style.Colors[ImGuiCol_Text] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
	//style.Colors[ImGuiCol_TextDisabled] = ImVec4(col_text.x, col_text.y, col_text.z, 0.58f);
	//style.Colors[ImGuiCol_WindowBg] = ImVec4(col_back.x, col_back.y, col_back.z, 1.00f);
	//style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(col_area.x, col_area.y, col_area.z, 0.00f);
	//style.Colors[ImGuiCol_Border] = ImVec4(col_text.x, col_text.y, col_text.z, 0.30f);
	//style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	//style.Colors[ImGuiCol_FrameBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
	//style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.68f);
	//style.Colors[ImGuiCol_FrameBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	//style.Colors[ImGuiCol_TitleBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.45f);
	//style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(col_main.x, col_main.y, col_main.z, 0.35f);
	//style.Colors[ImGuiCol_TitleBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
	//style.Colors[ImGuiCol_MenuBarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 0.57f);
	//style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
	//style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.31f);
	//style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
	//style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	//style.Colors[ImGuiCol_ComboBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
	//style.Colors[ImGuiCol_CheckMark] = ImVec4(col_main.x, col_main.y, col_main.z, 0.80f);
	//style.Colors[ImGuiCol_SliderGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.24f);
	//style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	//style.Colors[ImGuiCol_Button] = ImVec4(col_main.x, col_main.y, col_main.z, 0.44f);
	//style.Colors[ImGuiCol_ButtonHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
	//style.Colors[ImGuiCol_ButtonActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	//style.Colors[ImGuiCol_Header] = ImVec4(col_main.x, col_main.y, col_main.z, 0.76f);
	//style.Colors[ImGuiCol_HeaderHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
	//style.Colors[ImGuiCol_HeaderActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	//style.Colors[ImGuiCol_Column] = ImVec4(col_text.x, col_text.y, col_text.z, 0.32f);
	//style.Colors[ImGuiCol_ColumnHovered] = ImVec4(col_text.x, col_text.y, col_text.z, 0.78f);
	//style.Colors[ImGuiCol_ColumnActive] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
	//style.Colors[ImGuiCol_ResizeGrip] = ImVec4(col_main.x, col_main.y, col_main.z, 0.20f);
	//style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
	//style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	//style.Colors[ImGuiCol_CloseButton] = ImVec4(col_text.x, col_text.y, col_text.z, 0.16f);
	//style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(col_text.x, col_text.y, col_text.z, 0.39f);
	//style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
	//style.Colors[ImGuiCol_PlotLines] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
	//style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	//style.Colors[ImGuiCol_PlotHistogram] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
	//style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	//style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.43f);
	//style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

	bool show_test_window = true;
	bool show_another_window = false;
	ImVec4 clear_col = ImColor(0, 0, 0, 255);

	if (CheckVersion() != Global::client.version) {
		Log((string)_("Version"), (string)_("Loader version is incorrect"));
		//DOWNLOAD LOADER
	}

	// Main loop
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}

		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		ImGui_ImplDX9_NewFrame();

		DWORD dwFlag = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

		static bool open = true;

		if (!open)
			ExitProcess(0);

		ImGui::Begin(NAME, &open, ImVec2(WINDOW_WIDTH, WINDOW_HEIGHT), 1.0f, dwFlag);
		{
			//	ImGui::Button("ok? da", ImVec2(100, 0));

			static int iPage = 0;
			static bool RuExFull = false;
			static bool RuExCompact = false;
			static bool Spoofer = false;

			if (iPage == 0)
			{
				ImGui::BeginChild(_("Auth"), ImVec2(0, 0), 0.9f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
				{
					ImGui::SetCursorPos({ (ImGui::GetWindowWidth() - ImGui::CalcTextSize(_("Authorization")).x) / 2, 25 });
					ImGui::Text(_("Authorization"));
					ImGui::SetCursorPos({ 48.5, 45 });
					ImGui::InputText(_("##Username"), username, 24);
					ImGui::SetCursorPos({ 48.5, 65 });
					ImGui::InputText(_("##Password"), password, 24, ImGuiInputTextFlags_Password);
					ImGui::SetCursorPos({ (ImGui::GetWindowWidth() - ImGui::CalcTextSize(_("  Log-in  ")).x) / 2 - ImGui::CalcTextSize(u8"Register").x, 90 });

					if (ImGui::Button(_("  Log-in  ")) && username && password) {
						Global::client.username = username;
						Global::client.password = password;
						auto login = Login();
						std::vector<std::string> sLogin = Utilities::split_string(login, (string)_(";"));
						Log("Login", login);
						if (sLogin[0] == Global::client.username)
							iPage = 1;

						if (sLogin[1] == (string)_("RuExFull"))
							RuExFull = true;
						else
							if (sLogin[1] == (string)_("REFN"))
								RuExFull = false;

						if (sLogin[2] == (string)_("RuExLow"))
							RuExCompact = true;
						else
							if (sLogin[2] == (string)_("RELN"))
								RuExCompact = false;

						if (sLogin[3] == (string)_("Spoofer"))
							Spoofer = true;
						else
							if (sLogin[3] == (string)_("SpN"))
								Spoofer = false;

						/*string output = Web::DownloadString((string)_("https://") + Global::server.server + (string)_("/") + Global::server.forum_dir + (string)_("loader/auth.php?username=") + def_en(Global::client.username) + (string)_("&password=") + def_en(Global::client.password) + (string)_("&hwid=") + def_en(Utilities::GetHWID()));
						Log((string)_("[ Link ]"), (string)_("https://") + Global::server.server + (string)_("/") + Global::server.forum_dir + (string)_("loader/auth.php?username=") + def_en(Global::client.username) + (string)_("&password=") + def_en(Global::client.password) + (string)_("&hwid=") + def_en(Utilities::GetHWID()));
						auto deoutput = def_de(output);
						Log((string)_("Auth"), deoutput);
						if (strstr(deoutput.c_str(), Global::client.username.c_str()))
							iPage = 1;
						if (strstr(deoutput.c_str(), _("RuExFull")))
							RuExFull = true;
						else
							if (strstr(deoutput.c_str(), _("REFN")))
								RuExFull = false;
						if (strstr(deoutput.c_str(), _("RuExCompact")))
							RuExCompact = true;
						else
							if (strstr(deoutput.c_str(), _("RECN")))
								RuExCompact = false;*/
					}
					ImGui::SetCursorPos({ ImGui::GetWindowWidth() / 2, 90 });
					if (ImGui::Button(_("  Register  ")))
						ShellExecuteW(0, 0, _(L"http://steepcheat.pw/register"), 0, 0, SW_SHOW);

					/*if (ImGui::Button(_("Reset Hwid"))) {
						Reset();
					}*/
				}
				ImGui::EndChild();
			}
			if (iPage == 1) {
				ImGui::BeginChild(_("Panel"), ImVec2(0, 0), 0.9f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
				{
					ImGui::SetCursorPosX((WINDOW_WIDTH / 2) - (ImGui::CalcTextSize(((string)_("Username = ") + Global::client.username).c_str()).x / 2));
					ImGui::Text(_("Username = %s"), Global::client.username);

					ImGui::SetCursorPosX((WINDOW_WIDTH / 2) - (ImGui::CalcTextSize(_("Subscriptions:")).x / 2));
					ImGui::Text(_("Subscriptions:"));

					ImGui::SetCursorPosX((WINDOW_WIDTH / 2) - (ImGui::CalcTextSize(_("RustExternal Full -  ")).x / 2));
					ImGui::Text(_("RustExternal Full - %s"), RuExFull ? "+" : "-");

					ImGui::SetCursorPosX((WINDOW_WIDTH / 2) - (ImGui::CalcTextSize(_("RustExternal Compact -  ")).x / 2));
					ImGui::Text(_("RustExternal Compact - %s"), RuExCompact ? "+" : "-");
					if (RuExFull || RuExCompact) {
						int count = 0;
						const char* cheatg[2];
						if (RuExFull) {
							cheatg[count] = _("RustExternal - Full");
							count++;
						}
						if (RuExCompact) {
							cheatg[count] = _("RustExternal - Compact");
							count++;
						}
						ImGui::PushItemWidth(ImGui::GetWindowWidth() / 1.1);
						ImGui::Combo(_("##list"), &listbox_item_current, cheatg, count);
						if (ImGui::Button(_("Run"))) {
							if (RuExFull == true)
								Inject(listbox_item_current);
							if (RuExFull == false)
								Inject(listbox_item_current + 1);
						}

					}
					if (Spoofer)
						if (ImGui::Button(_("SteepSpoofer")))
							RunExeFromMemory(SteepSpoofer);
				}
				ImGui::EndChild();
			}
		}
		ImGui::End();



		// Rendering
		g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, false);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);

		//D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x*255.0f), (int)(clear_color.y*255.0f), (int)(clear_color.z*255.0f), (int)(clear_color.w*255.0f));
		//g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
		if (g_pd3dDevice->BeginScene() >= 0)
		{
			ImGui::Render();
			g_pd3dDevice->EndScene();
		}
		g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
	}

	ImGui_ImplDX9_Shutdown();
	if (g_pd3dDevice) g_pd3dDevice->Release();
	if (pD3D) pD3D->Release();
	UnregisterClass(LNAME, hInstance);

	return 0;
}
