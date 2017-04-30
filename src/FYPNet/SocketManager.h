#ifndef SocketManager_H
#define SocketManager_H

#include <iostream>
#include <vector>

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <iphlpapi.h>
#include <stdio.h>

#include "FYPClientStatus.h"
#include "FYPSocketStatus.h"

#include "Socket.h"
#include "Buffer.h"

class SocketManager {
public:
	SocketManager();

	int Initialise();
	int Close();

	int Connect(std::string ip, int port);
	int Disconnect(int socket_id);
	int Accept(bool mode);

	int Listen(int port, int max_connections, bool mode);

	void Wait(int time_ms);

	std::vector<std::shared_ptr<Socket>> GetSocketList();
	std::shared_ptr<Socket> GetSocket(int socket_id);

	std::shared_ptr<Socket> ClientSocket();
	void ClientDisconnect();

private:
	WSADATA wsa_data;

	SOCKET listen_socket = INVALID_SOCKET;
	SOCKET client_socket = INVALID_SOCKET; // for init
	std::shared_ptr<Socket> main_client_socket; // for use

	std::vector<std::shared_ptr<Socket>> socket_list;

	bool is_client = false;
};

#endif // !SocketManager_H