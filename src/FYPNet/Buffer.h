#ifndef Buffer_H
#define Buffer_H

#include <string>
#include <iostream>

#include "FYPPacketType.h"

#include "json.hpp"

class Buffer{
public:
	Buffer();

	void ClearBuffer();

	void AddValue(std::string key, std::string value);
	void AddValue(std::string key, int value);
	void AddValue(std::string key, bool value);
	void AddValue(std::string key, FYPPacketType value);

	std::string GetFeed();

private:
	nlohmann::json buffer_data;

};

#endif // !Buffer_H
