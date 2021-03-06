
// Tips for Getting Started:
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here



#include <string>
#include <vector>

#include <Windows.h>
#include <winternl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <IPTypes.h>
#include <Iphlpapi.h>
#include <icmpapi.h>
#include <Psapi.h>
#include <Shlwapi.h>
#include <ShlObj.h>
#include <stdarg.h>
#include <strsafe.h>
#include <tchar.h>
#include <time.h>
#include <TlHelp32.h>
#include <Wbemidl.h>
#include <devguid.h>    // Device guids
#include <winioctl.h>	// IOCTL
#include <intrin.h>		// cpuid()
#include <locale.h>		// 64-bit wchar atoi
#include <powrprof.h>	// check_power_modes()
#include <SetupAPI.h>
#include <ImageHlp.h>
#include <Softpub.h>
#include <wintrust.h>

#pragma comment(lib,"ImageHlp.lib")
#pragma comment(lib,"wintrust.lib")
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Mpr.lib")
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "powrprof.lib")
#include "VersionHelpers.h"
#include "DebugUtils.h"
#include "WinStructs.h"
#include "ApiTypeDefs.h"
#include "APIs.h"
#include "winapifamily.h"

// Win API Obfuscation
#include "MurmurHash2A.h"
#include "hash_work.h"
#include "export_work.h"

// XAntiDebug
#include "CMemPtr.h"
#include "crc32.h"
#include "internal.h"
#include "ldasm.h"
// ScyllaHide Detector
#include "Native.h"
#include "Hash.h"
#define JM_XORSTR_DISABLE_AVX_INTRINSICS
#include "XorStr.h"
#include "Helpers.h"
#include "ScyllaHideDetector.h"

// GH Syscalls
#include "nt_defs.h"
#include "syscall_defs.h"
#include "gh_syscall.h"

/* AntiDebugs headers */


#endif //PCH_H
