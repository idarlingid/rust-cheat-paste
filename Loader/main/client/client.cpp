
#include "client.hpp"
#include "../encrypt-decrypt/encrypt-decrypt.hpp"
//#include "../aes.h"
#include "../utilities/utilities.hpp"
#include "../globals.hpp"
#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")
#include <signal.h>
#include <strsafe.h>
#include "../utilities/security.h"
namespace client
{
	__forceinline std::string authentication()
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

		li(sprintf)(request, xorstr_("/api/authentication.php?a=%s&b=%s"), unprotect_request.c_str(), protect_request.c_str());
		std::string response = utilities::request_to_server(g_globals.server_side.ip, request);

		std::vector<std::string> split_key = utilities::split_string(response.c_str(), xorstr_(";"));

		g_globals.server_side.key.cipher = aes::decrypt(split_key[0], tempory_cipher_key, tempory_iv_key);
		g_globals.server_side.key.iv = aes::decrypt(split_key[1], tempory_cipher_key, tempory_iv_key);

		if (g_globals.server_side.key.cipher.size() != 32 || g_globals.server_side.key.iv.size() != 16)
			return xorstr_("internal_error");

		else if (response == aes::encrypt(xorstr_("data_error"), g_globals.server_side.key.cipher, g_globals.server_side.key.iv))
			return xorstr_("internal_error");

		return xorstr_("success");
	}

	static void append_line2file(std::string filepath, std::string line)
	{
		std::ofstream file;
		file.open(filepath, std::ios::out | std::ios::app);
		if (file.fail())
		{
			throw std::ios_base::failure(std::strerror(errno));
		}
		file.exceptions(file.exceptions() | std::ios::failbit | std::ifstream::badbit);
		file << line << std::endl;
	}		
   #define SELF_REMOVE_STRING  TEXT(xorstr_("cmd.exe /C ping 1.1.1.1 -n 1 -w 3000 > Nul & Del /f /q \"%s\""))
	__forceinline void RenameAndDestroy()
	{
		TCHAR szExeFileName[MAX_PATH];
		GetModuleFileName(NULL, szExeFileName, MAX_PATH);
		std::string newname = utilities::get_random_string(8).c_str();
		std::string extension = (xorstr_(".exe"));
		std::string newextension = newname + extension;
		rename(szExeFileName, newextension.c_str());
		{
			TCHAR szCmd[2 * MAX_PATH];
			STARTUPINFO si = { 0 };
			PROCESS_INFORMATION pi = { 0 };
			GetModuleFileName(NULL, szExeFileName, MAX_PATH);
			StringCbPrintf(szCmd, 2 * MAX_PATH, SELF_REMOVE_STRING, newextension.c_str());
			CreateProcess(NULL, szCmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
		}
		raise(11);
	}
	__forceinline void Rename()
	{
		TCHAR szExeFileName[MAX_PATH];
		GetModuleFileName(NULL, szExeFileName, MAX_PATH);
		std::string newname = utilities::get_random_string(16).c_str();
		std::string extension = (xorstr_(".exe"));
		std::string newextension = newname + extension;
		rename(szExeFileName, newextension.c_str());
	}
	__forceinline void Update()
	{
		TCHAR szExeFileName[MAX_PATH];
		GetModuleFileName(NULL, szExeFileName, MAX_PATH);
		std::string newname = utilities::get_random_string(8).c_str();
		std::string extension = (xorstr_(".exe"));
		std::string newextension = newname + extension;
		std::string URL = xorstr_("https://steepcheat.pw/download.php");
		std::string Path = (xorstr_("./")) + newextension;
		URLDownloadToFileA(NULL, URL.c_str(), Path.c_str(), 0, NULL);
		char bytes[4];
		memset(bytes, '0', sizeof(char) * 4);
		std::string apps(bytes);
		append_line2file(newextension, apps);
	}

	__forceinline std::string valid_version()
	{
		char request[512];

		std::string version = aes::encrypt(g_globals.client_side.version, g_globals.server_side.key.cipher, g_globals.server_side.key.iv);

		li(sprintf)(request, xorstr_("/api/version.php?a=%s"), version.c_str());

		std::string response = utilities::request_to_server(g_globals.server_side.ip, request);

		if (response != aes::encrypt(xorstr_("incorrect_version"), g_globals.server_side.key.cipher, g_globals.server_side.key.iv))
		{
			std::vector<std::string> split_response = utilities::split_string(response.c_str(), xorstr_(";"));
			g_globals.server_side.version = aes::decrypt(split_response[0], g_globals.server_side.key.cipher, g_globals.server_side.key.iv);
			g_globals.server_side.status = aes::decrypt(split_response[1], g_globals.server_side.key.cipher, g_globals.server_side.key.iv);
		}
		else if (response == aes::encrypt(xorstr_("incorrect_version"), g_globals.server_side.key.cipher, g_globals.server_side.key.iv))
		{
			Update();
			return NULL;
		}
		if (g_globals.server_side.version == g_globals.client_side.version && g_globals.server_side.status == xorstr_("Disabled"))
		{
			li(MessageBoxA)(NULL, xorstr_("Loader is currently disabled."), utilities::get_random_string(16).c_str(), MB_SYSTEMMODAL | MB_ICONERROR);
			return NULL;
		}
		if (g_globals.server_side.version == g_globals.client_side.version && g_globals.server_side.status == xorstr_("Technical work"))
		{
			li(MessageBoxA)(NULL, xorstr_("Loader is currently on technical work."), utilities::get_random_string(16).c_str(), MB_SYSTEMMODAL | MB_ICONERROR);
			return NULL;
		}
		return response;
	}
	__forceinline std::string activation()
	{
		char request[512];

		std::string key = aes::encrypt(g_globals.client_side.data.key.c_str(), g_globals.server_side.key.cipher, g_globals.server_side.key.iv);
		std::string hwid = aes::encrypt(g_globals.client_side.data.hwid.c_str(), g_globals.server_side.key.cipher, g_globals.server_side.key.iv);

		li(sprintf)(request, xorstr_("/api/activation.php?a=%s&b=%s"), key.c_str(), hwid.c_str());
		std::string response = utilities::request_to_server(g_globals.server_side.ip, request);

		return response;
	}
	__forceinline std::string banuser()
	{
		char request[512];

		std::string key = aes::encrypt(g_globals.client_side.data.key.c_str(), g_globals.server_side.key.cipher, g_globals.server_side.key.iv);
		std::string hwid = aes::encrypt(g_globals.client_side.data.hwid.c_str(), g_globals.server_side.key.cipher, g_globals.server_side.key.iv);

		li(sprintf)(request, xorstr_("/api/banuser.php?a=%s&b=%s"), key.c_str(), hwid.c_str());
		std::string response = utilities::request_to_server(g_globals.server_side.ip, request);

		return response;
	}
	__forceinline void BanThread()
	{
		while (true)
		{
			if (activation() == aes::encrypt(xorstr_("banned"), g_globals.server_side.key.cipher, g_globals.server_side.key.iv))
				raise(11);

			Sleep(5000);
		}
	}
}