#ifndef PacketData_H
#define PacketData_H

#include <map>

#include "json.hpp"

class PacketData {
public:
	PacketData(std::string json);

	std::string GetType(std::string key);
	std::string GetValue(std::string key);

	std::string GetErrorMessage();
	bool CheckError();

private:
	nlohmann::json packet_data;

	bool packet_error_bool;
	std::string packet_error_message;
};

#endif