#pragma once

using fn_nt_close_type = NTSTATUS(*)(HANDLE);
using fn_query_information_process_type = NTSTATUS(*)(HANDLE, UINT, PVOID, ULONG, PULONG);
using fn_query_object_type = NTSTATUS(*)(HANDLE, UINT, PVOID, ULONG, PULONG);
using fn_query_system_information_type = NTSTATUS(*)(ULONG, PVOID, ULONG, PULONG);

extern "C" NTSTATUS NtFlushInstructionCache(HANDLE, PVOID, SIZE_T);
extern "C" NTSTATUS NtOpenThread(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, CLIENT_ID*);
extern "C" NTSTATUS NtSuspendThread(HANDLE, PULONG);
extern "C" NTSTATUS NtResumeThread(HANDLE, PULONG);
extern "C" NTSTATUS NtAllocateVirtualMemory(HANDLE, PVOID*, ULONG, PSIZE_T, ULONG, ULONG);
extern "C" NTSTATUS NtFreeVirtualMemory(HANDLE, PVOID*, PSIZE_T, ULONG);

typedef enum err_code {

	err_success,
	err_enum_process_modules_failed,
	err_size_too_small,
	err_mod_name_not_found,
	err_mod_query_failed,
	err_create_file_failed,
	err_create_file_mapping_failed,
	err_create_file_mapping_already_exists,
	err_map_file_failed,
	err_mem_deprotect_failed,
	err_mem_reprotect_failed,
	err_text_section_not_found,
	err_file_path_query_failed

} err_code;

typedef enum suspend_resume_type {

	srt_suspend,
	srt_resume

} suspend_resume_type, * psuspend_resume_type;

typedef struct suspend_resume_info {

	ULONG current_pid;
	ULONG current_tid;
	suspend_resume_type type;

} suspend_resume_info, * psuspend_resume_info;

typedef struct wrk_system_process_information {

	ULONG next_entry_offset;

	ULONG number_of_threads;

	LARGE_INTEGER spare_li1;

	LARGE_INTEGER spare_li2;

	LARGE_INTEGER spare_li3;

	LARGE_INTEGER create_time;

	LARGE_INTEGER user_time;

	LARGE_INTEGER kernel_time;

	UNICODE_STRING image_name;

	KPRIORITY base_priority;

	HANDLE unique_process_id;

	HANDLE inherited_from_unique_process_id;

	ULONG handle_count;

	ULONG session_id;

	ULONG_PTR page_directory_base;

	SIZE_T peak_virtual_size;

	SIZE_T virtual_size;

	ULONG page_fault_count;

	SIZE_T peak_working_set_size;

	SIZE_T working_set_size;

	SIZE_T quota_peak_paged_pool_usage;

	SIZE_T quota_paged_pool_usage;

	SIZE_T quota_peak_non_paged_pool_usage;

	SIZE_T quota_non_paged_pool_usage;

	SIZE_T pagefile_usage;

	SIZE_T peak_pagefile_usage;

	SIZE_T private_page_count;

	LARGE_INTEGER read_operation_count;

	LARGE_INTEGER write_operation_count;

	LARGE_INTEGER other_operation_count;
	LARGE_INTEGER read_transfer_count;

	LARGE_INTEGER write_transfer_count;

	LARGE_INTEGER other_transfer_count;

	SYSTEM_THREAD_INFORMATION threads[1];

} wrk_system_process_information, * pwrk_system_process_information;

typedef enum wrk_memory_information_class {

	memory_basic_information

} wrk_memory_information_class, * pwrk_memory_information_class;

typedef struct object_type_information {

	UNICODE_STRING type_name;
	ULONG total_number_of_handles;
	ULONG total_number_of_objects;

} object_type_information, * pobject_type_information;

typedef struct object_all_information {

	ULONG number_of_objects;
	object_type_information object_type_information[1];

} object_all_information, * pobject_all_information;

static void* teb() {

	return reinterpret_cast<void*>(__readgsqword(0x30));

}

