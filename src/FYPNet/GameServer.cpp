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
		std::cout << "Could not initialise socket manager" << std::endl;
		engine_running = false;
	}

	if (!socket_manager->Listen(12500, 1000, true))
	{
		std::cout << "Could not initialise listen socket" << std::endl;
		engine_running = false;
	}

	while (engine_running)
	{
		int new_client = socket_manager->Accept(true);

		if (new_client > -1)
		{
			std::cout << "Accepted new client!" << std::endl;
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
				std::string _raw_data = socket_manager->GetSocket(i)->ReadMessage(message_size);
				PacketData message = PacketData(std::string(_raw_data));

				if (!message.CheckError())
				{
					// LOGIN / REGISTER
					// For this prototype, we won't bother fetching any data from a database.
					// Just return a static account.
					if (message.GetValue("packet") == std::to_string(FYP_ON_CONNECT))
					{
						if (message.GetValue("connect_type") == "Register")
						{
							std::cout << "Request registration, but this is just a prototype so we can just force the client to connect with an account." << std::endl;
						}
						
						// Send force login (*FYPGP = GameServer Prototype)
						socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("packet", FYPGP_ON_LOGIN);
						socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("username", "FYProto");
						socket_manager->GetSocket(i)->Dispatch(0);

						// Could have a PlayerManager here which keeps track of logged in users, caches their account to minimise database connections.
						// {...}
					}

					// CHARACER SELECT / CREATE
					// Again, another static prototype method.
					// Send some static character packages.
					if (message.GetValue("packet") == std::to_string(FYPGB_ON_GETCHARS))
					{
						// Could look at the PlayerManager here for the user account data for this specific socket.
						// {...}

						// Send back some character data.
						socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("packet", FYPGB_ON_GETCHARS);
						socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("character", "male;8;0;0;1;0;1;0;1;1;10;0;-1;4;7;3;0;0;0;1;0;0;0;1;7;5;0;1;5");
						socket_manager->GetSocket(i)->Dispatch(0);
					}
				}
				else
				{
					// Some invalid packet came through, just send a warning to the client. (mostly for debug and to turn scriptkiddies away)
					socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("packet", FYP_ON_INVALID_PACKET);
					socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("message", message.GetErrorMessage());
					socket_manager->GetSocket(i)->Dispatch(0);
				}
			}
		}
	}

	socket_manager->Wait(5000);
	socket_manager->Close();
	
	Stop();
	
	return 0;
}

void GameServer::Stop()
{
	engine_running = false;
}