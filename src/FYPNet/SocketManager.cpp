#include "SocketManager.h"

SocketManager::SocketManager()
{

}

SocketManager::~SocketManager()
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

	socket_list.push_back(SOCKET());

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
/// Simply wraps sleep into the socket manager, this is only used for testing purposes.
///</summary>
void SocketManager::Wait(int time)
{
	Sleep(time);
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
/// -- SOCKET on success
/// -- NULL on failiure
///</summary>
SOCKET SocketManager::Accept(int mode)
{
	int socket_size = sizeof(SOCKADDR_IN);
	SOCKADDR_IN socket_address;
	SOCKET socket_in = accept(listen_socket, (SOCKADDR *)&socket_address, &socket_size);

	if (socket_in == INVALID_SOCKET)
	{
		closesocket(socket_in);
		return NULL;
	}

	socket_list.push_back(socket_in);
	return socket_in;
}

void SocketManager::AddValue(std::string s)
{

}

void SocketManager::AddValue(int i)
{

}

void SocketManager::Send(SOCKET s, int buffid)
{
	send(s, buffer_list.c_str(), strlen(buffer_list.c_str()), 0);
}