#ifndef Socket_H
#define Socket_H

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <iphlpapi.h>
#include <stdio.h>

#include <vector>

#include "Buffer.h"

class Socket {
public:
	Socket(SOCKET s);

	int AddBuffer();
	std::shared_ptr<Buffer> GetBuffer(int buffer_id);
	void Dispatch(int buffer_id);

private:
	SOCKET socket;
	std::vector<std::shared_ptr<Buffer>> buffer_list;

};

#endif // !Socket_H
