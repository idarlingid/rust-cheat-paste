//#include "menu/registrywindow.hpp"
#include "..\client/client.hpp"
#include "..\utilities/security.h"
#include <fstream>
#include <signal.h>
#include <thread>
#pragma once
namespace sec
{
	__forceinline std::string sec::getCurrentDateTime()
	{
		time_t now = time(0);
		struct tm  tstruct;
		char  buf[80];
		tstruct = *localtime(&now);
		strftime(buf, sizeof(buf), "%Y-%m-%d - %H-%M-%S", &tstruct);
		return std::string(buf);
	};

	__forceinline void sec::Logger(std::string logMsg, int Mode)
	{
		std::ofstream log_file("logs.txt", std::ios_base::out | std::ios_base::app);
		switch (Mode)
		{
		case 0:
		{
			if (log_file.is_open())
			{
				log_file << "[ Date: " << sec::getCurrentDateTime() << " ] " << "[ success ] - " << logMsg << std::endl;
			}
			break;
		}
		case 1:
		{
			if (log_file.is_open())
			{
				log_file << "[ Date: " << sec::getCurrentDateTime() << " ] " << "[ error ] - " << logMsg << std::endl;
			}
			break;
		}
		case 2:
		{
			if (log_file.is_open())
			{
				log_file << "[ Date: " << sec::getCurrentDateTime() << " ] " << "[ banned ] - " << logMsg << std::endl;
			}
			break;
		}
		log_file.close();
		}
	}
	__forceinline BOOL sec::IsRemoteSession(void)
	{
		return GetSystemMetrics(SM_REMOTESESSION);
	}

	__forceinline BOOL sec::EnablePriv(LPCSTR lpszPriv)
	{
		HANDLE hToken;
		LUID luid;
		TOKEN_PRIVILEGES tkprivs;
		ZeroMemory(&tkprivs, sizeof(tkprivs));

		if (!OpenProcessToken(GetCurrentProcess(), (TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY), &hToken))
			return FALSE;

		if (!LookupPrivilegeValue(NULL, lpszPriv, &luid)) {
			CloseHandle(hToken); return FALSE;
		}

		tkprivs.PrivilegeCount = 1;
		tkprivs.Privileges[0].Luid = luid;
		tkprivs.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		BOOL bRet = AdjustTokenPrivileges(hToken, FALSE, &tkprivs, sizeof(tkprivs), NULL, NULL);
		CloseHandle(hToken);
		return bRet;
	}
	__forceinline void sec::shutdown()
	{
		raise(11);
	}

	__forceinline void sec::Session()
	{
		std::this_thread::sleep_for(std::chrono::seconds(240));
		Logger(xorstr_("Session clossed because expired."), 1);
		shutdown();
	}

	__forceinline BOOL sec::MakeCritical()
	{
		HANDLE hDLL;
		RtlSetProcessIsCritical fSetCritical;

		hDLL = LoadLibraryA(xorstr_("ntdll.dll"));
		if (hDLL != NULL)
		{
			EnablePriv(SE_DEBUG_NAME);
			(fSetCritical) = (RtlSetProcessIsCritical)GetProcAddress((HINSTANCE)hDLL, (xorstr_("RtlSetProcessIsCritical")));
			if (!fSetCritical) return 0;
			fSetCritical(1, 0, 0);
			return 1;
		}
		else
			return 0;
	}

