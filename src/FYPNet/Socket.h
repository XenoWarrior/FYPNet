#ifndef Socket_H
#define Socket_H

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <iphlpapi.h>
#include <stdio.h>

#include <sstream>
#include <iomanip>

#include <vector>

#include "Buffer.h"

class Socket {
public:
	Socket(SOCKET s);

	SOCKET GetSocket();

	int AddBuffer();
	std::shared_ptr<Buffer> GetBuffer(int buffer_id);

	int ReceiveMessage();
	std::string ReadMessage(int size);

	void Dispatch(int buffer_id);

private:
	SOCKET socket;
	std::vector<std::shared_ptr<Buffer>> buffer_list;

	char message_in[8195];
};

#endif // !Socket_H
