#ifndef Buffer_H
#define Buffer_H

#include <string>

#include "json.hpp"

class Buffer{
public:
	Buffer();

	void ClearBuffer();

	void AddValue(std::string key, std::string value);
	void AddValue(std::string key, int value);

	std::string GetFeed();

private:
	nlohmann::json buffer_data;

};

#endif // !Buffer_H
