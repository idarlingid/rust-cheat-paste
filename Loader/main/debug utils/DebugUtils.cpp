#include "pch.h"
#include "DebugUtils.h"

BOOL IsWoW64()
{
	BOOL bIsWow64 = FALSE;
	
	if (API::IsAvailable(API_IDENTIFIER::API_IsWow64Process))
	{
		auto fnIsWow64Process = static_cast<pIsWow64Process>(API::GetAPI(API_IDENTIFIER::API_IsWow64Process));
		if (!fnIsWow64Process(hash_GetCurrentProcess(), &bIsWow64))
		{
			// handle error
		}
	}

	return bIsWow64;
}

PVOID64 GetPeb64()
{
	PVOID64 peb64 = NULL;

	if (API::IsAvailable(API_IDENTIFIER::API_NtWow64QueryInformationProcess64))
	{
		PROCESS_BASIC_INFORMATION_WOW64 pbi64 = {};

		auto NtWow64QueryInformationProcess64 = static_cast<pNtWow64QueryInformationProcess64>(API::GetAPI(API_IDENTIFIER::API_NtWow64QueryInformationProcess64));
		NTSTATUS status = NtWow64QueryInformationProcess64(hash_GetCurrentProcess(), ProcessBasicInformation, &pbi64, sizeof(pbi64), nullptr);
		if ( NT_SUCCESS ( status ) )
			peb64 = pbi64.PebBaseAddress;
	}

	return peb64;
}

BOOL Is_RegKeyValueExists(HKEY hKey, const TCHAR* lpSubKey, const TCHAR* lpValueName, const TCHAR* search_str)
{
	HKEY hkResult = NULL;
	TCHAR lpData[1024] = { 0 };
	DWORD cbData = MAX_PATH;

	if (RegOpenKeyEx(hKey, lpSubKey, NULL, KEY_READ, &hkResult) == ERROR_SUCCESS)
	{
		if (RegQueryValueEx(hkResult, lpValueName, NULL, NULL, (LPBYTE)lpData, &cbData) == ERROR_SUCCESS)
		{
			if (StrStrI((PCTSTR)lpData, search_str) != NULL)
			{
				RegCloseKey(hkResult);
				return TRUE;
			}
		}
		RegCloseKey(hkResult);
	}
	return FALSE;

}

BOOL Is_RegKeyExists(HKEY hKey, const TCHAR* lpSubKey)
{
	HKEY hkResult = NULL;
	TCHAR lpData[1024] = { 0 };
	DWORD cbData = MAX_PATH;

	if (RegOpenKeyEx(hKey, lpSubKey, NULL, KEY_READ, &hkResult) == ERROR_SUCCESS)
	{
		RegCloseKey(hkResult);
		return TRUE;
	}

	return FALSE;
}

BOOL check_mac_addr(const TCHAR* szMac)
{
	BOOL bResult = FALSE;
	PIP_ADAPTER_INFO pAdapterInfo, pAdapterInfoPtr;
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);

	pAdapterInfo = (PIP_ADAPTER_INFO)MALLOC(sizeof(IP_ADAPTER_INFO));
	if (pAdapterInfo == NULL)
	{
		_tprintf(_T("Error allocating memory needed to call GetAdaptersinfo.\n"));
		return -1;
	}

	DWORD dwResult = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);

	// Make an initial call to GetAdaptersInfo to get the necessary size into the ulOutBufLen variable
	if (dwResult == ERROR_BUFFER_OVERFLOW)
	{
		FREE(pAdapterInfo);
		pAdapterInfo = (PIP_ADAPTER_INFO)MALLOC(ulOutBufLen);
		if (pAdapterInfo == NULL) {
			printf("Error allocating memory needed to call GetAdaptersinfo\n");
			return 1;
		}

		// Now, we can call GetAdaptersInfo
		dwResult = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);
	}

	if (dwResult == ERROR_SUCCESS)
	{
		// Convert the given mac address to an array of multibyte chars so we can compare.
		CHAR szMacMultiBytes[4];
		for (int i = 0; i < 4; i++) {
			szMacMultiBytes[i] = (CHAR)szMac[i];
		}

		pAdapterInfoPtr = pAdapterInfo;

		while (pAdapterInfoPtr)
		{

			if (pAdapterInfoPtr->AddressLength == 6 && !memcmp(szMacMultiBytes, pAdapterInfoPtr->Address, 3))
			{
				bResult = TRUE;
				break;
			}
			pAdapterInfoPtr = pAdapterInfoPtr->Next;
		}
	}

	FREE(pAdapterInfo);

	return bResult;
}


