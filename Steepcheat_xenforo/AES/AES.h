#include <windows.h>
#include "Libs\CryptoPP/aes.h"
#include "Libs\CryptoPP/modes.h"
#include "Libs\CryptoPP/base64.h"
//#include "..\utilities\lazy_importer.hpp"
#pragma comment(lib, "cryptlib.lib")

namespace aes
{
	__forceinline std::string encrypt(const std::string& str, const std::string& cipher_key, const std::string& iv_key)
	{
		std::string str_out;

		CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption encryption((BYTE*)cipher_key.c_str(), cipher_key.length(), (BYTE*)iv_key.c_str());

		CryptoPP::StringSource encryptor(str, true,
			new CryptoPP::StreamTransformationFilter(encryption,
				new CryptoPP::Base64Encoder(
					new CryptoPP::StringSink(str_out),
					false
				)
			)
		);
		return str_out;
	}

	__forceinline std::string decrypt(const std::string& str, const std::string& cipher_key, const std::string& iv_key)
	{
		std::string str_out;

		CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption decryption((BYTE*)cipher_key.c_str(), cipher_key.length(), (BYTE*)iv_key.c_str());

		CryptoPP::StringSource decryptor(str, true,
			new CryptoPP::Base64Decoder(
				new CryptoPP::StreamTransformationFilter(decryption,
					new CryptoPP::StringSink(str_out)
				)
			)
		);
		return str_out;
	}
}