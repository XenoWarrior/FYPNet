#include "ZoneServer.h"

ZoneServer::ZoneServer()
{

}

int ZoneServer::Run()
{

	socket_manager = std::make_shared<SocketManager>();

	if (!socket_manager->Initialise())
	{
		std::cout << "[ZoneServer] Could not initialise socket manager" << std::endl;
		engine_running = false;
	}

	if (!socket_manager->Listen(7100, 1000, true))
	{
		std::cout << "[ZoneServer] Could not initialise listen socket" << std::endl;
		engine_running = false;
	}

	// In the old logic, the zoneserver would ask the master server for a configuration.
	// For the sake of prototyping, the master server will send a hard-coded response pointing to this server on localhost:7100
	while (engine_running)
	{
		int new_client = socket_manager->Accept(true);

		if (new_client > -1)
		{
			std::cout << "[ZoneServer] Accepted new client!" << std::endl;
		}

		for (int i = 0; i < socket_manager->GetSocketList().size(); i++)
		{
			int message_size = socket_manager->GetSocket(i)->ReceiveMessage();

			if (message_size == 0)
			{
				std::cout << "[ZoneServer] Socket: " << socket_manager->GetSocket(i)->GetSocket() << " disconnected." << std::endl;
				socket_manager->Disconnect(i);
			}

			if (message_size > 0)
			{
				std::string _raw_data = socket_manager->GetSocket(i)->ReadMessage(message_size);
				PacketData message = PacketData(std::string(_raw_data));

				if (!message.CheckError())
				{
					try
					{
						if (message.GetValue("packet") == std::to_string(FYP_ON_CONNECT))
						{
							// Send force login (*FYPGP = GameServer Prototype)
							socket_manager->GetSocket(i)->GetBuffer(0)->ClearBuffer();
							socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("packet", FYPGP_ON_LOGIN);
							socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("username", message.GetValue("username"));
							socket_manager->GetSocket(i)->Dispatch(0);

							std::cout << "[ZoneServer] Authenticated: " << message.GetValue("username") << std::endl;
						}
						else if (message.GetValue("packet") == std::to_string(FYPGP_ON_UPDATEPOS))
						{
							
						}
						else
						{
							socket_manager->GetSocket(i)->GetBuffer(0)->ClearBuffer();
							socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("packet", FYP_ON_INVALID_PACKET);
							socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("message", std::string("Unknown packet ID " + message.GetValue("packet")));
							socket_manager->GetSocket(i)->Dispatch(0);
						}


					}
					catch (std::exception e)
					{
						// Error in client request
						std::cout << "[ZoneServer] Exception Caught: " << e.what() << std::endl;

						// Disconenct client to send it back to menu screen
						socket_manager->Disconnect(i);
					}
				}
				else
				{
					// Some invalid packet came through, just send a warning to the client. (mostly for debug and to turn scriptkiddies away)
					socket_manager->GetSocket(i)->GetBuffer(0)->ClearBuffer();
					socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("packet", FYP_ON_INVALID_PACKET);
					socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("message", message.GetErrorMessage());
					socket_manager->GetSocket(i)->Dispatch(0);
				}
			}
		}
	}

	socket_manager->Wait(5000);
	socket_manager->DisconnectAll();
	socket_manager->Close();

	return 0;
}

void ZoneServer::Stop()
{
	engine_running = false;
}