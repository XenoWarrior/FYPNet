#include <memory>
#include <algorithm>
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
		if (new_client > -1)
		{
			std::cout << "Accepted new client!" << std::endl;
			socket_manager->GetSocket(new_client)->GetBuffer(0)->AddValue("message", "Hello!");
			socket_manager->GetSocket(new_client)->Dispatch(0);
		}

		for (int i = 0; i < socket_manager->GetSocketList().size(); i++)
		{
			int message_size = socket_manager->GetSocket(i)->ReceiveMessage();
			if (message_size == 0)
			{
				std::cout << "Socket: " << socket_manager->GetSocket(i)->GetSocket() << " disconnected." << std::endl;
				socket_manager->Disconnect(i);
			}

			if (message_size > 0)
			{
				std::string message = socket_manager->GetSocket(i)->ReadMessage(message_size);
				std::cout << "Socket: " << socket_manager->GetSocket(i)->GetSocket() << " said: " << message << std::endl;

				if (message == std::string("Hello\r"))
				{
					socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("message", "Hello again!");
					socket_manager->GetSocket(i)->Dispatch(0);
				}
			}
		}
	}

	socket_manager->Wait(5000);
	socket_manager->Close();

	return 0;
}