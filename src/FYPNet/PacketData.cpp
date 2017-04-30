#include "PacketData.h"

PacketData::PacketData(std::string json)
{
	if(!json.empty())
	{
		try
		{
			std::cout << "[DEBUG] " << json << std::endl;
			packet_data = nlohmann::json::parse(json);

			packet_error_bool = false;
		}
		catch (std::exception e)
		{
			packet_error_bool = true;
			packet_error_message = "Message must be a parsable JSON feed, returning error JSON.";

			std::cout << "Invalid packet sent, unable to parse JSON feed." << std::endl;
		}
	}
	else
	{
		std::cout << "[DEBUG] Client sent an empty feed." << std::endl;
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