static unsigned int pid() {

	return *reinterpret_cast<unsigned int*>(static_cast<unsigned char*>(teb()) + 0x40);

}

static unsigned int tid() {

	return *reinterpret_cast<unsigned int*>(static_cast<unsigned char*>(teb()) + 0x48);

}

static void* alloc(void* base, size_t size, const DWORD protect) {

	const NTSTATUS status = NtAllocateVirtualMemory(reinterpret_cast<HANDLE>(-1), &base, base ? 12 : 0, &size, MEM_RESERVE | MEM_COMMIT, protect);

	return NT_SUCCESS(status) ? base : nullptr;

}

static void ah_free(void* base) {

	size_t region_size;

	NtFreeVirtualMemory(reinterpret_cast<HANDLE>(-1), &base, &region_size, MEM_RELEASE);

}

static bool __stdcall enum_processes(bool (*callback)(pwrk_system_process_information process, void* argument), void* arg) {

	ULONG length;

	NTSTATUS status = NtQuerySystemInformation(SystemProcessInformation, nullptr, 0, &length);

	if (status != 0xC0000004L)
		return false;

	pwrk_system_process_information info = static_cast<pwrk_system_process_information>(alloc(nullptr, length, PAGE_READWRITE));

	if (!info)
		return false;

	status = NtQuerySystemInformation(SystemProcessInformation, info, length, &length);

	if (!NT_SUCCESS(status)) {

		ah_free(info);
		return false;

	}
	do {

		if (!callback(info, arg)) {
			break;

		}

		info = reinterpret_cast<pwrk_system_process_information>(reinterpret_cast<PBYTE>(info) + info->next_entry_offset

			);
	} while (info->next_entry_offset);

	ah_free(info);

	return true;

}

static bool suspend_resume_callback(pwrk_system_process_information process, void* argument) {

	if (!process || !argument)
		return false;

	const psuspend_resume_info info = static_cast<psuspend_resume_info>(argument);

	if (reinterpret_cast<size_t>(process->unique_process_id) != static_cast<size_t>(info->current_pid))
		return true;

	for (unsigned int i = 0; i < process->number_of_threads; ++i) {

		if (reinterpret_cast<size_t>(process->threads[i].ClientId.UniqueThread) == static_cast<size_t>(info->current_tid))
			continue;

		HANDLE h_thread = nullptr;

		const NTSTATUS status = NtOpenThread(&h_thread, THREAD_SUSPEND_RESUME, nullptr, &process->threads[i].ClientId);

		if (NT_SUCCESS(status) && h_thread) {

			ULONG suspend_count = 0;

			switch (info->type) {

			case srt_suspend:
				NtSuspendThread(h_thread, &suspend_count);
				break;

			case srt_resume:
				NtResumeThread(h_thread, &suspend_count);
				break;
			}

			NtClose(h_thread);

		}

	}

	return false;

}

static bool suspend_threads() {

	suspend_resume_info info;
	info.current_pid = pid();
	info.current_tid = tid();
	info.type = srt_suspend;

	return enum_processes(suspend_resume_callback, &info);

}

static bool resume_threads() {

	suspend_resume_info info;
	info.current_pid = pid();
	info.current_tid = tid();
	info.type = srt_resume;

	return enum_processes(suspend_resume_callback, &info);

}

static DWORD get_module_name(const HMODULE module, LPSTR module_name, const DWORD size) {

	const DWORD length = GetModuleFileNameExA(GetCurrentProcess(), module, module_name, size);

	if (!length) {

		strncpy(module_name, "not found", size - 1);
		return err_mod_name_not_found;

	}

	return err_success;
}

static DWORD protect_memory(void* address, const size_t size, const DWORD new_protect) {

	DWORD old_protect = 0;

	const bool b_ret = VirtualProtect(address, size, new_protect, &old_protect);

	if (!b_ret)
		return 0;

	return old_protect;

}

