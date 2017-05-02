/**
* GameServer.cpp (Prototype)
*
* A prototype game server implementation.
* 
* Points to consider:
* - Something like this could be run on an MMO server with the appropriate technology:
*    - (Multi-threading would be very important for this!)
* - Works great in this current state for a small multiplayer game (of up to 16 players?)
*    - (Untested player limit, might be more or less than 16 players)
*/

#include "GameServer.h"

GameServer::GameServer()
{

}

int GameServer::Run()
{
	socket_manager = std::make_shared<SocketManager>();

	if (!socket_manager->Initialise())
	{
		std::cout << "[GameServer] Could not initialise socket manager" << std::endl;
		engine_running = false;
	}

	if (!socket_manager->Listen(12500, 1000, true))
	{
		std::cout << "[GameServer] Could not initialise listen socket" << std::endl;
		engine_running = false;
	}

	while (engine_running)
	{
		int new_client = socket_manager->Accept(true);

		if (new_client > -1)
		{
			std::cout << "[GameServer] Accepted new client!" << std::endl;
		}

		for (int i = 0; i < socket_manager->GetSocketList().size(); i++)
		{
			int message_size = socket_manager->GetSocket(i)->ReceiveMessage();

			if (message_size == 0)
			{
				std::cout << "[GameServer] Socket: " << socket_manager->GetSocket(i)->GetSocket() << " disconnected." << std::endl;
				socket_manager->Disconnect(i);
			}

			if (message_size > 0)
			{
				std::string _raw_data = socket_manager->GetSocket(i)->ReadMessage(message_size);
				PacketData message = PacketData(std::string(_raw_data));

				if (!message.CheckError())
				{
					// LOGIN / REGISTER
					// For this prototype, we won't bother fetching any data from a database.
					// Just return a static account.
					try
					{
						if (message.GetValue("packet") == std::to_string(FYP_ON_CONNECT))
						{
							// Send force login (*FYPGP = GameServer Prototype)
							socket_manager->GetSocket(i)->GetBuffer(0)->ClearBuffer();
							socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("packet", FYPGP_ON_LOGIN);
							socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("username", message.GetValue("username"));
							socket_manager->GetSocket(i)->Dispatch(0);

							std::cout << "[GameServer] Authenticated: " << message.GetValue("username") << std::endl;

							// Could have a PlayerManager here which keeps track of logged in users, caches their account to minimise database connections.
							// {...}
						}

						// CHARACER SELECT / CREATE
						// Again, another static prototype method.
						// Send some static character packages.
						else if (message.GetValue("packet") == std::to_string(FYPGP_ON_GETCHARS))
						{
							// Could look at the PlayerManager here for the user account data for this specific socket.
							// {...}

							// Send back some character data.
							socket_manager->GetSocket(i)->GetBuffer(0)->ClearBuffer();
							socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("packet", FYPGP_ON_GETCHARS);
							socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("0", std::string("XenoWarrior:0:male;8;0;0;1;0;1;0;1;1;7;0;-1;4;7;3;0;0;0;1;0;0;0;1;7;5;0;1;5:13:37:0"));
							socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("1", std::string("Zenro45:0:male;10;4;2;3;2;3;0;7;3;10;2;3;1;0;0;0;0;0;4;2;0;0;0;0;0;0;3;1:2:334:0"));
							//socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("2", std::string("TestPleb:0:female;1;0;0;1;0;1;0;1;1;1;0;-1;1;0;0;0;0;0;1;0;0;0;0;0;0;0;1;0:24:4450:0"));
							socket_manager->GetSocket(i)->Dispatch(0);
						}

						// GET ZONE LIST
						// This method will send a list of available zone servers (which provide the in-game logic)
						// Due to prototyping, this will only return a static server, make sure it is running!
						else if (message.GetValue("packet") == std::to_string(FYPGP_ON_GETZONES))
						{
							// Send a static zone server config to client
							socket_manager->GetSocket(i)->GetBuffer(0)->ClearBuffer();
							socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("packet", FYPGP_ON_GETZONES);
							socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("0", std::string("ProtoZone;0;7100;0"));
							socket_manager->GetSocket(i)->Dispatch(0);

							std::cout << "[GameServer] Sent zone list." << std::endl;

						}

						// JOIN ZONE
						// Client will ask the server if it can join the zone, server checks conditions (like valid password before allowing)
						// A further check should also be done on the zone server and disconnect anyone not authenticated to join.
						else if (message.GetValue("packet") == std::to_string(FYPGP_ON_JOINZONE))
						{
							// Accept the client join request
							socket_manager->GetSocket(i)->GetBuffer(0)->ClearBuffer();
							socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("packet", FYPGP_ON_ZONEACCEPT);
							socket_manager->GetSocket(i)->Dispatch(0);

							std::cout << "[GameServer] Sent zone join accept." << std::endl;
						}

						// If the packet is not known in the list above
						else
						{
							socket_manager->GetSocket(i)->GetBuffer(0)->ClearBuffer();
							socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("packet", FYP_ON_INVALID_PACKET);
							socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("message", std::string("Unknown packet ID " + message.GetValue("packet")));
							socket_manager->GetSocket(i)->Dispatch(0);
							
							std::cout << "[GameServer] Sent invalid packet message." << std::endl;
						}
					}
					catch (std::exception e)
					{
						// Error in client request
						std::cout << "[GameServer] Exception Caught: " << e.what() << std::endl;

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

					std::cout << "[GameServer] Sent invalid packet message." << std::endl;
				}
			}
		}
	}

	socket_manager->Wait(5000);
	socket_manager->DisconnectAll();
	socket_manager->Close();
	
	return 0;
}

void GameServer::Stop()
{
	engine_running = false;
}