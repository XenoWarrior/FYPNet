#include "ZoneServer.h"

ZoneServer::ZoneServer()
{

}

int ZoneServer::Run()
{

	socket_manager = std::make_shared<SocketManager>();
	player_manager = std::make_shared<PlayerManager>();

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

			if (message_size == 0 or socket_manager->SocketConnected(i) == FYP_SOCK_FAILURE)
			{
				std::cout << "[ZoneServer] Socket: " << socket_manager->GetSocket(i)->GetSocket() << " disconnected." << std::endl;
				player_manager->RemovePlayer(socket_manager->GetSocket(i)->GetSocket());
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

							// Add the player
							player_manager->AddPlayer(message.GetValue("username"), socket_manager->GetSocket(i)->GetSocket());

							// Debug
							std::cout << "[ZoneServer] Name -> Socket: Player(" << player_manager->GetPlayer(message.GetValue("username"))->player_name << ")" << std::endl;
							std::cout << "[ZoneServer] Socket -> Name: Player(" << player_manager->GetPlayer(socket_manager->GetSocket(i)->GetSocket())->player_name << ")" << std::endl;
							
							// Debug
							std::cout << "[ZoneServer] Authenticated: " << message.GetValue("username") << std::endl;

							// Tell already connected players who connected
							for (auto s : socket_manager->GetSocketList())
							{
								// Make sure we're not sending the new player back to the new player
								// They would see duplicate objects of themselves.
								// One which they control and one which does not get updated.
								if (s != socket_manager->GetSocket(i))
								{
									std::cout << "[GameServer] Telling " << player_manager->GetPlayer(s->GetSocket())->player_name << " that " << message.GetValue("username") << " has joined." << std::endl;

									s->GetBuffer(0)->ClearBuffer();
									s->GetBuffer(0)->AddValue("packet", FYPGP_ON_NEWPLAYER);
									s->GetBuffer(0)->AddValue("name", message.GetValue("username"));
									s->Dispatch(0);

									std::cout << "[GameServer] Telling " << message.GetValue("username") << " that " << player_manager->GetPlayer(s->GetSocket())->player_name << " exists." << std::endl;

									socket_manager->GetSocket(i)->GetBuffer(0)->ClearBuffer();
									socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("packet", FYPGP_ON_NEWPLAYER);
									socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("name", player_manager->GetPlayer(s->GetSocket())->player_name);
									socket_manager->GetSocket(i)->Dispatch(0);
								}
							}
						}
						else if (message.GetValue("packet") == std::to_string(FYPGP_ON_UPDATEPOS))
						{
							std::cout << "[ZoneServer] Updated position for: " << message.GetValue("character") << std::endl;

							player_manager->GetPlayer(message.GetValue("character"))->player_pos["x"] = stoi(message.GetValue("x"));
							player_manager->GetPlayer(message.GetValue("character"))->player_pos["y"] = stoi(message.GetValue("y"));
							player_manager->GetPlayer(message.GetValue("character"))->player_pos["angle"] = stoi(message.GetValue("angle"));

							for (auto s : socket_manager->GetSocketList())
							{
								if (s != socket_manager->GetSocket(i))
								{
									s->GetBuffer(0)->ClearBuffer();
									s->GetBuffer(0)->AddValue("packet", std::to_string(FYPGP_ON_UPDATEPOS));
									s->GetBuffer(0)->AddValue("character", message.GetValue("character"));
									s->GetBuffer(0)->AddValue("x", std::to_string(player_manager->GetPlayer(message.GetValue("character"))->player_pos["x"]));
									s->GetBuffer(0)->AddValue("y", std::to_string(player_manager->GetPlayer(message.GetValue("character"))->player_pos["y"]));
									s->GetBuffer(0)->AddValue("angle", std::to_string(player_manager->GetPlayer(message.GetValue("character"))->player_pos["angle"]));
									s->Dispatch(0);
								}
							}
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