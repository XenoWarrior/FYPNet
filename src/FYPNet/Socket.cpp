/**
* Socket.cpp
*
* Author: Ashley Scott
* Date: 2016 - 2017
*
*/

#include "Socket.h"

/**
* Constructor
*/
Socket::Socket(SOCKET s)
{
	socket = s;

	AddBuffer();
}

/**
* Receives a message and returns its message size.
*/
int Socket::ReceiveMessage()
{
	return recv(socket, message_in, 8195, MSG_PEEK);
}

/**
* Reads the message from the client and returns its whole value.
*/
std::string Socket::ReadMessage(int size)
{
	std::string message;

	recv(socket, message_in, size, 0);

	for(int i = 0; i < size; i++)
	{
		message += message_in[i];
	}

	return message;
}

/**
 * Sends the buffer data to this specific socket.
*/
void Socket::Dispatch(int buffer_id)
{
	send(socket, buffer_list[buffer_id]->GetFeed().c_str(), strlen(buffer_list[buffer_id]->GetFeed().c_str()), 0);
}

/**
* Adds a new buffer to the socket.
* Ideal for caching responses.
*/
int Socket::AddBuffer()
{
	buffer_list.push_back(std::make_shared<Buffer>());
	return buffer_list.size() - 1;
}

/**
* Gets a buffer and its data from the socket.
*/
std::shared_ptr<Buffer> Socket::GetBuffer(int buffer_id)
{
	return buffer_list[buffer_id];
}

/**
* Gets the specific socket that refers to the client.
*/
SOCKET Socket::GetSocket()
{
	return socket;
}
