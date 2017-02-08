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
	buffer_data.push_back(value);
}

void Buffer::AddValue(std::string key, int value)
{
}

std::string Buffer::GetFeed()
{
	return "";
}