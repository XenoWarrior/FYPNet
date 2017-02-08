#ifndef SocketManager_H
#define SocketManager_H

#include <iostream>
#include <vector>

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <iphlpapi.h>
#include <stdio.h>

#include "Shared.h"
#include "Socket.h"
#include "Buffer.h"

class SocketManager {
public:
	SocketManager();

	int Initialise();
	int Close();

	int Connect();
	int Listen(int port, int max_connections, bool mode);

	void Wait(int time_ms);
	int Accept(bool mode);

	std::shared_ptr<Socket> GetSocket(int socket_id);

private:
	WSADATA wsa_data;
	SOCKET listen_socket = INVALID_SOCKET;

	std::vector<std::shared_ptr<Socket>> socket_list;

};

#endif // !SocketManager_H