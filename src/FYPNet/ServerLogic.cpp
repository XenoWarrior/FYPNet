#include "ServerLogic.h"

ServerLogic::ServerLogic()
{

}

int ServerLogic::Run()
{
	std::shared_ptr<SocketManager> socket_manager = std::make_shared<SocketManager>();

	if (!socket_manager->Initialise())
	{
		std::cout << "Unable to initialise socket manager." << std::endl;
		engine_running = false;
	}

	if (!socket_manager->Listen(12500, 1000, true))
	{
		std::cout << "Unable to open socket on port 12500." << std::endl;
		engine_running = false;
	}

	while (engine_running)
	{
		int new_client = socket_manager->Accept(true);
		if (new_client > -1)
		{
			std::cout << "Accepted new client!" << std::endl;
			socket_manager->GetSocket(new_client)->GetBuffer(0)->AddValue("packet", FYP_ON_CONNECT);
			socket_manager->GetSocket(new_client)->GetBuffer(0)->AddValue("message", std::string("Welcome to the server."));
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
				std::string raw_message = socket_manager->GetSocket(i)->ReadMessage(message_size);

				std::cout << "----------------------------------------------------------------------------" << std::endl;
				std::cout << "MESSAGE FROM CLIENT: " << socket_manager->GetSocket(i)->GetSocket() << ":" << std::endl;
				std::cout << raw_message << std::endl;
				std::cout << "----------------------------------------------------------------------------" << std::endl;

				PacketData message = PacketData(std::string(raw_message));

				if (!message.CheckError())
				{
					if (message.GetValue("packet") == std::to_string((int)FYP_ON_CONNECT))
					{
						std::cout << "Socket: " << socket_manager->GetSocket(i)->GetSocket() << " said: " << message.GetValue("message") << std::endl;

						// For game logics, server can inform all clients that this client connected here.
					}
					if (message.GetValue("packet") == std::to_string((int)FYP_ON_MESSAGE))
					{
						// For message logic, server can inform all clients of messages to the server here.
						for (std::shared_ptr<Socket> s : socket_manager->GetSocketList())
						{
							if (s != socket_manager->GetSocket(i))
							{
								s->GetBuffer(0)->AddValue("packet", FYP_ON_MESSAGE);
								s->GetBuffer(0)->AddValue("message", std::string(message.GetValue("message")));
								s->Dispatch(0);
							}
							else
							{
								s->GetBuffer(0)->AddValue("packet", FYP_ON_MESSAGE);
								s->GetBuffer(0)->AddValue("message", std::string("Your message was sent."));
								s->Dispatch(0);
							}
						}
					}
				}
				else
				{
					socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("packet", FYP_ON_INVALID_PACKET);
					socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("message", message.GetErrorMessage());
					socket_manager->GetSocket(i)->Dispatch(0);
				}
			}
		}
	}

	socket_manager->Wait(5000);
	socket_manager->Close();

	return 0;
}

void ServerLogic::Stop()
{
	engine_running = false;
}