#pragma once
#include <Windows.h>
#include <string>

namespace client
{
	extern __forceinline std::string authentication();
	extern __forceinline void RenameAndDestroy();
	extern __forceinline void Rename();
	extern __forceinline void Update();
	extern __forceinline std::string valid_version();
	extern __forceinline std::string activation();
	extern __forceinline std::string banuser();
	extern __forceinline void BanThread();
}
