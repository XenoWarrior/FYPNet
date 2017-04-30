#include <memory>
#include <algorithm>
#include <iostream>

#include "json.hpp"
#include "SocketManager.h"
#include "PacketData.h"

#pragma comment(lib, "Ws2_32.lib")

bool server_running = true;

/**
 * Handles detection of termination signals from Windows.
*/
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

/**
* Main program for testing the networking library API.
*/
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
			socket_manager->GetSocket(new_client)->GetBuffer(0)->AddValue("packet", FYP_OUT_WELCOME_RESPONSE);
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
				PacketData message = PacketData(socket_manager->GetSocket(i)->ReadMessage(message_size));

				if (message.GetValue("packet") == "FYP_IN_CLIENT_MESSAGE")
				{
					std::cout << "Socket: " << socket_manager->GetSocket(i)->GetSocket() << " said: " << message.GetValue("message") << " -- Type:" << message.GetType("message") << std::endl;

					for (std::shared_ptr<Socket> s : socket_manager->GetSocketList())
					{
						if (s != socket_manager->GetSocket(i))
						{
							s->GetBuffer(0)->AddValue("packet", FYP_OUT_CLIENT_MESSAGE);
							s->GetBuffer(0)->AddValue("message", message.GetValue("message"));
							s->Dispatch(0);
						}
					}
				}
			}
		}
	}

	socket_manager->Wait(5000);
	socket_manager->Close();

	return 0;
}