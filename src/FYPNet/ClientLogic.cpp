#include "ClientLogic.h"

ClientLogic::ClientLogic()
{

}

int ClientLogic::Run()
{
	std::shared_ptr<SocketManager> socket_manager = std::make_shared<SocketManager>();

	if (!socket_manager->Initialise())
	{
		std::cout << "Unable to initialise socket manager." << std::endl;
		engine_running = false;
	}

	if (!socket_manager->Connect("localhost", 12500))
	{
		std::cout << "Unable to connect to server." << std::endl;
		socket_manager->Wait(3000);
		engine_running = false;
	}

	// Send a test
	if (engine_running)
	{
		socket_manager->ClientSocket()->GetBuffer(0)->AddValue("packet", FYP_ON_CONNECT);
		socket_manager->ClientSocket()->GetBuffer(0)->AddValue("message", std::string("Hello, world!"));
		socket_manager->ClientSocket()->Dispatch(0);

	}

	while (engine_running)
	{
		int message_size = socket_manager->ClientSocket()->ReceiveMessage();
		if (message_size == 0)
		{
			std::cout << "Server: " << socket_manager->ClientSocket()->GetSocket() << " disconnected." << std::endl;
			socket_manager->ClientDisconnect();
		}

		if (message_size > 0)
		{
			std::string raw_message = socket_manager->ClientSocket()->ReadMessage(message_size);

			std::cout << "----------------------------------------------------------------------------" << std::endl;
			std::cout << "MESSAGE FROM SERVER: " << socket_manager->ClientSocket()->GetSocket() << ":" << std::endl;
			std::cout << raw_message << std::endl;
			std::cout << "----------------------------------------------------------------------------" << std::endl;

			PacketData message = PacketData(std::string(raw_message));

			if (!message.CheckError())
			{
				if (message.GetValue("packet") == std::to_string((int)FYP_ON_CONNECT))
				{
					std::cout << "Server: " << socket_manager->ClientSocket()->GetSocket() << " said: " << message.GetValue("message") << std::endl;
				}
				if (message.GetValue("packet") == std::to_string((int)FYP_ON_MESSAGE))
				{
					std::cout << "Server: " << socket_manager->ClientSocket()->GetSocket() << " said: " << message.GetValue("message") << std::endl;
				}
				if (message.GetValue("packet") == std::to_string((int)FYP_ON_INVALID_PACKET))
				{
					std::cout << "Server: " << socket_manager->ClientSocket()->GetSocket() << " said: " << message.GetValue("message") << std::endl;
				}
			}
			else
			{
				std::cout << "Unknown server response" << std::endl;
			}
		}
	}

	return 0;
}

void ClientLogic::Stop()
{
	engine_running = false;
}