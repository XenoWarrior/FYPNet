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
	~SocketManager();

	// Init
	int Initialise();
	int Close();
	int Listen(int, int, bool);

	// Misc
	void Wait(int);

	// Main
	int Connect();
	SOCKET Accept(int);
	void Send(SOCKET, int);

	// Data
	void AddValue(std::string);
	void AddValue(int);

private:
	WSADATA wsa_data;
	SOCKET listen_socket = INVALID_SOCKET;

	std::vector<SOCKET> socket_list;

	// Soon to be a list of buffers with their own data (ideal for multi-threading)...
	std::string buffer_list = "NOT IMPLEMENTED YET";

};

#endif // !SocketManager_H