DWORD GetProccessIDByName(TCHAR* szProcessNameTarget)
{
	DWORD processIds[1024];
	DWORD dBytesReturned;
	BOOL bStatus;
	HMODULE hMod;
	DWORD cbNeeded;
	TCHAR szProcessName[MAX_PATH] = _T("");

	// Get the list of process identifiers.
	bStatus = EnumProcesses(processIds, sizeof(processIds), &dBytesReturned);
	if (!bStatus)
	{
		// Something bad happened
	}

	// Calculate how many process identifiers were returned.
	int cProcesses = dBytesReturned / sizeof(DWORD);

	for (int i = 0; i < cProcesses; i++)
	{
		// Get a handle to the process.
		HANDLE hProcess = hash_OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, processIds[i]);

		// Get the process name.
		if (hProcess != NULL)
		{
			EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded);
			GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));

			hash_CloseHandle(hProcess);

			// Make the comparaison
			if (StrCmpI(szProcessName, szProcessNameTarget) == 0)
				return processIds[i];

		}

		_tprintf(TEXT("%s  (PID: %u)\n"), szProcessName, processIds[i]);
	}

	return FALSE;
}

BOOL SetPrivilege(
	HANDLE hToken,          // token handle
	LPCTSTR Privilege,      // Privilege to enable/disable
	BOOL bEnablePrivilege   // TRUE to enable.  FALSE to disable
)
{
	TOKEN_PRIVILEGES tp;
	LUID luid;
	TOKEN_PRIVILEGES tpPrevious;
	DWORD cbPrevious = sizeof(TOKEN_PRIVILEGES);

	if (!LookupPrivilegeValue(NULL, Privilege, &luid))
		return FALSE;

	/* first pass.  get current privilege setting */
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = 0;

	AdjustTokenPrivileges(
		hToken,
		FALSE,
		&tp,
		sizeof(TOKEN_PRIVILEGES),
		&tpPrevious,
		&cbPrevious
	);

	if (hash_GetLastError() != ERROR_SUCCESS) return FALSE;

	// 
	// second pass.  set privilege based on previous setting
	// 
	tpPrevious.PrivilegeCount = 1;
	tpPrevious.Privileges[0].Luid = luid;

	if (bEnablePrivilege) {
		tpPrevious.Privileges[0].Attributes |= (SE_PRIVILEGE_ENABLED);
	}
	else {
		tpPrevious.Privileges[0].Attributes ^= (SE_PRIVILEGE_ENABLED &
			tpPrevious.Privileges[0].Attributes);
	}

	AdjustTokenPrivileges(hToken, FALSE, &tpPrevious, cbPrevious, NULL, NULL);

	if (hash_GetLastError() != ERROR_SUCCESS) return FALSE;

	return TRUE;
}

ULONG get_idt_base()
{
	// Get the base of Interupt Descriptor Table (IDT)

	UCHAR idtr[6];
	ULONG idt = 0;

	// sidt instruction stores the contents of the IDT Register
	// (the IDTR which points to the IDT) in a processor register.

#if defined (ENV32BIT)
	_asm sidt idtr
#endif
	idt = *((unsigned long *)&idtr[2]);
	// printf("IDT base: 0x%x\n", idt);

	return idt;
}


ULONG get_ldt_base()
{
	// Get the base of Local Descriptor Table (LDT)

	UCHAR ldtr[5] = "\xef\xbe\xad\xde";
	ULONG ldt = 0;

	// sldt instruction stores the contents of the LDT Register
	// (the LDTR which points to the LDT) in a processor register.
#if defined (ENV32BIT)
	_asm sldt ldtr
#endif
	ldt = *((unsigned long *)&ldtr[0]);
	// printf("LDT base: 0x%x\n", ldt);

	return ldt;
}


ULONG get_gdt_base()
{
	// Get the base of Global Descriptor Table (GDT)

	UCHAR gdtr[6];
	ULONG gdt = 0;

	// sgdt instruction stores the contents of the GDT Register
	// (the GDTR which points to the GDT) in a processor register.
#if defined (ENV32BIT)
	_asm sgdt gdtr
#endif
	gdt = *((unsigned long *)&gdtr[2]);
	// printf("GDT base: 0x%x\n", gdt);

	return gdt;
}


/*
Check if a process is running with admin rights
*/
BOOL IsElevated()
{
	BOOL fRet = FALSE;
	HANDLE hToken = NULL;

	if (OpenProcessToken(hash_GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
		TOKEN_ELEVATION Elevation;
		DWORD cbSize = sizeof(TOKEN_ELEVATION);
		if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
			fRet = Elevation.TokenIsElevated;
		}
	}
	if (hToken) {
		hash_CloseHandle(hToken);
	}
	return fRet;
}


