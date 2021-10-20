#pragma once
#include <windows.h>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/base64.h>
#include "..\utilities\lazy_importer.hpp"
#pragma comment(lib, "cryptlib.lib")

namespace aes 
{
	extern __forceinline std::string encrypt(const std::string& str, const std::string& cipher_key, const std::string& iv_key);
	extern __forceinline std::string decrypt(const std::string& str, const std::string& cipher_key, const std::string& iv_key);
}

