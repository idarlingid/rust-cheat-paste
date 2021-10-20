#include <string>
#include <random>
#define VC_EXTRALEAN
#include <Windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <shlwapi.h>
#include <memory>
#include <comdef.h>

namespace Utilities
{

	__forceinline std::string Get_random_string(size_t length)
	{
		std::string str(_("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890"));
		std::random_device rd;
		std::mt19937 generator(rd());
		std::shuffle(str.begin(), str.end(), generator);
		return str.substr(0, length);
	}

	__forceinline std::string request_to_server(std::string site, std::string param)
	{
		HINTERNET hInternet = InternetOpenW(_(L""), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);

		if (hInternet == NULL)
		{
			//li(MessageBoxA)(NULL, _("Cannot connect to server."), utilities::get_random_string(16).c_str(), MB_SYSTEMMODAL | MB_ICONERROR);
			return NULL;
		}
		else
		{
			std::wstring widestr;
			for (int i = 0; i < site.length(); ++i)
			{
				widestr += wchar_t(site[i]);
			}
			const wchar_t* site_name = widestr.c_str();

			std::wstring widestr2;
			for (int i = 0; i < param.length(); ++i)
			{
				widestr2 += wchar_t(param[i]);
			}
			const wchar_t* site_param = widestr2.c_str();

			HINTERNET hConnect = InternetConnectW(hInternet, site_name, 80, NULL, NULL, INTERNET_SERVICE_HTTP, 0, NULL);

			if (hConnect == NULL)
			{
				MessageBoxA(NULL, _("Error sending message to server"), Utilities::Get_random_string(16).c_str(), MB_SYSTEMMODAL | MB_ICONERROR);
				return NULL;
			}
			else
			{
				const wchar_t* parrAcceptTypes[] = { _(L"text/*"), NULL };

				HINTERNET hRequest = HttpOpenRequestW(hConnect, _(L"POST"), site_param, NULL, NULL, parrAcceptTypes, 0, 0);

				if (hRequest == NULL)
				{
					(MessageBoxA)(NULL, _("Error sending message to server"), Utilities::Get_random_string(16).c_str(), MB_SYSTEMMODAL | MB_ICONERROR);
					return NULL;
				}
				else
				{
					BOOL bRequestSent = (HttpSendRequestW)(hRequest, NULL, 0, NULL, 0);

					if (!bRequestSent)
					{
						(MessageBoxA)(NULL, _("Error sending message to server"), Utilities::Get_random_string(16).c_str(), MB_SYSTEMMODAL | MB_ICONERROR);
						return NULL;
					}
					else
					{
						std::string strResponse;
						const int nBuffSize = 1024;
						char buff[nBuffSize];

						BOOL bKeepReading = true;
						DWORD dwBytesRead = -1;

						while (bKeepReading && dwBytesRead != 0)
						{
							bKeepReading = (InternetReadFile)(hRequest, buff, nBuffSize, &dwBytesRead);
							strResponse.append(buff, dwBytesRead);
						}
						return strResponse;
					}
					(InternetCloseHandle)(hRequest);
				}
				(InternetCloseHandle)(hConnect);
			}
			(InternetCloseHandle)(hInternet);
		}
	}

	const char* cryptKey(char str[], int KEY)
	{
		for (int i = 0; i < strlen(str); i++)
			str[i] += KEY;
		const char* ret = str;
		return str;
	}

	const wchar_t* cryptKeyW(wchar_t str[], int KEY)
	{
		for (int i = 0; i < wcslen(str); i++)
			str[i] += KEY;

		return str;
	}

	__forceinline void strip_string(std::string& str)
	{
		str.erase(std::remove_if(str.begin(), str.end(), [](int c) {return !(c > 32 && c < 127); }), str.end());
	}

	__forceinline std::vector<std::string> split_string(const std::string& str, const std::string& delim)
	{
		std::vector<std::string> tokens;
		size_t prev = 0, pos = 0;
		do
		{
			pos = str.find(delim, prev);
			if (pos == std::string::npos) pos = str.length();
			std::string token = str.substr(prev, pos - prev);
			if (!token.empty()) tokens.push_back(token);
			prev = pos + delim.length();

		} while (pos < str.length() && prev < str.length());

		return tokens;
	}

