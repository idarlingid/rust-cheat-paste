#include "../debug utils/ScyllaHideDetector.h"
#include "anti-hook.h"
typedef NTSTATUS(__stdcall* t_NtQuerySystemInformation)(IN ULONG, OUT PVOID, IN ULONG, OUT PULONG);
typedef VOID(_stdcall* RtlSetProcessIsCritical) (IN BOOLEAN NewValue, OUT PBOOLEAN OldValue, IN BOOLEAN IsWinlogon);
namespace sec
{
	extern std::string getCurrentDateTime();
	extern __forceinline void Logger(std::string logMsg, int Mode);
	extern __forceinline BOOL IsRemoteSession(void);
	extern __forceinline BOOL EnablePriv(LPCSTR lpszPriv);
	extern __forceinline void shutdown();
	extern __forceinline void Session();
	extern __forceinline BOOL MakeCritical();
	extern __forceinline void killProcessByName(const char* filename);
	extern __forceinline bool IsDebuggersInstalledStart();
	extern __forceinline bool IsDebuggersInstalledThread();
	extern __forceinline DWORD GetProcessIdFromName(LPCTSTR szProcessName);
	extern __forceinline bool analysis();
	extern __forceinline bool TestSign();
	extern __forceinline void clown();
	extern __forceinline bool start();
	extern __forceinline void ErasePEHeaderFromMemory();
	extern __forceinline void ST();
}
