#pragma once
#include <windows.h>
#include <string>
#include <random>
#include <vector>
#include <WinInet.h>
#include <fstream>

#include "lazy_importer.hpp"
#pragma comment(lib, "WinINet.lib")


namespace utilities
{
	extern __forceinline std::string get_random_string(size_t length);
	extern __forceinline void strip_string(std::string& str);
	extern __forceinline BOOL ProcessExists(const char* const processName);
	extern __forceinline std::vector<std::string> split_string(const std::string& str, const std::string& delim);
	extern __forceinline std::string request_to_server(std::string site, std::string param);
	extern __forceinline std::string get_hwid();
	extern __forceinline std::string MAC();
	template<class T>
	 T get_export(const std::string& dll_name, const std::string& function_name)
	{
		 return (T)(li(GetProcAddress)(li(GetModuleHandleA)(dll_name.c_str()), function_name.c_str()));
	}
}