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
			packet_data = nlohmann::json::parse(json.c_str());

			packet_error_bool = false;
		}
		catch (std::exception e)
		{
			packet_error_bool = true;
			packet_error_message = std::string("Message must be a parsable JSON feed, returning error JSON. (Exception: " + std::string(e.what()) + ")");

			std::cout << "Invalid packet sent, unable to parse JSON feed." << std::endl;
		}
	}
	else
	{
		std::cout << "Client sent an empty feed." << std::endl;
	}
}

std::string PacketData::GetType(std::string key)
{
	return packet_data[key]["type"];
}

std::string PacketData::GetValue(std::string key)
{
	return packet_data[key]["value"];
}

bool PacketData::CheckError()
{
	return packet_error_bool;
}
std::string PacketData::GetErrorMessage()
{
	return packet_error_message;
}