	__forceinline std::string GetHWID()
	{
		std::string result = "";

		HANDLE hDevice = CreateFileA(_("\\\\.\\PhysicalDrive0"), (DWORD)nullptr, FILE_SHARE_READ | FILE_SHARE_WRITE, (LPSECURITY_ATTRIBUTES)nullptr, OPEN_EXISTING, (DWORD)nullptr, (HANDLE)nullptr);

		if (hDevice == INVALID_HANDLE_VALUE) return result;

		STORAGE_PROPERTY_QUERY storagePropertyQuery;
		ZeroMemory(&storagePropertyQuery, sizeof(STORAGE_PROPERTY_QUERY));
		storagePropertyQuery.PropertyId = StorageDeviceProperty;
		storagePropertyQuery.QueryType = PropertyStandardQuery;

		STORAGE_DESCRIPTOR_HEADER storageDescriptorHeader = { 0 };
		DWORD dwBytesReturned = 0;

		DeviceIoControl
			(
				hDevice,
				IOCTL_STORAGE_QUERY_PROPERTY,
				&storagePropertyQuery,
				sizeof(STORAGE_PROPERTY_QUERY),
				&storageDescriptorHeader,
				sizeof(STORAGE_DESCRIPTOR_HEADER),
				&dwBytesReturned,
				nullptr
				);

		const DWORD dwOutBufferSize = storageDescriptorHeader.Size;
		BYTE* pOutBuffer = new BYTE[dwOutBufferSize];
		ZeroMemory(pOutBuffer, dwOutBufferSize);

		DeviceIoControl
			(
				hDevice,
				IOCTL_STORAGE_QUERY_PROPERTY,
				&storagePropertyQuery,
				sizeof(STORAGE_PROPERTY_QUERY),
				pOutBuffer,
				dwOutBufferSize,
				&dwBytesReturned,
				nullptr
				);

		STORAGE_DEVICE_DESCRIPTOR* pDeviceDescriptor = (STORAGE_DEVICE_DESCRIPTOR*)pOutBuffer;

		if (pDeviceDescriptor->SerialNumberOffset)
		{
			result += std::string((char*)(pOutBuffer + pDeviceDescriptor->SerialNumberOffset));
		}

		if (pDeviceDescriptor->ProductRevisionOffset)
		{
			result += std::string((char*)(pOutBuffer + pDeviceDescriptor->ProductRevisionOffset));
		}

		if (pDeviceDescriptor->ProductIdOffset)
		{
			result += std::string((char*)(pOutBuffer + pDeviceDescriptor->ProductIdOffset));
		}

		uint32_t regs[4];
		__cpuid((int*)regs, 0);

		std::string vendor;

		vendor += std::string((char*)&regs[1], 4);
		vendor += std::string((char*)&regs[3], 4);
		vendor += std::string((char*)&regs[2], 4);

		result += std::string(vendor);

		strip_string(result);

		delete[] pOutBuffer;
		CloseHandle(hDevice);

		result = md5::create_from_string(result);

		return result;
	}

	BOOL IsProcessRunning(DWORD pid)
	{
		HANDLE process = OpenProcess(SYNCHRONIZE, FALSE, pid);
		DWORD ret = WaitForSingleObject(process, 0);
		CloseHandle(process);
		return ret == WAIT_TIMEOUT;
	}
	_forceinline std::string get_random_string(size_t length)
	{
		const static std::string chrs = (string)_("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

		thread_local static std::mt19937 rg{ std::random_device{}() };
		thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(chrs) - 2);

		std::string s;

		s.reserve(length);

		while (length--)
			s += chrs[pick(rg)];

		return s;
	}
	HANDLE GetProcessByName(const char* name)
	{
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		PROCESSENTRY32 entry;
		entry.dwSize = sizeof(entry);
		HANDLE process;
		ZeroMemory(&process, sizeof(HANDLE));

		do {
			if (!strcmp(name, _bstr_t(entry.szExeFile)))
			{
				process = OpenProcess(PROCESS_ALL_ACCESS, false, entry.th32ProcessID);
				CloseHandle(hSnapshot);
			}
		} while (Process32Next(hSnapshot, &entry));

		return process;
	}
}