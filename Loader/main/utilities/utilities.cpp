#include "utilities.hpp"
#include "../encrypt-decrypt/md5.hpp"
#include <TlHelp32.h>
#include <iphlpapi.h>
#include <signal.h>


namespace utilities
{
	__forceinline std::string get_random_string(size_t length)
	{
		std::string str(xorstr_("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890"));
		std::random_device rd;
		std::mt19937 generator(rd());
		std::shuffle(str.begin(), str.end(), generator);
		return str.substr(0, length);
	}
	__forceinline void strip_string(std::string& str)
	{
		str.erase(std::remove_if(str.begin(), str.end(), [](int c) {return !(c > 32 && c < 127); }), str.end());
	}

	__forceinline BOOL ProcessExists(const char* const processName)
	{
		HANDLE hProcessSnap;
		PROCESSENTRY32 pe32;
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		pe32.dwSize = sizeof(PROCESSENTRY32);
		do {
			if (strcmp(pe32.szExeFile, processName) == 0)
			{
				CloseHandle(hProcessSnap);
				return true;
			}
		} while (Process32Next(hProcessSnap, &pe32));

		CloseHandle(hProcessSnap);
		return false;
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
	__forceinline std::string request_to_server(std::string site, std::string param)
	{
		HINTERNET hInternet = InternetOpenW(xorstr_(L""), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);

		if (hInternet == NULL)
		{
			li(MessageBoxA)(NULL, xorstr_("Cannot connect to server."), utilities::get_random_string(16).c_str(), MB_SYSTEMMODAL | MB_ICONERROR);
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

			HINTERNET hConnect = li(InternetConnectW)(hInternet, site_name, 80, NULL, NULL, INTERNET_SERVICE_HTTP, 0, NULL);

			if (hConnect == NULL)
			{
				li(MessageBoxA)(NULL, xorstr_("Error sending message to server"), utilities::get_random_string(16).c_str(), MB_SYSTEMMODAL | MB_ICONERROR);
				return NULL;
			}
			else
			{
				const wchar_t* parrAcceptTypes[] = { xorstr_(L"text/*"), NULL };

				HINTERNET hRequest = li(HttpOpenRequestW)(hConnect, xorstr_(L"POST"), site_param, NULL, NULL, parrAcceptTypes, 0, 0);

				if (hRequest == NULL)
				{
					li(MessageBoxA)(NULL, xorstr_("Error sending message to server"), utilities::get_random_string(16).c_str(), MB_SYSTEMMODAL | MB_ICONERROR);
					return NULL;
				}
				else
				{
					BOOL bRequestSent = li(HttpSendRequestW)(hRequest, NULL, 0, NULL, 0);

					if (!bRequestSent)
					{
						li(MessageBoxA)(NULL, xorstr_("Error sending message to server"), utilities::get_random_string(16).c_str(), MB_SYSTEMMODAL | MB_ICONERROR);
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
							bKeepReading = li(InternetReadFile)(hRequest, buff, nBuffSize, &dwBytesRead);
							strResponse.append(buff, dwBytesRead);
						}
						return strResponse;
					}
					li(InternetCloseHandle)(hRequest);
				}
				li(InternetCloseHandle)(hConnect);
			}
			li(InternetCloseHandle)(hInternet);
		}
	}
	__forceinline std::string get_hwid()
	{
		std::string result = xorstr_("");

		HANDLE hDevice = li(CreateFileA)(xorstr_("\\\\.\\PhysicalDrive0"), (DWORD)nullptr, FILE_SHARE_READ | FILE_SHARE_WRITE, (LPSECURITY_ATTRIBUTES)nullptr, OPEN_EXISTING, (DWORD)nullptr, (HANDLE)nullptr);

		if (hDevice == INVALID_HANDLE_VALUE) return result;

		STORAGE_PROPERTY_QUERY storagePropertyQuery;
		ZeroMemory(&storagePropertyQuery, sizeof(STORAGE_PROPERTY_QUERY));
		storagePropertyQuery.PropertyId = StorageDeviceProperty;
		storagePropertyQuery.QueryType = PropertyStandardQuery;

		STORAGE_DESCRIPTOR_HEADER storageDescriptorHeader = { 0 };
		DWORD dwBytesReturned = 0;

		li(DeviceIoControl)
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

		li(DeviceIoControl)
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
		li(CloseHandle)(hDevice);

		result = md5::create_from_string(result);

		return result;
	}

	__forceinline std::string MAC()
	{
		PIP_ADAPTER_INFO AdapterInfo;
		DWORD dwBufLen = sizeof(IP_ADAPTER_INFO);
		char* mac_addr = (char*)malloc(18);

		AdapterInfo = (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));
		if (AdapterInfo == NULL) 
		{
			printf("Error allocating memory needed to call GetAdaptersinfo\n");
			free(mac_addr);
			return NULL; // it is safe to call free(NULL)
		}

		// Make an initial call to GetAdaptersInfo to get the necessary size into the dwBufLen variable
		if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW) 
		{
			free(AdapterInfo);
			AdapterInfo = (IP_ADAPTER_INFO*)malloc(dwBufLen);
			if (AdapterInfo == NULL) {
				printf("Error allocating memory needed to call GetAdaptersinfo\n");
				free(mac_addr);
				return NULL;
			}
		}

		if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR) 
		{
			// Contains pointer to current adapter info
			PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
			do {
				// technically should look at pAdapterInfo->AddressLength
				//   and not assume it is 6.
				sprintf(mac_addr, "%02X:%02X:%02X:%02X:%02X:%02X",
					pAdapterInfo->Address[0], pAdapterInfo->Address[1],
					pAdapterInfo->Address[2], pAdapterInfo->Address[3],
					pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
				printf("Address: %s, mac: %s\n", pAdapterInfo->IpAddressList.IpAddress.String, mac_addr);
				// print them all, return the last one.
				// return mac_addr;

				printf("\n");
				pAdapterInfo = pAdapterInfo->Next;
			} while (pAdapterInfo);
		}
		free(AdapterInfo);
		return md5::create_from_string(mac_addr); // caller must free.
	}
}