	__forceinline void sec::killProcessByName(const char* filename)
	{
		HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
		PROCESSENTRY32 pEntry;
		pEntry.dwSize = sizeof(pEntry);
		BOOL hRes = Process32First(hSnapShot, &pEntry);
		while (hRes)
		{
			if (strcmp(pEntry.szExeFile, filename) == 0)
			{
				HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
					(DWORD)pEntry.th32ProcessID);
				if (hProcess != NULL)
				{
					TerminateProcess(hProcess, 9);
					CloseHandle(hProcess);
				}
			}
			hRes = Process32Next(hSnapShot, &pEntry);
		}
		CloseHandle(hSnapShot);
	}

	__forceinline bool sec::IsDebuggersInstalledStart()
	{
		LPVOID drivers[2048];
		DWORD cbNeeded;
		int cDrivers, i;

		if (EnumDeviceDrivers(drivers, sizeof(drivers), &cbNeeded) && cbNeeded < sizeof(drivers))
		{
			TCHAR szDriver[2048];

			cDrivers = cbNeeded / sizeof(drivers[0]);

			for (i = 0; i < cDrivers; i++)
			{
				if (GetDeviceDriverBaseName(drivers[i], szDriver, sizeof(szDriver) / sizeof(szDriver[0])))
				{
					std::string strDriver = szDriver;
					if (strDriver.find("kprocesshacker") != std::string::npos)
					{
						Logger(xorstr_("Delete Process Hacker, before launching loader. And restart your PC."), 1);
						return true;
					}
					if (strDriver.find("HttpDebug") != std::string::npos)
					{
						Logger(xorstr_("Delete HTTP Debugger, before launching loader. And restart your PC."), 1);
						return true;
					}
					if (strDriver.find("npf") != std::string::npos)
					{
						Logger(xorstr_("Delete Wireshark, before launching loader. And restart your PC."), 1);
						return true;
					}
					if (strDriver.find("TitanHide") != std::string::npos)
					{
						Logger(xorstr_("Delete TitanHide, before launching loader. And restart your PC."), 1);
						return true;
					}
					if (strDriver.find("vgk") != std::string::npos)
					{
						Logger(xorstr_("Disable Vanguard Anti-Cheat, before launching loader. And restart your PC."), 1);
						return true;
					}
					if (strDriver.find("faceitac") != std::string::npos)
					{
						Logger(xorstr_("Disable FaceIt Anti-Cheat, before launching loader. And restart your PC."), 1);
						return true;
					}
					if (strDriver.find("EasyAntiCheat") != std::string::npos)
					{
						Logger(xorstr_("Disable EAC Anti-Cheat, before launching loader. And restart your PC."), 1);
						return true;
					}
					if (strDriver.find("BEDaisy") != std::string::npos)
					{
						Logger(xorstr_("Disable Battleye Anti-Cheat, before launching loader. And restart your PC."), 1);
						return true;
					}
					if (strDriver.find("SharpOD_Drv") != std::string::npos)
					{
						Logger(xorstr_("Remove SharpOD, before launching loader. And restart your PC."), 1);
						return true;
					}
				}
			}
		}
		return false;
	}

	__forceinline bool sec::IsDebuggersInstalledThread()
	{
		LPVOID drivers[2048];
		DWORD cbNeeded;
		int cDrivers, i;

		if (EnumDeviceDrivers(drivers, sizeof(drivers), &cbNeeded) && cbNeeded < sizeof(drivers))
		{
			TCHAR szDriver[2048];

			cDrivers = cbNeeded / sizeof(drivers[0]);

			for (i = 0; i < cDrivers; i++)
			{
				if (GetDeviceDriverBaseName(drivers[i], szDriver, sizeof(szDriver) / sizeof(szDriver[0])))
				{
					std::string strDriver = szDriver;
					if (strDriver.find("kprocesshacker") != std::string::npos)
					{
						Logger(xorstr_("Delete Process Hacker, before launching loader. And restart your PC."), 1);
						return true;
					}
					if (strDriver.find("HttpDebug") != std::string::npos)
					{
						Logger(xorstr_("Delete HTTP Debugger, before launching loader. And restart your PC."), 1);
						return true;
					}
					if (strDriver.find("npf") != std::string::npos)
					{
						Logger(xorstr_("Delete Wireshark, before launching loader. And restart your PC."), 1);
						return true;
					}
					if (strDriver.find("TitanHide") != std::string::npos)
					{
						Logger(xorstr_("Delete TitanHide, before launching loader. And restart your PC."), 1);
						return true;
					}
					if (strDriver.find("SharpOD_Drv") != std::string::npos)
					{
						Logger(xorstr_("Remove SharpOD, before launching loader. And restart your PC."), 1);
						return true;
					}
				}
			}
		}
		return false;
	}

	__forceinline DWORD sec::GetProcessIdFromName(LPCTSTR szProcessName)
	{
		PROCESSENTRY32 pe32;
		HANDLE hSnapshot = NULL;
		SecureZeroMemory(&pe32, sizeof(PROCESSENTRY32));

		// We want a snapshot of processes
		hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		// Check for a valid handle, in this case we need to check for
		// INVALID_HANDLE_VALUE instead of NULL
		if (hSnapshot == INVALID_HANDLE_VALUE)
		{
			return 0;
		}

		// Now we can enumerate the running process, also 
		// we can't forget to set the PROCESSENTRY32.dwSize member
		// otherwise the following functions will fail
		pe32.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(hSnapshot, &pe32) == FALSE)
		{
			// Cleanup the mess
			CloseHandle(hSnapshot);
			return 0;
		}

		// Do our first comparison
		if (StrCmpI(pe32.szExeFile, szProcessName) == 0)
		{
			// Cleanup the mess
			CloseHandle(hSnapshot);
			return pe32.th32ProcessID;
		}

		// Most likely it won't match on the first try so 
		// we loop through the rest of the entries until
		// we find the matching entry or not one at all
		while (Process32Next(hSnapshot, &pe32))
		{
			if (StrCmpI(pe32.szExeFile, szProcessName) == 0)
			{
				// Cleanup the mess
				CloseHandle(hSnapshot);
				return pe32.th32ProcessID;
			}
		}
		// If we made it this far there wasn't a match, so we'll return 0
		// _tprintf(_T("\n-> Process %s is not running on this system ..."), szProcessName);

		CloseHandle(hSnapshot);
		return 0;
	}

	__forceinline bool sec::analysis()
	{
		std::string szProcesses[] =
		{
			xorstr_("ollydbg.exe"),
			xorstr_("x64dbg.exe"),
			xorstr_("x32dbg.exe"),
			xorstr_("die.exe"),
			xorstr_("tcpview.exe"),			// Part of Sysinternals Suite
			xorstr_("autoruns.exe"),			// Part of Sysinternals Suite
			xorstr_("autorunsc.exe"),		// Part of Sysinternals Suite
			xorstr_("filemon.exe"),			// Part of Sysinternals Suite
			xorstr_("procmon.exe"),			// Part of Sysinternals Suite
			xorstr_("regmon.exe"),			// Part of Sysinternals Suite
			xorstr_("procexp.exe"),			// Part of Sysinternals Suite
			xorstr_("idaq.exe"),				// IDA Pro Interactive Disassembler
			xorstr_("idaq64.exe"),			// IDA Pro Interactive Disassembler
			xorstr_("ida.exe"),				// IDA Pro Interactive Disassembler
			xorstr_("ida64.exe"),			// IDA Pro Interactive Disassembler
			xorstr_("ImmunityDebugger.exe"), // ImmunityDebugger
			xorstr_("Wireshark.exe"),		// Wireshark packet sniffer
			xorstr_("dumpcap.exe"),			// Network traffic dump tool
			xorstr_("HookExplorer.exe"),		// Find various types of runtime hooks
			xorstr_("ImportREC.exe"),		// Import Reconstructor
			xorstr_("PETools.exe"),			// PE Tool
			xorstr_("LordPE.exe"),			// LordPE
			xorstr_("dumpcap.exe"),			// Network traffic dump tool
			xorstr_("SysInspector.exe"),		// ESET SysInspector
			xorstr_("proc_analyzer.exe"),	// Part of SysAnalyzer iDefense
			xorstr_("sysAnalyzer.exe"),		// Part of SysAnalyzer iDefense
			xorstr_("sniff_hit.exe"),		// Part of SysAnalyzer iDefense
			xorstr_("windbg.exe"),			// Microsoft WinDbg
			xorstr_("joeboxcontrol.exe"),	// Part of Joe Sandbox
			xorstr_("joeboxserver.exe"),		// Part of Joe Sandbox
			xorstr_("fiddler.exe"),
			xorstr_("tv_w32.exe"),
			xorstr_("tv_x64.exe"),
			xorstr_("Charles.exe"),
			xorstr_("netFilterService.exe"),
			xorstr_("HTTPAnalyzerStdV7.exe")
		};

		WORD iLength = sizeof(szProcesses) / sizeof(szProcesses[0]);
		for (int i = 0; i < iLength; i++)
		{
			if (GetProcessIdFromName(szProcesses[i].c_str()))
			{
				killProcessByName(szProcesses[i].c_str());
				return true;
			}
		}
		return false;
	}

	__forceinline bool sec::TestSign()
	{
		HMODULE ntdll = hash_GetModuleHandleA(xorstr_("ntdll.dll"));

		auto NtQuerySystemInformation = (t_NtQuerySystemInformation)hash_GetProcAddress(ntdll, xorstr_("NtQuerySystemInformation"));

		SYSTEM_CODEINTEGRITY_INFORMATION cInfo;
		cInfo.Length = sizeof(cInfo);

		NtQuerySystemInformation(
			SystemCodeIntegrityInformation,
			&cInfo,
			sizeof(cInfo),
			NULL
		);

		return (cInfo.CodeIntegrityOptions & CODEINTEGRITY_OPTION_TESTSIGN)
			|| (cInfo.CodeIntegrityOptions & CODEINTEGRITY_OPTION_DEBUGMODE_ENABLED);
	}

	__forceinline void sec::clown()
	{
		MakeCritical();
		shutdown();
	}

	__forceinline bool sec::start()
	{
		return IsRemoteSession() || IsDebuggersInstalledThread() || analysis();
	}
	__forceinline void sec::ErasePEHeaderFromMemory()
	{
		DWORD OldProtect = 0;

		// Get base address of module
		char* pBaseAddr = (char*)GetModuleHandle(NULL);

		// Change memory protection
		VirtualProtect(pBaseAddr, 4096, // Assume x86 page size
			PAGE_READWRITE, &OldProtect);

		// Erase the header
		ZeroMemory(pBaseAddr, 4096);
	}
	__forceinline bool HideThread(HANDLE hThread)
	{
		typedef NTSTATUS(NTAPI* pNtSetInformationThread)
			(HANDLE, UINT, PVOID, ULONG);

		NTSTATUS Status;

		// Get NtSetInformationThread
		pNtSetInformationThread NtSIT = (pNtSetInformationThread)
			GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")), "NtSetInformationThread");
		// Shouldn't fail
		if (NtSIT == NULL)
			return false;

		// Set the thread info
		if (hThread == NULL)
			Status = NtSIT(GetCurrentThread(),
				0x11, //ThreadHideFromDebugger
				0, 0);
		else
			Status = NtSIT(hThread, 0x11, 0, 0);

		if (Status != 0x00000000)
			return false;
		else
			return true;
	}
	void checkPEB()
	{
		PBOOLEAN BeingDebugged = (PBOOLEAN)__readgsqword(0x60) + 2;
		if (*BeingDebugged)
		{
			shutdown();
		}
	}
	__forceinline void sec::ST()
	{
		HideThread(GetCurrentThread);
		while (true)
		{
			if (start())
			{
				client::banuser();
				Logger(xorstr_("Analysis Try."), 2);
				clown();
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
	}
}