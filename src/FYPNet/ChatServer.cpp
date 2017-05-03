/**
* ChatServer.cpp (Prototype)
*
* A prototype chat server implementation.
*/

#include "ChatServer.h"

ChatServer::ChatServer()
{

}

int ChatServer::Run()
{
	socket_manager = std::make_shared<SocketManager>();

	int unique_conns = 0;

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
		socket_manager->Wait(10);

		int new_client = socket_manager->Accept(true);
		if (new_client > -1)
		{
			std::cout << "Accepted new client!" << std::endl;
		}

		for (int i = 0; i < socket_manager->GetSocketList().size(); i++)
		{
			int message_size = socket_manager->GetSocket(i)->ReceiveMessage();

			if (message_size == 0 or socket_manager->SocketConnected(i) == FYP_SOCK_FAILURE)
			{
				std::cout << "[ChatServer] Socket: " << socket_manager->GetSocket(i)->GetSocket() << " disconnected." << std::endl;
				socket_manager->Disconnect(i);
			}

			if (message_size > 0)
			{
				std::string _raw_data = socket_manager->GetSocket(i)->ReadMessage(message_size);
				PacketData message = PacketData(std::string(_raw_data));

				if (!message.CheckError())
				{
					// CONNECT
					// Here we can connect the user and inform all other users that someone connected
					if (message.GetValue("packet") == std::to_string(FYP_ON_CONNECT))
					{
						std::cout << "Client says: " << message.GetValue("message") << std::endl;

						// Send a username back to the client, we could have a method which asks for the username, too.
						socket_manager->GetSocket(i)->GetBuffer(0)->ClearBuffer();
						socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("packet", FYPCP_ON_ACCEPT);
						socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("username", "UserID" + std::to_string(unique_conns));
						socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("message", "[Server] Welcome to the server, User " + std::to_string(unique_conns));
						socket_manager->GetSocket(i)->Dispatch(0);
						
						// Now we can inform all members this person has joined.
						for (std::shared_ptr<Socket> s : socket_manager->GetSocketList())
						{
							// Tell everyone the message that is not this client
							if (s != socket_manager->GetSocket(i))
							{
								s->GetBuffer(0)->ClearBuffer();
								s->GetBuffer(0)->AddValue("packet", FYPCP_ON_NEWUSER);
								s->GetBuffer(0)->AddValue("message", "[Server] User " + std::to_string(unique_conns) + " has joined the chat.");
								s->Dispatch(0);
							}
						}

						// Increment unique connections
						unique_conns++;
					}
					else if(message.GetValue("packet") == std::to_string(FYPCP_ON_MESSAGE))
					{
						// Now we can inform all members this person has joined.
						for (std::shared_ptr<Socket> s : socket_manager->GetSocketList())
						{
							// Tell everyone the message
							s->GetBuffer(0)->ClearBuffer();
							s->GetBuffer(0)->AddValue("packet", FYPCP_ON_NEWUSER);
							s->GetBuffer(0)->AddValue("message", message.GetValue("message"));
							s->Dispatch(0);
						}
					}
					else
					{
						// Unknown packet type in feed.
						socket_manager->GetSocket(i)->GetBuffer(0)->ClearBuffer();
						socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("packet", FYP_ON_INVALID_PACKET);
						socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("message", "[Server] Unknown packet: " + message.GetValue("packet"));
						socket_manager->GetSocket(i)->Dispatch(0);
					}
				}
				else
				{
					// Some invalid packet came through, just send a warning to the client. (mostly for debug and to turn scriptkiddies away)
					socket_manager->GetSocket(i)->GetBuffer(0)->ClearBuffer();
					socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("packet", FYP_ON_INVALID_PACKET);
					socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("message", "[Server]" + message.GetErrorMessage());
					socket_manager->GetSocket(i)->Dispatch(0);
				}
			}
		}
	}

	socket_manager->Wait(5000);
	socket_manager->DisconnectAll();
	socket_manager->Close();

	Stop();

	return 0;
}

void ChatServer::Stop()
{
	engine_running = false;
}