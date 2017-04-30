#include "Buffer.h"

#include "Shared.h"

/**
* Constructor
*/
Buffer::Buffer()
{
}

/**
* Clears all buffer data.
*/
void Buffer::ClearBuffer()
{

}

/**
* Adds a string value to the buffer.
*/
void Buffer::AddValue(std::string key, std::string value)
{
	buffer_data[key]["type"] = "string";
	buffer_data[key]["value"] = value;
}

/**
* Adds an integer value to the buffer.
*/
void Buffer::AddValue(std::string key, int value)
{
	buffer_data[key]["type"] = "int";
	buffer_data[key]["value"] = value;
}

/**
* Adds a bool value to the buffer.
*/
void Buffer::AddValue(std::string key, bool value)
{
	buffer_data[key]["type"] = "bool";
	buffer_data[key]["value"] = value;
}

/**
* Adds a packet identifier value to the buffer.
*/
void Buffer::AddValue(std::string key, FYPPacketType value)
{
	buffer_data[key]["type"] = "FYPPacketType";
	buffer_data[key]["value"] = value;
}

/**
* Returns the JSON feed for the buffer, used for Dispatch.
*/
std::string Buffer::GetFeed()
{
	return buffer_data.dump();
}