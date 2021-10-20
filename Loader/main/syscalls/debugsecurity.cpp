#include "includes.h"
#include "util.h"
#include "syscalls.h"
#include "apiset.h"
#include <signal.h>
#include "debugsecurity.h"
#include <thread>
#include "..\debug utils/XorStr.h"
#include "..\utilities\security.h"
#include "..\client/client.hpp"
#pragma comment(lib,"ntdll.lib")
std::unordered_map<std::string, std::vector<char>> debugsecurity::parsed_images;
__forceinline bool stop()
{
	return raise(11);
}
__forceinline bool read_file(const std::string_view path, std::vector<char>& out)
{
	std::ifstream file(path.data(), std::ios::binary);
	if (!file.good()) 
	{
		//std::cout << "{} isnt valid." << path << std::endl;
		return false;
	}

	file.unsetf(std::ios::skipws);

	file.seekg(0, std::ios::end);
	const size_t size = file.tellg();
	file.seekg(0, std::ios::beg);

	out.resize(size);

	file.read(&out[0], size);

	file.close();

	return true;
}
__forceinline bool init()
{
	std::list<std::string> blacklist = { "ntdll.dll", "kernel32.dll" };

	std::unordered_map<std::string, pe::virtual_image> memory_modules;
	std::unordered_map<std::string, pe::image<true>> disk_modules;
	if (!get_all_modules(memory_modules))
	{
		//std::cout << "failed to get loaded modules" << std::endl;
		return false;
	}

	for (auto& [name, vi] : memory_modules) {
		auto it = std::find(blacklist.begin(), blacklist.end(), name);
		if (it == blacklist.end()) {
			continue;
		}

		std::vector<char> raw;
		char path[MAX_PATH];
		GetModuleFileNameA(GetModuleHandleA(name.c_str()), path, MAX_PATH);

		if (!read_file(path, raw))
		{
			//std::cout << "failed to read{}" << name << std::endl;
			continue;
		}

		disk_modules[name] = pe::image<true>(raw);
	}

	for (auto& [name, image] : disk_modules) {
		std::vector<char> mem;

		image.copy(mem);
		image.relocate(mem, uintptr_t(GetModuleHandleA(name.c_str())));

		for (auto& [mod, funcs] : image.imports()) {
			std::string mod_name{ mod };
			g_apiset.find(mod_name);

			for (auto& func : funcs) {
				*reinterpret_cast<uintptr_t*>(&mem[func.rva]) = uintptr_t(GetProcAddress(GetModuleHandleA(mod_name.c_str()), func.name.c_str()));
			}
		}

		debugsecurity::parsed_images[name] = mem;
	}

	disk_modules.clear();
	memory_modules.clear();

	return !debugsecurity::parsed_images.empty();
}
void debugsecurity::thread()
{
	if (!init()) 
	{
		//std::cout << "failed to init security thread.";
		stop();
	}

	while (true) 
	{
		check();
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		/*std::this_thread::sleep_for(chrono::seconds(1));*/
	}
}

__forceinline bool debugsecurity::check() 
{
	static auto peb = util::peb();
	auto being_debugged = static_cast<bool>(peb->BeingDebugged);
	if (being_debugged) 
	{
		stop();
		return true;
	}


	static auto query_info = g_syscalls.get<native::NtQueryInformationProcess>("NtQueryInformationProcess");

	uint32_t debug_inherit = 0;
	auto status = query_info(INVALID_HANDLE_VALUE, native::ProcessDebugFlags, &debug_inherit, sizeof(debug_inherit), 0);
	if (!NT_SUCCESS(status)) 
	{
		stop();
		//std::cout << "failed to get local process debug flags, status {:#X}." << (status & 0xFFFFFFFF);
		return true;
	}

	if (debug_inherit == 0) 
	{
		stop();
		return true;
	}

	uint64_t remote_debug = 0;
	status = query_info(INVALID_HANDLE_VALUE, native::ProcessDebugPort, &remote_debug, sizeof(remote_debug), 0);
	if (!NT_SUCCESS(status)) 
	{
		stop();
		//std::cout << "failed to get local process debug port, status {:#X}." << (status & 0xFFFFFFFF) << std::endl;
		return true;
	}

	if (remote_debug != 0) 
	{
		stop();
		return true;
	}

	return false;
}