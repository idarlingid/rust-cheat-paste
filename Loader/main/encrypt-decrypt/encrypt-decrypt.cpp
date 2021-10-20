#include "encrypt-decrypt.hpp"
#include "..\utilities\lazy_importer.hpp"


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
