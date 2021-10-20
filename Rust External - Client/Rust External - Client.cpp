#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>  
#include <Process/Process.h>
#include <Windows.h>
#include <string>
#include <cassert>
#include <emmintrin.h>
#include <tlhelp32.h>
#include <winternl.h>
typedef struct _UncStr
{
	char stub[0x10];
	int len;
	wchar_t str;
} *pUncStr;

using namespace std;

uint64_t gBase, uBase;
int pid;
SOCKET Sock;
long long firstentry = 0;
UINT64 TodCycle = 0;

#pragma once
#include <Windows.h>

constexpr DWORD init_code = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x775, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
constexpr DWORD read_code = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x776, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
constexpr DWORD write_code = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x777, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);

class driver_manager {
	HANDLE m_driver_handle = nullptr;

	struct info_t {
		UINT64 target_pid = 0;
		UINT64 target_address = 0x0;
		UINT64 buffer_address = 0x0;
		UINT64 size = 0;
		UINT64 return_size = 0;
		UINT64 GA = 0;
		UINT64 UP = 0;
	};

public:
	driver_manager(const char* driver_name, DWORD target_process_id) {
		m_driver_handle = CreateFileA(driver_name, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		attach_to_process(target_process_id);
	}

	void attach_to_process(DWORD process_id) {
		info_t io_info;

		io_info.target_pid = process_id;

		DeviceIoControl(m_driver_handle, init_code, &io_info, sizeof(io_info), &io_info, sizeof(io_info), nullptr, nullptr);
		gBase = io_info.GA;
		uBase = io_info.UP;
		Sleep(10000);
	}

	template<typename T> T RPM(const UINT64 address) {
		info_t io_info;
		T read_data;

		io_info.target_address = address;
		io_info.buffer_address = (UINT64)&read_data;
		io_info.size = sizeof(T);

		DeviceIoControl(m_driver_handle, read_code, &io_info, sizeof(io_info), &io_info, sizeof(io_info), nullptr, nullptr);
		if (!io_info.buffer_address)
			std::cout << "read_data is null" << std::endl;
		return read_data;
	}

	template<typename T> bool WPM(const UINT64 address, const T buffer) {
		info_t io_info;

		io_info.target_address = address;
		io_info.buffer_address = (UINT64)&buffer;
		io_info.size = sizeof(T);

		DeviceIoControl(m_driver_handle, write_code, &io_info, sizeof(io_info), &io_info, sizeof(io_info), nullptr, nullptr);
		return io_info.return_size == sizeof(T);
	}
};

driver_manager* gDriver;

#define safe_read(Addr, Type) gDriver->RPM<Type>(Addr)
#define safe_write(Addr, Data, Type) gDriver->WPM<Type>(Addr, Data)
#define safe_memcpy(Dst, Src, Size) driver::write_memory(Sock, pid, Dst, driver::read_memory(Sock, pid, Src, 0, Size), Size)

int length(uintptr_t addr) { return safe_read(addr + 0x10, int); }

std::string readstring(uintptr_t addr) {
	try {
		static char buff[128] = { 0 };
		buff[length(addr)] = '\0';

		for (int i = 0; i < length(addr); ++i) {
			if (buff[i] < 128) {
				buff[i] = safe_read(addr + 0x14 + (i * 2), UCHAR);
			}
			else {
				buff[i] = '?';
				if (buff[i] >= 0xD800 && buff[i] <= 0xD8FF)
					i++;
			}
		}
		return std::string(&buff[0], &buff[length(addr)]);
	}
	catch (const std::exception& exc) {}
	return "Error";
}
struct monostr
{
	char buffer[128];
};

std::string readchar(uintptr_t addr) {
	std::string str = safe_read(addr, monostr).buffer;
	return str;
}

#include "Of.h"

#include "Main.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_impl_win32.h"
#include "Overlay.h"

LPCSTR TargetTitle = "Rust";
LPCSTR OverName = "Rust External - Client";
bool CreateConsole = false;

#include "Value.h"
#include "SDK/Math.h"
#include "SDK/BaseProjectile.h"
#include "SDK/BasePlayer.h"
#include "SDK/Misc.h"
#include "SDK/Aim.h"


BasePlayer* AimEntity = nullptr;



void WeaponFix(BaseProjectile* weapon)
{
	int ItemID = 0;
	if (weapon)
		ItemID = weapon->GetItemID();
	if (!ItemID) return;

	for (DWORD64 val : semiautomatic) {
		if (ItemID == val) {
			weapon->NoRecoil();
			weapon->NoSpread();
			weapon->SetAutomatic();
			return;
		}
	}
	for (DWORD64 val : sniper) {
		if (ItemID == val) {
			weapon->NoRecoil();
			weapon->NoSpread();
			return;
		}
	}
	for (DWORD64 val : automatic) {
		if (ItemID == val) {
			weapon->NoRecoil();
			weapon->NoSpread();
			return;
		}
	}
}

void InitLocalPlayer() {
	long long i = 0;
	UINT64  ObjMgr = safe_read(uBase + oGameObjectManager, UINT64);
	UINT64  Obj = safe_read(ObjMgr + 0x8, UINT64);

	bool LP_isValid = false;

	for (UINT64 Obj = safe_read(ObjMgr + 0x8, UINT64); (Obj && (Obj != safe_read(ObjMgr, UINT64))); Obj = safe_read(Obj + 0x8, UINT64))
	{
		//std::cout << "Ent" << endl;
		UINT64 GameObject = safe_read(Obj + 0x10, UINT64);
		WORD Tag = safe_read(GameObject + 0x54, WORD);
		//std::cout << Tag << '\n';
		if (Tag == 6 || Tag == 5 || Tag == 20011)
		{
			UINT64 ObjClass = safe_read(GameObject + 0x30, UINT64);
			UINT64 Entity = safe_read(ObjClass + 0x18, UINT64);

			if (Tag == 6)
			{
				BasePlayer* Player = (BasePlayer*)safe_read(Entity + 0x28, UINT64);
				//if (!Player->IsValid())continue;
				//std::cout << "BASEPLAYER FOUND" << endl;
				//std::cout << "ID: " << Player->GetSteamID() << endl;

				if (safe_read(safe_read(GameObject + 0x60, UINT64), UINT64) == 0x616C506C61636F4C)
				{

					LP_isValid = true;
					LocalPlayer.BasePlayer = Player;
					firstentry = Obj;
					AimEntity = Player;
					if (TodCycle && LocalPlayer.pViewMatrix)
						return;
				}
			}
			else if (Tag == 5)
			{
				UINT64 ObjClass = safe_read(GameObject + 0x30, UINT64);
				UINT64	Entity = safe_read(ObjClass + 0x18, UINT64);
				LocalPlayer.pViewMatrix = (Matrix4x4*)(Entity + 0x2E4);
			}
			else if (Tag == 20011)
			{
				UINT64 ObjClass = safe_read(GameObject + 0x30, UINT64);
				UINT64	Entity = safe_read(ObjClass + 0x18, UINT64);
				UINT64 Dome = safe_read(Entity + 0x28, UINT64);
				TodCycle = safe_read(Dome + 0x38, UINT64);
			}
		}

	NextEnt:
		continue;
	}

	if (!TodCycle || !LP_isValid || LocalPlayer.BasePlayer->IsMenu()) {
		return;
	}
}


void InputHandler() {
	for (int i = 0; i < 5; i++) ImGui::GetIO().MouseDown[i] = false;
	int button = -1;
	if (GetAsyncKeyState(VK_LBUTTON)) button = 0;
	if (button != -1) ImGui::GetIO().MouseDown[button] = true;
}
ImVec2 Pos;
void Draw() {
	char fpsinfo[64];
	sprintf(fpsinfo, "Rust External - Client");
	RGBA White = { 255,0,255,255 };
	DrawStrokeText( 15, Value::floats::Screen::H - 25 , &White, fpsinfo);
	RGBA Color = { Value::Colors::Aim::Fov[0] * 255, Value::Colors::Aim::Fov[1] * 255 , Value::Colors::Aim::Fov[2] * 255, Value::Colors::Aim::Fov[3] * 255 };
	if(Value::bools::Aim::Fov)
		DrawCircle(Value::floats::Screen::W / 2, Value::floats::Screen::H / 2, Value::floats::Aim::Fov, &Color, 50);
}
#include <comdef.h>
Vector2 Penis;
void ESP(BasePlayer* BP, BasePlayer* LP) {
	if (Value::bools::Visuals::ESP::Enable) {
		Vector2 tempFeetR, tempFeetL;
		if (LocalPlayer.WorldToScreen(BP->GetBoneByID2(r_foot), tempFeetR) && LocalPlayer.WorldToScreen(BP->GetBoneByID2(penis), Penis) && LocalPlayer.WorldToScreen(BP->GetBoneByID2(l_foot), tempFeetL)) {
			Vector2 tempHead;
			if (LocalPlayer.WorldToScreen(BP->GetBoneByID2(jaw) + Vector3(0.f, 0.16f, 0.f), tempHead))
			{
				Vector2 tempFeet = (tempFeetR + tempFeetL) / 2.f;
				float Entity_h = tempHead.y - tempFeet.y;
				float w = Entity_h / 4;
				float Entity_x = tempFeet.x - w;
				float Entity_y = tempFeet.y;
				float Entity_w = Entity_h / 2;

				bool PlayerWounded = BP->HasFlags(64);
				bool PlayerSleeping = BP->HasFlags(16);

				if (PlayerSleeping && Value::bools::Visuals::ESP::IgnoreSleeper)
					return;

				RGBA White = { 255, 0,255,255 };
				RGBA Bot = { 0, 0,0,255 };
				//D2D1::ColorF::Enum PlayerClr = PlayerSleeping ? D2D1::ColorF::BlueViolet : PlayerWounded ? D2D1::ColorF::DarkOrange : D2D1::ColorF::Gold;
				//if (Value::bools::Visuals::ESP::Player)	DrawLine(Vars::Other::Width / 2, Vars::Other::Height, Penis.x, Penis.y, 255, 0, 255, 255);
				if (Value::bools::Visuals::ESP::Health) {
					RGBA Color = { Value::Colors::Visuals::ESP::Health[0] * 255, Value::Colors::Visuals::ESP::Health[1] * 255 , Value::Colors::Visuals::ESP::Health[2] * 255, Value::Colors::Visuals::ESP::Health[3] * 255 };
					char Health[64];
					sprintf(Health, "%0.f HP", BP->GetHealth());
					DrawStrokeText(Penis.x + 10, Penis.y + 30, &Color, Health);
				}
				if (Value::bools::Visuals::ESP::Box && !PlayerSleeping && !PlayerWounded && !(BP->GetSteamID() < 1000000000)) {
					RGBA Color = { Value::Colors::Visuals::ESP::Box[0] * 255, Value::Colors::Visuals::ESP::Box[1] * 255 , Value::Colors::Visuals::ESP::Box[2] * 255, Value::Colors::Visuals::ESP::Box[3] * 255 };
					DrawRect(Entity_x, Entity_y, Entity_w, Entity_h, &Color, 2);
				}
				if (Value::bools::Visuals::ESP::Box && !PlayerSleeping && !PlayerWounded && (BP->GetSteamID() < 1000000000)) {
					RGBA Color = { Value::Colors::Visuals::ESP::Box[0] * 255, Value::Colors::Visuals::ESP::Box[1] * 255 , Value::Colors::Visuals::ESP::Box[2] * 255, Value::Colors::Visuals::ESP::Box[3] * 255 };
					DrawRect(Entity_x, Entity_y, Entity_w, Entity_h, &Color, 2);
				}
				if (Value::bools::Visuals::ESP::SteamID) {
					RGBA Color = { Value::Colors::Visuals::ESP::SteamID[0] * 255, Value::Colors::Visuals::ESP::SteamID[1] * 255 , Value::Colors::Visuals::ESP::SteamID[2] * 255, Value::Colors::Visuals::ESP::SteamID[3] * 255 };
					char SteamID[64];
					sprintf(SteamID, "ID : %i", BP->GetSteamID());
					DrawStrokeText(Penis.x + 10, Penis.y + 30, &Color, SteamID);
				}
				if (Value::bools::Visuals::ESP::Name) {
					RGBA Color = { Value::Colors::Visuals::ESP::Name[0] * 255, Value::Colors::Visuals::ESP::Name[1] * 255 , Value::Colors::Visuals::ESP::Name[2] * 255, Value::Colors::Visuals::ESP::Name[3] * 255 };
					DrawStrokeText(Penis.x + 10, Penis.y, &Color, BP->GetNamecChars().c_str());
				}
				if (Value::bools::Visuals::ESP::Weapon) {
					RGBA Color = { Value::Colors::Visuals::ESP::Weapon[0] * 255, Value::Colors::Visuals::ESP::Weapon[1] * 255 , Value::Colors::Visuals::ESP::Weapon[2] * 255, Value::Colors::Visuals::ESP::Weapon[3] * 255 };
					DrawStrokeText(Penis.x + 10, Penis.y + 15, &Color, BP->GetActiveWeaponcChars().c_str());
				}
				if (Value::bools::Visuals::ESP::Distance) {
					RGBA Color = { Value::Colors::Visuals::ESP::Distance[0] * 255, Value::Colors::Visuals::ESP::Distance[1] * 255 , Value::Colors::Visuals::ESP::Distance[2] * 255, Value::Colors::Visuals::ESP::Distance[3] * 255 };
					char Distance[64];
					sprintf(Distance, "%d M", (int)Math::Calc3D_Dist(LP->GetBoneByID2(head), BP->GetBoneByID2(head)));
					DrawStrokeText(Penis.x + 10, Penis.y + 45, &Color, Distance);
				}
			}
		}
	}
}

void Entity() {
	DWORD64 Id = LocalPlayer.BasePlayer->GetActiveWeapon()->GetItemID();
	if (Id == 1525520776 || Id == 200773292) return; //building plan, hammer(киянка) fix крашей от киянки

	static UINT64 oPlayerList = 0;
	if (!oPlayerList) {
		UINT64 val = safe_read(gBase + 50748008, UINT64); // 293BF10 0x2940680 BasePlayer_c* //50748072
		UINT64 st = safe_read(val + 0xB8, UINT64);
		UINT64 list = safe_read(st + 0x8, UINT64);
		oPlayerList = safe_read(list + 0x28, UINT64);
	}

	int EntityCount = safe_read(oPlayerList + 0x10, int);
	UINT64 EntityBuffer = safe_read(oPlayerList + 0x18, UINT64);
	for (int i = 0; i < EntityCount; i++)
	{
		auto Player = (BasePlayer*)safe_read(EntityBuffer + 0x20 + (i * 0x8), UINT64);
		if (!Player || Player == LocalPlayer.BasePlayer || !Player->IsValid())
			continue; //4 чтения
		ESP(Player, LocalPlayer.BasePlayer);
		if (Value::bools::Aim::VisibleCheck) {
			if (!Player->IsVisible())
				continue;
		} //2 чтения
		if (Value::bools::Aim::IgnoreSleepers && Player->HasFlags(16))
			continue;
		if (!Player->IsValid())
			continue;
		if (AimFov(Player) > Value::floats::Aim::Fov) continue;
		if (Value::bools::Aim::IgnoreTeam) {
			UINT64 EntitySID = Player->GetSteamID();
			int size = LocalPlayer.BasePlayer->GetTeamSize(); //3
			for (int j = 0; j < size; j++) {
				UINT64 SlotSID = LocalPlayer.BasePlayer->IsTeamMate(j);       //5
				if (SlotSID == EntitySID)
					continue;
			}
		}
		AimEntity = Player;
		Aim(AimEntity);
	}
	return;
}
#include "ImGui/imgui_internal.h"
float W = 200.f, H = 140;;
void PlayerInfoPlayer(BasePlayer* BP) {
	if (Value::bools::Visuals::PlayerPanel::Enable) {
		float Pos = 0;
		ImGui::SetNextWindowSize({ W, H });
		ImGui::Begin("PlayerInfo", NULL, ImGuiWindowFlags_NoDecoration);
		auto pos = ImGui::GetWindowPos();
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		window->DrawList->AddRectFilled({ pos.x - 10, pos.y - 10 }, { pos.x + W, pos.y + H }, ImGui::ColorConvertFloat4ToU32({ 0.25 , 0.25 , 0.25 , 1 }), 2);
		RGBA col = { 192, 192, 192, 255 };
		if (Value::bools::Visuals::PlayerPanel::Name) {
			auto nickname = BP->GetNamecChars();
			window->DrawList->AddText({ pos.x + (W / 2) - (ImGui::CalcTextSize(nickname.c_str()).x / 2), pos.y + 5 }, ImGui::ColorConvertFloat4ToU32({ 0.75 , 0.75 , 0.75 , 1 }), nickname.c_str());
		}
		window->DrawList->AddLine({ pos.x, pos.y + 23 }, { pos.x + W,  pos.y + 23 }, ImGui::ColorConvertFloat4ToU32({ 0.75 , 0.75 , 0.75 , 1 }), 1);
		if (Value::bools::Visuals::PlayerPanel::Weapons)
			for (int i = 0; i < 6; i++)
			{
				BaseProjectile* GetWeaponInfo = BP->GetWeaponInfo(i);
				if (GetWeaponInfo)
				{
					auto Item = BP->GetWeaponInfo(i)->GetName();
					auto name = readstring(Item);
					if (name.length() < 20)
					{
						auto text = name.c_str();
						ImVec2 textsize = ImGui::CalcTextSize(text);
						window->DrawList->AddText({ pos.x + (W / 2) - textsize.x / 2, pos.y + 25 + Pos }, ImGui::ColorConvertFloat4ToU32({ 0.75 , 0.75 , 0.75 , 1 }), text);
						//DrawStrokeText( pos.x + (W / 2) - textsize.x / 2, pos.y + 40 + Pos , &col , text);
					}
				}
				else
				{
					window->DrawList->AddText({ pos.x + (W / 2) - ImGui::CalcTextSize("Noting").x / 2, pos.y + 20 + Pos }, ImGui::ColorConvertFloat4ToU32({ 0.75 , 0.75 , 0.75 , 1 }), "Nothing");
					//DrawStrokeText( pos.x + (W / 2) - ImGui::CalcTextSize("Noting").x / 2, pos.y + 40 + Pos , &col, "Nothing");
				}
				Pos += 15;
			}
		if (Value::bools::Visuals::PlayerPanel::HP) {
			float health = BP->GetHealth();
			float maxheal = 100.f;
			window->DrawList->AddRect({ pos.x + (W / 2) - 50, pos.y + 27 + Pos }, { pos.x + (W / 2) - 50 + 100, pos.y + 27 + Pos + 15 }, ImGui::ColorConvertFloat4ToU32({ 0.75 , 0.75 , 0.75 , 1 }));
			window->DrawList->AddRectFilled({ pos.x + (W / 2) + -49, pos.y + 28 + Pos }, { pos.x + (W / 2) + -49 + (98 * (health / maxheal)), pos.y + 28 + Pos + 13 }, ImGui::ColorConvertFloat4ToU32({ 0.75 , 0.75 , 0.75 , 1 }), 2);
		}
		ImGui::End();
	}
}

void Render() {
	if (GetAsyncKeyState(VK_INSERT) & 1) ShowMenu = !ShowMenu;
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.25, 0.25, 0.25, 1);
	Draw();
	if (LocalPlayer.BasePlayer) {
		int WeaponId = 0;
		int last = -1;
		Misc(TodCycle);
		Entity();
		if (!(AimEntity->IsDead()) && Value::bools::Aim::TargetLine) {
			Vector2 ScreenPos;
			RGBA Color = { Value::Colors::Aim::TargetLine[0] * 255, Value::Colors::Aim::TargetLine[1] * 255 , Value::Colors::Aim::TargetLine[2] * 255, Value::Colors::Aim::TargetLine[3] * 255 };
			if (LocalPlayer.WorldToScreen(AimEntity->GetBoneByID2(spine1), ScreenPos))
				DrawLine(Value::floats::Screen::W / 2, Value::floats::Screen::H, ScreenPos.x, ScreenPos.y, &Color, 2);
		}

		if (!(AimEntity->IsDead()) && Value::bools::Visuals::PlayerPanel::Enable) {
			PlayerInfoPlayer(AimEntity);
		}

		BaseProjectile* active = LocalPlayer.BasePlayer->GetActiveWeapon();

		if (active->GetUID() != 0)
		{
			if (WeaponId != active->GetItemID())
			{
				WeaponId = active->GetItemID();
			}
			last = active->GetUID();
			if (last)
			{
				WeaponFix(active);
			}
		}
		else if (!last)
			last = active->GetUID();
	}
	else
		if (!LocalPlayer.BasePlayer)
			InitLocalPlayer();
	if (ShowMenu == true) {
		InputHandler();
		ImGui::Begin("Rust External - Client");
		{
			ImGui::SetColorEditOptions(ImGuiColorEditFlags_Float | ImGuiColorEditFlags_PickerHueWheel);
			ImGui::Text("PID : %d", pid);
			ImGui::Text("-----------------ESP--------------------");
			ImGui::Checkbox("Enable##1", &Value::bools::Visuals::ESP::Enable);
			if (Value::bools::Visuals::ESP::Enable) {
				ImGui::Checkbox("Name", &Value::bools::Visuals::ESP::Name);
				ImGui::SameLine();
				ImGui::ColorEdit4("##Name", Value::Colors::Visuals::ESP::Name);
				ImGui::Checkbox("Health", &Value::bools::Visuals::ESP::Health);
				ImGui::SameLine();
				ImGui::ColorEdit4("##Health", Value::Colors::Visuals::ESP::Health);
				ImGui::Checkbox("Box", &Value::bools::Visuals::ESP::Box);
				ImGui::SameLine();
				ImGui::ColorEdit4("##Box", Value::Colors::Visuals::ESP::Box);
				ImGui::Checkbox("Weapon", &Value::bools::Visuals::ESP::Weapon);
				ImGui::SameLine();
				ImGui::ColorEdit4("##Weapon", Value::Colors::Visuals::ESP::Weapon);
				//ImGui::Checkbox("SteamID", &Value::bools::Visuals::ESP::SteamID);
				//ImGui::SameLine();
				//ImGui::ColorEdit4("##SteamID", Value::Colors::Visuals::ESP::SteamID);
				ImGui::Checkbox("Distance", &Value::bools::Visuals::ESP::Distance);
				ImGui::SameLine();
				ImGui::ColorEdit4("##Distance", Value::Colors::Visuals::ESP::Distance);
				ImGui::Checkbox("Ignore Sleepers", &Value::bools::Visuals::ESP::IgnoreSleeper);
			}

			ImGui::Text("-----------------Player-info-panel------");
			ImGui::Checkbox("Enable", &Value::bools::Visuals::PlayerPanel::Enable);
			if (Value::bools::Visuals::PlayerPanel::Enable) {
				ImGui::Checkbox("Name", &Value::bools::Visuals::PlayerPanel::Name);
				ImGui::Checkbox("HP", &Value::bools::Visuals::PlayerPanel::HP);
				ImGui::Checkbox("Weapon", &Value::bools::Visuals::PlayerPanel::Weapons);
			}

			ImGui::Text("-----------------AIM--------------------");
			ImGui::Checkbox("Enable##2", &Value::bools::Aim::Enable);
			if (Value::bools::Aim::Enable) {
				ImGui::Checkbox("Target Line", &Value::bools::Aim::TargetLine);
				ImGui::SameLine();
				ImGui::ColorEdit4("##TargetLine", Value::Colors::Aim::TargetLine);
				ImGui::Checkbox("Visible Check", &Value::bools::Aim::VisibleCheck);
				ImGui::Checkbox("Ignore sleepers", &Value::bools::Aim::IgnoreSleepers);
				//ImGui::Checkbox("Ignore Team", &Value::bools::Aim::IgnoreTeam);
				ImGui::Checkbox("Fov", &Value::bools::Aim::Fov);
				ImGui::SameLine();
				ImGui::ColorEdit4("##Fov", Value::Colors::Aim::Fov);
				if (Value::bools::Aim::Fov)
					ImGui::SliderInt("Fov", &Value::floats::Aim::Fov, 0, 120);
				ImGui::Checkbox("Smooth", &Value::bools::Aim::Smooth);
				if (Value::bools::Aim::Smooth)
					ImGui::SliderFloat("Smooth Speed", &Value::floats::Aim::Smooth, 1, 25);
			}

			ImGui::Text("-----------------Player-----------------");
			ImGui::Checkbox("Spiderman", &Value::bools::Player::PlayerWalk::Spiderman);
			ImGui::Checkbox("Fake Admin", &Value::bools::Player::PlayerWalk::FakeAdmin);
			ImGui::Checkbox("Change Gravity", &Value::bools::Player::PlayerWalk::ChangeGravity);
			if (Value::bools::Player::PlayerWalk::ChangeGravity)
				ImGui::SliderFloat("Gravity", &Value::floats::Player::Gravity, 0, 2);
			ImGui::Checkbox("Infinity Jump", &Value::bools::Player::PlayerWalk::InfinityJump);

			ImGui::Text("-----------------Visual-----------------");
			ImGui::Checkbox("Always Day", &Value::bools::Visuals::World::AlwaysDay);
			if (Value::bools::Visuals::World::AlwaysDay)
				ImGui::SliderInt("Time", &Value::floats::Visuals::World::Time, 0, 24);
			ImGui::Checkbox("Custom Fov", &Value::bools::Player::CustomFov);
			if (Value::bools::Player::CustomFov)
				ImGui::SliderFloat("Custom Fov##1", &Value::floats::Player::CustomFov, 0, 120);

			ImGui::Text("-----------------Weapon-----------------");
			ImGui::Checkbox("No Recoil", &Value::bools::Weapon::NoRecoil);
			ImGui::Checkbox("No Spread", &Value::bools::Weapon::NoSpread);
			ImGui::Checkbox("IsAutimatic", &Value::bools::Weapon::IsAutimatic);
				

			Pos = ImGui::GetWindowPos();
		}
		ImGui::End();
		ImGui::GetIO().MouseDrawCursor = 1;
		SetWindowLong(Wnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOOLWINDOW | WM_SETFOCUS);
	}
	else {
		ImGui::GetIO().MouseDrawCursor = 0;
		SetWindowLong(Wnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
	}
	ImGui::EndFrame();
	pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	if (pDevice->BeginScene() >= 0) {
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		pDevice->EndScene();
	}
	HRESULT result = pDevice->Present(NULL, NULL, NULL, NULL);
	if (result == D3DERR_DEVICELOST && pDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
		ImGui_ImplDX9_InvalidateDeviceObjects();
		pDevice->Reset(&pParams);
		ImGui_ImplDX9_CreateDeviceObjects();
	}
}

DWORD WINAPI LoopThread(LPVOID lpParameter) {
	while (true) {
		static RECT old_rc;
		ZeroMemory(&Message, sizeof(MSG));
		while (Message.message != WM_QUIT) {
			RECT rc; POINT xy;
			ZeroMemory(&rc, sizeof(RECT));
			ZeroMemory(&xy, sizeof(POINT));
			GetClientRect(GameWnd, &rc);
			ClientToScreen(GameWnd, &xy);
			rc.left = xy.x; rc.top = xy.y;
			ImGuiIO& io = ImGui::GetIO();
			io.ImeWindowHandle = GameWnd;
			POINT p; GetCursorPos(&p);
			io.MousePos.x = p.x - xy.x;
			io.MousePos.y = p.y - xy.y;
			if (GetAsyncKeyState(0x1)) {
				io.MouseDown[0] = true;
				io.MouseClicked[0] = true;
				io.MouseClickedPos[0].x = io.MousePos.x;
				io.MouseClickedPos[0].x = io.MousePos.y;
			}
			else {
				io.MouseDown[0] = false;
			}
			if (rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom) {
				old_rc = rc;
				Width = rc.right;
				Height = rc.bottom;
				pParams.BackBufferWidth = Width;
				pParams.BackBufferHeight = Height;
				SetWindowPos(Wnd, (HWND)0, xy.x, xy.y, Width, Height, SWP_NOREDRAW);
				pDevice->Reset(&pParams);
			}
		}
		Sleep(100);
	}
	return 0;
}

WPARAM MainLoop() {
	while (Message.message != WM_QUIT) {
		if (PeekMessage(&Message, Wnd, 0, 0, PM_REMOVE)) {
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
		HWND hwnd_active = GetForegroundWindow();
		if (hwnd_active == GameWnd) {
			HWND hwndtest = GetWindow(hwnd_active, GW_HWNDPREV);
			SetWindowPos(Wnd, hwndtest, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		Render();
	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	ClearD3D();
	DestroyWindow(Wnd);
	return Message.wParam;
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {
		return true;
	}
	switch (msg)
	{
	case WM_DESTROY:
		ClearD3D();
		PostQuitMessage(0);
		exit(4);
		break;
	case WM_SIZE:
		if (pDevice != NULL && wParam != SIZE_MINIMIZED) {
			ImGui_ImplDX9_InvalidateDeviceObjects();
			pParams.BackBufferWidth = LOWORD(lParam);
			pParams.BackBufferHeight = HIWORD(lParam);
			HRESULT hr = pDevice->Reset(&pParams);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
		break;
	}
	return 0;
}

void SetupWindow() {
	WNDCLASSEX wClass{
		sizeof(WNDCLASSEX), 0, WndProc, 0, 0, nullptr, LoadIcon(nullptr, IDI_APPLICATION), LoadCursor(nullptr, IDC_ARROW), nullptr, nullptr, OverName, LoadIcon(nullptr, IDI_APPLICATION)
	};
	if (!RegisterClassEx(&wClass)) {
		exit(1);
	}
	GameWnd = FindWindowA(NULL, TargetTitle);
	if (GameWnd) {
		GetClientRect(GameWnd, &GameRect);
		POINT xy;
		ClientToScreen(GameWnd, &xy);
		GameRect.left = xy.x;
		GameRect.top = xy.y;
		Width = GameRect.right;
		Height = GameRect.bottom;
	}
	else {
		exit(2);
	}
	Wnd = CreateWindowExA(NULL, OverName, OverName, WS_POPUP | WS_VISIBLE, GameRect.left, GameRect.top, Width, Height, NULL, NULL, 0, NULL);
	DwmExtendFrameIntoClientArea(Wnd, &Margin);
	ShowWindow(Wnd, SW_SHOW);
	UpdateWindow(Wnd);
}

HRESULT DirectXInit(HWND hWnd) {
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &pObject))) {
		exit(3);
	}
	D3DPRESENT_PARAMETERS pParams = { 0 };
	pParams.Windowed = TRUE;
	pParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	pParams.hDeviceWindow = hWnd;
	pParams.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	pParams.BackBufferFormat = D3DFMT_A8R8G8B8;
	pParams.BackBufferWidth = Width;
	pParams.BackBufferHeight = Height;
	pParams.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	pParams.EnableAutoDepthStencil = TRUE;
	pParams.AutoDepthStencilFormat = D3DFMT_D16;
	pParams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	if (FAILED(pObject->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &pParams, 0, &pDevice))) {
		pObject->Release();
		exit(4);
	}
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantTextInput || ImGui::GetIO().WantCaptureKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui_ImplWin32_Init(Wnd);
	ImGui_ImplDX9_Init(pDevice);
	pObject->Release();
	return S_OK;
}

DWORD WINAPI ProcessCheck(LPVOID lpParameter) {
	while (true) {
		if (GameWnd != NULL) {
			if (!FindWindowA(NULL, TargetTitle)) {
				exit(2);
			}
		}
		Sleep(5000);
		InitLocalPlayer();
	}
}


#include <TlHelp32.h>

int main() {
	if (CreateConsole == false) { ShowWindow(::GetConsoleWindow(), SW_HIDE); }
	else { ShowWindow(::GetConsoleWindow(), SW_SHOW); }
	while (Initialised == false) {
		if (FindWindowA(NULL, TargetTitle)) {
			Initialised = true;
		}
	}

	HANDLE hMutex = (OpenMutexA)(MUTEX_ALL_ACCESS, 0, "CVhnljfxUvddaNk67f6v9t9BHQU9Qr3nZZQWRm2AClp6Bwu8");
	HANDLE hMutex1 = (OpenMutexA)(MUTEX_ALL_ACCESS, 0, "CVhnIjfUtxUvddaNk67RREGRGU9Qr3nZZWQRm2AClp6Bwu8");
	HANDLE hMutex2 = (OpenMutexA)(MUTEX_ALL_ACCESS, 0, "CVhnIjfUtxUvddFGDSFGsNk67fRREGRU9Qr3mZZQWRm2AClp8");

	/*if (!hMutex, !hMutex1, !hMutex2)
	{
		return 0;
	}*/

	pid = GetProcessIdByName("RustClient.exe");
	if (!pid) {
		MessageBox(NULL, "RustClient not found", NULL, MB_OK);
		Sleep(10000);
		return 0;
	}
	gDriver = new driver_manager("\\\\.\\rustzxcursedtest", pid);
	if (!gDriver) {
		std::cout << "Connection failed" << std::endl;
		Sleep(10000);	
		return 0;
	}
	if (gBase)
		std::cout << "GameAssembly.dll : " << gBase << endl;
	if (uBase)
		std::cout << "UnityPlayer.dll : " << uBase << endl;
	if (!gBase || !uBase) {
		MessageBox(NULL, "UnityPlayer.dll or GameAssembly.dll not found :\\", NULL, MB_OK);
		Sleep(1000);
		return 0;
	}
	Value::floats::Screen::W = GetSystemMetrics(SM_CXSCREEN);  // разрешение экрана по горизонтали
	Value::floats::Screen::H = GetSystemMetrics(SM_CYSCREEN); // разрешение экрана по вертикали
	InitLocalPlayer();
	SetupWindow();
	DirectXInit(Wnd);
	CreateThread(0, 0, ProcessCheck, 0, 0, 0);
	CreateThread(0, 0, LoopThread, 0, 0, 0);
	while (TRUE) {
		MainLoop();
	}
	return 0;
}

int main1() {
	if (CreateConsole == false) { ShowWindow(::GetConsoleWindow(), SW_HIDE); }
	else { ShowWindow(::GetConsoleWindow(), SW_SHOW); }
	while (Initialised == false) {
		if (FindWindowA(NULL, TargetTitle)) {
			Initialised = true;
		}
	}

	pid = GetProcessIdByName("RustClient.exe");
	if (!pid) {
		MessageBox(NULL, "RustClient not found", NULL, MB_OK);
		Sleep(10000);
		return 0;
	}
	/*gBase = driver::get_process_base_address(Sock, pid, 1);
	if (gBase)
		std::cout << "GameAssembly.dll : " << gBase << endl;
	uBase = driver::get_process_base_address(Sock, pid, 0);
	if (uBase)
		std::cout << "UnityPlayer.dll : " << uBase << endl;
	if (!gBase || !uBase)
		// std::cout << "UnityPlayer.dll or GameAssembly.dll not found :\\\n";
		MessageBox(NULL, "UnityPlayer.dll or GameAssembly.dll not found :\\", NULL, MB_OK);*/
	Value::floats::Screen::W = GetSystemMetrics(SM_CXSCREEN);  // разрешение экрана по горизонтали
	Value::floats::Screen::H = GetSystemMetrics(SM_CYSCREEN); // разрешение экрана по вертикали
	InitLocalPlayer();
	SetupWindow();
	DirectXInit(Wnd);
	CreateThread(0, 0, ProcessCheck, 0, 0, 0);
	CreateThread(0, 0, LoopThread, 0, 0, 0);
	while (TRUE) {
		MainLoop();
	}
	return 0;
}
