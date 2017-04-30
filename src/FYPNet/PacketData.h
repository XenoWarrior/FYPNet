#include <map>

#include "json.hpp"

class PacketData {
public:
	PacketData(std::string json);

	std::string GetType(std::string key);
	std::string GetValue(std::string key);

private:
	nlohmann::json packet_data;
};