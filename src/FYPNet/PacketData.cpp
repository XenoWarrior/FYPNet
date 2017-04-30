#include "PacketData.h"

PacketData::PacketData(std::string json)
{
	std::cout << "[DEBUG] " << json << std::endl;
	packet_data = nlohmann::json(json);
}

std::string PacketData::GetType(std::string key)
{
	return packet_data[key]["type"];
}

std::string PacketData::GetValue(std::string key)
{
	return packet_data[key]["value"];
}