static DWORD replace_exec_section(const HMODULE module, void* mapping) {

	const PIMAGE_DOS_HEADER image_dos_header = static_cast<PIMAGE_DOS_HEADER>(mapping);

	const PIMAGE_NT_HEADERS image_nt_headers = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<DWORD_PTR>(mapping) + image_dos_header->e_lfanew);

	for (WORD i = 0; i < image_nt_headers->FileHeader.NumberOfSections; ++i) {

		const PIMAGE_SECTION_HEADER image_section_header = reinterpret_cast<PIMAGE_SECTION_HEADER>(reinterpret_cast<DWORD_PTR>(IMAGE_FIRST_SECTION(image_nt_headers)) + static_cast<DWORD_PTR>(IMAGE_SIZEOF_SECTION_HEADER) * i);

		if (!strcmp(reinterpret_cast<const char*>(image_section_header->Name), ".text")) {

			DWORD protect = protect_memory(reinterpret_cast<void*>(reinterpret_cast<DWORD_PTR>(module) + static_cast<DWORD_PTR>(image_section_header->VirtualAddress)), image_section_header->Misc.VirtualSize, PAGE_EXECUTE_READWRITE);

			if (!protect)
				return err_mem_deprotect_failed;

			memcpy(reinterpret_cast<void*>(reinterpret_cast<DWORD_PTR>(module) + static_cast<DWORD_PTR>(image_section_header->VirtualAddress)), reinterpret_cast<void*>(reinterpret_cast<DWORD_PTR>(mapping) + static_cast<DWORD_PTR>(image_section_header->VirtualAddress)), image_section_header->Misc.VirtualSize);

			protect = protect_memory(reinterpret_cast<void*>(reinterpret_cast<DWORD_PTR>(module) + static_cast<DWORD_PTR>(image_section_header->VirtualAddress)), image_section_header->Misc.VirtualSize, protect);

			if (!protect)
				return err_mem_reprotect_failed;

			return err_success;

		}

	}

	return err_text_section_not_found;

}

static DWORD unhook_module(const HMODULE module) {

	char module_name[MAX_PATH];

	SecureZeroMemory(module_name, sizeof(module_name));

	DWORD ret = get_module_name(module, module_name, sizeof(module_name));

	if (ret == err_mod_name_not_found)
		return ret;

	const HANDLE file = CreateFileA(module_name, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);

	if (file == INVALID_HANDLE_VALUE)
		return err_create_file_failed;

	const HANDLE file_mapping = CreateFileMapping(file, nullptr, PAGE_READONLY | SEC_IMAGE, 0, 0, nullptr);

	if (!file_mapping) {

		CloseHandle(file);
		return err_create_file_mapping_failed;

	}

	if (GetLastError() == ERROR_ALREADY_EXISTS) {

		CloseHandle(file);
		return err_create_file_mapping_already_exists;

	}

	const LPVOID mapping = MapViewOfFile(file_mapping, FILE_MAP_READ, 0, 0, 0);

	if (!mapping) {

		CloseHandle(file_mapping);
		CloseHandle(file);
		return err_map_file_failed;

	}

	suspend_threads();

	ret = replace_exec_section(module, mapping);

	NtFlushInstructionCache(reinterpret_cast<HANDLE>(-1), nullptr, 0);

	resume_threads();

	if (ret) {

		UnmapViewOfFile(mapping);
		CloseHandle(file_mapping);
		CloseHandle(file);
		return ret;

	}

	UnmapViewOfFile(mapping);
	CloseHandle(file_mapping);
	CloseHandle(file);

	return err_success;

}


static HMODULE add_module(const char* lib_name) {

	HMODULE module = GetModuleHandleA(lib_name);

	if (!module)
		module = LoadLibraryA(lib_name);

	return module;

}

static void unhook(std::string lib_name) 
{
	const HMODULE module = add_module(lib_name.c_str());

	const DWORD h_mod = unhook_module(module);

	FreeLibrary(module);
}
