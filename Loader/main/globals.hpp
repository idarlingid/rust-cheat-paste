#pragma once
#include "..\main\utilities\xorstr.hpp"
struct globals
{
	struct
	{	
		std::string version;
		std::string status;

		std::string ip = xorstr_("steepcheat.pw");

		struct
		{
			std::string cipher;
			std::string iv;
		} key;
	} server_side;

	struct
	{
		std::string version = xorstr_("1.2");

		struct
		{
			std::string name = utilities::get_random_string(16);
			float width = 600.f;
			float height = 350.f;
		} window_settings;		

		struct
		{
			std::string key;
			std::string hwid = utilities::get_hwid();

			std::string token;
			std::string structure_cheat;

		} data;
	} client_side;
};

extern globals g_globals;