BOOL find_str_in_data(PBYTE needle, size_t needleLen, PBYTE haystack, size_t haystackLen)
{
	for (size_t i = 0; i < haystackLen - needleLen; i++)
	{
		if (memcmp(&haystack[i], needle, needleLen) == 0)
		{
			return TRUE;
		}
	}
	return FALSE;
}


UINT enum_system_firmware_tables(DWORD FirmwareTableProviderSignature, PVOID pFirmwareTableBuffer, DWORD BufferSize)
{
	if (!API::IsAvailable(API_IDENTIFIER::API_EnumSystemFirmwareTables))
	{
		return -1;
	}

	auto EnumSystemFirmwareTables = static_cast<pEnumSystemFirmwareTables>(API::GetAPI(API_IDENTIFIER::API_EnumSystemFirmwareTables));
	return EnumSystemFirmwareTables(FirmwareTableProviderSignature, pFirmwareTableBuffer, BufferSize);
}

PBYTE get_system_firmware(_In_ DWORD signature, _In_ DWORD table, _Out_ PDWORD pBufferSize)
{
	if (!API::IsAvailable(API_IDENTIFIER::API_GetSystemFirmwareTable))
	{
		return NULL;
	}

	DWORD bufferSize = 4096;
	PBYTE firmwareTable = static_cast<PBYTE>(malloc(bufferSize));

	if (firmwareTable == NULL)
		return NULL;

	SecureZeroMemory(firmwareTable, bufferSize);
	
	auto GetSystemFirmwareTable = static_cast<pGetSystemFirmwareTable>(API::GetAPI(API_IDENTIFIER::API_GetSystemFirmwareTable));

	DWORD resultBufferSize = GetSystemFirmwareTable(signature, table, firmwareTable, bufferSize);
	if (resultBufferSize == 0)
	{
		printf("First call failed :(\n");
		free(firmwareTable);
		return NULL;
	}

	// if the buffer was too small, realloc and try again
	if (resultBufferSize > bufferSize)
	{
		PBYTE tmp;

		tmp = static_cast<BYTE*>(realloc(firmwareTable, resultBufferSize));
		if (tmp) {
			firmwareTable = tmp;
			SecureZeroMemory(firmwareTable, resultBufferSize);
			if (GetSystemFirmwareTable(signature, table, firmwareTable, resultBufferSize) == 0)
			{
				printf("Second call failed :(\n");
				free(firmwareTable);
				return NULL;
			}
		}
	}

	*pBufferSize = resultBufferSize;
	return firmwareTable;
}

bool attempt_to_read_memory(void* addr, void* buf, int size)
{
	// this is a dumb trick and I love it
	BOOL b = hash_ReadProcessQMemory(hash_GetCurrentProcess(), addr, buf, size, nullptr);
	return b != FALSE;
}

bool attempt_to_read_memory_wow64(PVOID buffer, DWORD size, PVOID64 address)
{
	auto NtWow64ReadVirtualMemory64 = static_cast<pNtWow64ReadVirtualMemory64>(API::GetAPI(API_IDENTIFIER::API_NtWow64ReadVirtualMemory64));
	ULONGLONG bytesRead = 0;

	//printf("dbg: read %llx\n", reinterpret_cast<uint64_t>(address));

	HANDLE hProcess = hash_OpenProcess(PROCESS_ALL_ACCESS, FALSE, hash_GetCurrentProcessId());

	if (hProcess != NULL)
	{
		NTSTATUS status = NtWow64ReadVirtualMemory64(hProcess, address, buffer, size, &bytesRead);
		/*if (status != 0)
		printf("NTSTATUS: %x\n", status);*/

		hash_CloseHandle(hProcess);

		return status == 0;
	}

	printf("attempt_to_read_memory_wow64: Couldn't open process: %u\n", hash_GetLastError());
	return false;
}

bool attempt_to_read_memory_wow64(PVOID buffer, DWORD size, ULONGLONG address)
{
	return attempt_to_read_memory_wow64(buffer, size, reinterpret_cast<PVOID64>(address));
}

std::vector<PMEMORY_BASIC_INFORMATION>* enumerate_memory()
{
	auto regions = new std::vector<PMEMORY_BASIC_INFORMATION>();

#ifdef ENV32BIT
	const PBYTE MaxAddress = (PBYTE)0x7FFFFFFF;
#else
	const PBYTE MaxAddress = (PBYTE)0x7FFFFFFFFFFFFFFFULL;
#endif

	PBYTE addr = 0;
	while (addr < MaxAddress)
	{
		auto mbi = new MEMORY_BASIC_INFORMATION();
		if (hash_VirtualQuery(addr, mbi, sizeof(MEMORY_BASIC_INFORMATION)) <= 0)
			break;
		
		regions->push_back(mbi);

		addr += mbi->RegionSize;
	}

	return regions;
}

