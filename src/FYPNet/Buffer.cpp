#include "Buffer.h"

#include <iostream>

Buffer::Buffer()
{
}

void Buffer::ClearBuffer()
{

}

void Buffer::AddValue(std::string key, std::string value)
{
	buffer_data[key]["type"] = "string";
	buffer_data[key]["value"] = value;
}

void Buffer::AddValue(std::string key, int value)
{
	buffer_data[key]["type"] = "int";
	buffer_data[key]["value"] = value;
}

std::string Buffer::GetFeed()
{
	return buffer_data.dump();
}