/**
* PacketData.cpp
*
* Author: Ashley Scott
* Date: 2016 - 2017
*
*/

#include "PacketData.h"

PacketData::PacketData(std::string json)
{
	if(!json.empty())
	{
		try
		{
			std::cout << "[Parse]: " << json << std::endl;

			// For GameServer logic
			// Due to the bugs with the networking library used in GameMaker: Studio
			//
			// This block of code will clean up the start of any data received as it 
			// has a tendancy to add random characters at the start or end of the string.

			if(json[0] != '{')
			{
				std::cout << "[Parse] Detected issue with string start, attempting repair." << std::endl;

				while (json[0] != '{')
				{
					if (json.length() < 1)
					{
						break;
					}

					std::cout << "[Parse] >> Removing unknown start character: " << json[0] << std::endl;
					json.erase(json.begin());
				}
			}

			// It is possible for the string to become empty when checking if it is actually valid in the first place
			if (!json.empty())
			{
				int string_end = json.length() - 1;
				if (json[string_end] != '}')
				{
					std::cout << "[Parse] Detected issue with string end, attempting repair." << std::endl;

					while (json[string_end] != '}')
					{
						string_end = json.length() - 1;

						if (json.length() < 1)
						{
							break;
						}

						std::cout << "[Parse] >> Removing unknown end character: " << json[string_end] << std::endl;
						json.erase(json.begin()+string_end);
					}
				}
			}
			else
			{
				std::cout << "[Parse] Unable to parse as string has become empty (was invalid from the start)" << std::endl;
			}

			std::cout << "[Parse]: " << json << std::endl;

			packet_data = nlohmann::json::parse(json.c_str());

			packet_error_bool = false;
		}
		catch (std::exception e)
		{
			packet_error_bool = true;
			packet_error_message = std::string("Message must be a parsable JSON feed, returning error JSON. (Exception: " + std::string(e.what()) + ")");

			std::cout << "[Parse] Invalid packet sent, unable to parse JSON feed." << std::endl;
		}
	}
	else
	{
		std::cout << "[Parse] Client sent an empty feed." << std::endl;
	}
}

std::string PacketData::GetType(std::string key)
{
	try
	{
		return packet_data[key]["type"];
	}
	catch (std::exception e)
	{
		throw e;
	}
}

std::string PacketData::GetValue(std::string key)
{
	try
	{
		return packet_data[key]["value"];
	}
	catch (std::exception e)
	{
		throw e;
	}
}

bool PacketData::CheckError()
{
	return packet_error_bool;
}
std::string PacketData::GetErrorMessage()
{
	return packet_error_message;
}
