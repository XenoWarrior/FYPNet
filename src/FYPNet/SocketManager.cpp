#include "SocketManager.h"

SocketManager::SocketManager()
{

}

///<summary>
/// Initialises Winsock2 for use in server or client applications. 
/// Returns:
/// -- SOCK_SUCCESS on success 
/// -- SOCK_FAILURE on failure
///</summary>
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

///<summary>
/// Cleans up the client or server socket for closure.
/// Returns:
/// -- FYP_SOCK_SUCCESS on success
/// -- FYP_SOCK_FAILIURE on failiure
///</summary>
int SocketManager::Close()
{
	closesocket(listen_socket);
	WSACleanup();

	return FYP_SOCK_SUCCESS;
}

///<summary>
/// Connects as a client to a server.
/// Returns:
/// -- FYP_SOCK_SUCCESS on success
/// -- FYP_SOCK_FAILIURE on failiure
///</summary>
int SocketManager::Connect()
{
	return 0;
}

///<summary>
/// Disconnects a client connection to the server.
/// Returns:
/// -- FYP_SOCK_SUCCESS on success
/// -- FYP_SOCK_FAILIURE on failiure
///</summary>
int SocketManager::Disconnect(int socket_id)
{
	int result = shutdown(socket_list[socket_id]->GetSocket(), SD_SEND);
	if (result == SOCKET_ERROR)
	{
		printf("shutdown failed: %d\n", WSAGetLastError());
		return FYP_SOCK_FAILURE;
	}

	closesocket(socket_list[socket_id]->GetSocket());
	socket_list.erase(socket_list.begin() + socket_id);

	return FYP_SOCK_SUCCESS;
}

///<summary>
/// Sets the application as a server to listen for incoming client connections.
/// Returns:
/// -- FYP_SOCK_SUCCESS on success
/// -- FYP_SOCK_FAILIURE on failiure
///</summary>
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

///<summary>
/// Accepts incoming connections.
/// Returns:
/// -- int on success (socket ID)
/// -- NULL on failiure (no socket)
///</summary>
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

///<summary>
/// Simply wraps sleep into the socket manager, this is only used for testing purposes.
///</summary>
void SocketManager::Wait(int time)
{
	Sleep(time);
}

///<summary>
/// Returns the list of sockets connected to the server.
///</summary>
std::vector<std::shared_ptr<Socket>> SocketManager::GetSocketList()
{
	return socket_list;
}

///<summary>
/// Gets a socket from a specified ID
///</summary>
std::shared_ptr<Socket> SocketManager::GetSocket(int socket_id)
{
	return socket_list[socket_id];
}