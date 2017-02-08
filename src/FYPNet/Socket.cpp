#include "Socket.h"

Socket::Socket(SOCKET s)
{
	socket = s;

	AddBuffer();
}

void Socket::Dispatch(int buffer_id)
{
	send(socket, buffer_list[buffer_id]->GetFeed().c_str(), strlen(buffer_list[buffer_id]->GetFeed().c_str()), 0);
}

int Socket::AddBuffer()
{
	buffer_list.push_back(std::shared_ptr<Buffer>());
	return buffer_list.size() - 1;
}

std::shared_ptr<Buffer> Socket::GetBuffer(int buffer_id)
{
	return buffer_list[buffer_id];
}