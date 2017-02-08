#include <memory>
#include <iostream>

#include "json.hpp"
#include "SocketManager.h"

#pragma comment(lib, "Ws2_32.lib")

bool server_running = true;
BOOL WINAPI ConsoleHandler(DWORD signal)
{
	switch (signal)
	{
		default:
		{
			std::cout << "[----] Shutdown event detected" << std::endl;
			server_running = false;
		}
	}

	return TRUE;
}

int main()
{
	std::shared_ptr<SocketManager> socket_manager = std::make_shared<SocketManager>();

	if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE))
	{
		std::cout << "Unable to register console handler" << std::endl;
		server_running = false;
	}

	if (!socket_manager->Initialise())
	{
		std::cout << "Unable to initialise socket manager." << std::endl;
	}

	if (!socket_manager->Listen(12500, 1000, true))
	{
		std::cout << "Unable to open socket on port 12500." << std::endl;
	}

	while (server_running)
	{
		int new_client = socket_manager->Accept(true);
		if (new_client != 0)
		{
			socket_manager->GetSocket(new_client)->GetBuffer(0)->AddValue("message", "Hello!");
			socket_manager->GetSocket(new_client)->GetBuffer(0)->AddValue("x", 100);
			socket_manager->GetSocket(new_client)->GetBuffer(0)->AddValue("y", 100);
			socket_manager->GetSocket(new_client)->Dispatch(0);
		}
	}

	socket_manager->Wait(5000);
	socket_manager->Close();

	return 0;
}