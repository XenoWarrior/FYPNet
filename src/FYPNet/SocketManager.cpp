/**
* SocketManager.cpp
*
* Author: Ashley Scott
* Date: 2016 - 2017
*
*/

#include "SocketManager.h"

SocketManager::SocketManager()
{

}

/**
* Initialises Winsock2 for use in server or client applications. 
* Returns:
* -- SOCK_SUCCESS on success 
* -- SOCK_FAILURE on failure
*/
int SocketManager::Initialise()
{
	int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (result != 0)
	{
		printf("WSAStartup failed: %d\n", result);
		return FYP_SOCK_FAILURE;
	}

	return FYP_SOCK_SUCCESS;
}

/**
* Cleans up the client or server socket for closure.
* Returns:
* -- FYP_SOCK_SUCCESS on success
* -- FYP_SOCK_FAILIURE on failiure
*/
int SocketManager::Close()
{
	closesocket(listen_socket);
	WSACleanup();

	return FYP_SOCK_SUCCESS;
}

/**
* Connects as a client to a server.
* Returns:
* -- FYP_SOCK_SUCCESS on success
* -- FYP_SOCK_FAILIURE on failiure
*/
int SocketManager::Connect(std::string ip, int port)
{
	client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client_socket == INVALID_SOCKET)
	{
		printf("create socket failed: %d\n", WSAGetLastError());
		WSACleanup();
		return FYP_SOCK_FAILURE;
	}

	sockaddr_in socket_service;
	socket_service.sin_family = AF_INET;
	socket_service.sin_addr.s_addr = inet_addr("127.0.0.1");
	socket_service.sin_port = htons(12500);

	if (connect(client_socket, (SOCKADDR *)& socket_service, sizeof(socket_service)) == SOCKET_ERROR)
	{
		printf("connect function failed with error: %ld\n", WSAGetLastError());
		closesocket(client_socket);
		WSACleanup();
		return FYP_SOCK_FAILURE;
	}

	main_client_socket = std::make_shared<Socket>(client_socket);

	is_client = true;

	return FYP_SOCK_SUCCESS;
}

/**
* Disconnects a client connection to the server.
* Returns:
* -- FYP_SOCK_SUCCESS on success
* -- FYP_SOCK_FAILIURE on failiure
*/
int SocketManager::Disconnect(int socket_id)
{
	int result = shutdown(socket_list[socket_id]->GetSocket(), SD_SEND);
	if (result == SOCKET_ERROR)
	{
		printf("shutdown failed: %d\n", WSAGetLastError());
		return FYP_SOCK_FAILURE;
	}

	result = closesocket(socket_list[socket_id]->GetSocket());
	if (result == SOCKET_ERROR)
	{
		printf("close failed: %d\n", WSAGetLastError());
		return FYP_SOCK_FAILURE;
	}

	socket_list.erase(socket_list.begin() + socket_id);

	return FYP_SOCK_SUCCESS;
}

/**
* Sets the application as a server to listen for incoming client connections.
* Returns:
* -- FYP_SOCK_SUCCESS on success
* -- FYP_SOCK_FAILIURE on failiure
*/
int SocketManager::Listen(int port, int cons, bool mode)
{
	listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listen_socket == INVALID_SOCKET)
	{
		return FYP_SOCK_FAILURE;
	}

	SOCKADDR_IN address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	if (mode)
	{
		u_long socket_mode = mode;
		ioctlsocket(listen_socket, FIONBIO, &socket_mode);
	}

	if (bind(listen_socket, (LPSOCKADDR)&address, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		closesocket(listen_socket);
		return FYP_SOCK_FAILURE;
	}

	if (listen(listen_socket, cons) == SOCKET_ERROR)
	{
		closesocket(listen_socket);
		return FYP_SOCK_FAILURE;
	}

	return FYP_SOCK_SUCCESS;
}

/**
* Accepts incoming connections.
* Returns:
* -- int on success (socket ID)
* -- NULL on failiure (no socket)
*/
int SocketManager::Accept(bool mode)
{
	int socket_size = sizeof(SOCKADDR_IN);

	SOCKADDR_IN socket_address;
	SOCKET socket_in = accept(listen_socket, (SOCKADDR *)&socket_address, &socket_size);

	if (socket_in == INVALID_SOCKET)
	{
		closesocket(socket_in);
		return -1;
	}

	socket_list.push_back(std::make_shared<Socket>(socket_in));

	return socket_list.size() - 1;
}

/**
* Simply wraps sleep into the socket manager, this is only used for testing purposes.
*/
void SocketManager::Wait(int time)
{
	Sleep(time);
}

/**
* Returns the list of sockets connected to the server.
*/
std::vector<std::shared_ptr<Socket>> SocketManager::GetSocketList()
{
	return socket_list;
}

/**
* Gets a socket from a specified ID
*/
std::shared_ptr<Socket> SocketManager::GetSocket(int socket_id)
{
	return socket_list[socket_id];
}


/**
* Returns the socket, used if client
*/
std::shared_ptr<Socket> SocketManager::ClientSocket()
{
	if (is_client)
	{
		return main_client_socket;
	}
	else
	{
		throw new std::exception("ClientSocket() cannot be called when there is no outgoing connections.");
	}
}

/**
* Returns the socket, used if client
*/
void SocketManager::ClientDisconnect()
{
	shutdown(main_client_socket->GetSocket(), SD_SEND);
	closesocket(main_client_socket->GetSocket());
}

/**
* Disconnects all clients from the server
*/
void SocketManager::DisconnectAll()
{
	for (int i = 0; i < socket_list.size(); i++)
	{
		Disconnect(i);
	}
}