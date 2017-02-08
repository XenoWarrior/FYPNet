#include "Socket.h"

Socket::Socket(SOCKET s)
{
	socket = s;

	AddBuffer();
}

int Socket::ReceiveMessage()
{
	return recv(socket, message_in, 8195, 0);
}

std::string Socket::ReadMessage(int size)
{
	std::string message;

	for(int i = 0; i < size-1; i++)
	{
		message += message_in[i];
	}

	return message;
}

void Socket::Dispatch(int buffer_id)
{
	send(socket, buffer_list[buffer_id]->GetFeed().c_str(), strlen(buffer_list[buffer_id]->GetFeed().c_str()), 0);
}

int Socket::AddBuffer()
{
	buffer_list.push_back(std::make_shared<Buffer>());
	return buffer_list.size() - 1;
}

std::shared_ptr<Buffer> Socket::GetBuffer(int buffer_id)
{
	return buffer_list[buffer_id];
}

SOCKET Socket::GetSocket()
{
	return socket;
}