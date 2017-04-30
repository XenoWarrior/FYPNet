/**
* ClientLogic.cpp
*
* Author: Ashley Scott
* Date: 2016 - 2017
*
* Demonstration of a client running the FYPNet core API (built on WinSock2).
* Shows how to initialise a client using the API and connect to a server.
*
* This API uses JSON to handle all of the data transport between the client and server,
* as well as sending the data, a tag "type" is sent with the value to identify which datatype the value is.
* This should work well with all programming languages which can decode JSON strings.
*
* As a developer of a server, regardless of which language, you should be able to "cast" the string values into their types easily.
*
* Comments in ClientLogic::Run() should help guide through the flow of setting up a client.
*
* TODO: Currently, there is one static socket for a client. Urgently needs updating to accept multiple outgoing client sockets to different servers.
*/

#include "ClientLogic.h"

ClientLogic::ClientLogic() : LogicInterface()
{
	socket_manager = std::make_shared<SocketManager>();
}

/**
* ClientLogic::Run()
*
* Handles configuring of a client to connect to a server.
* This code pretty much runs identical to ServerLogic. Follow the flow of ServerLogic for further information.
*/
int ClientLogic::Run()
{
	// Used within the client to keep track of client sockets.
	// A client can connect to multiple different servers.
	socket_manager = std::make_shared<SocketManager>();

	// Initialise the API (WinSock2)
	if (!socket_manager->Initialise())
	{
		std::cout << "Unable to initialise socket manager." << std::endl;
		engine_running = false;
	}

	// Connect to a server on port 12500
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

	// While there is no error
	while (engine_running)
	{
		// Keyword: ClientSocket (different in ServerLogic), to fetch message from this specifc server.
		int message_size = socket_manager->ClientSocket()->ReceiveMessage();
		if (message_size == 0)
		{
			std::cout << "Server: " << socket_manager->ClientSocket()->GetSocket() << " disconnected." << std::endl;
			socket_manager->ClientDisconnect();
		}

		// Like ServerLogic
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
				if (message.GetValue("packet") == std::to_string(FYP_ON_CONNECT))
				{
					std::cout << "Server: " << socket_manager->ClientSocket()->GetSocket() << " said: " << message.GetValue("message") << std::endl;
				}
				else  if (message.GetValue("packet") == std::to_string(FYP_ON_MESSAGE))
				{
					std::cout << "Server: " << socket_manager->ClientSocket()->GetSocket() << " said: " << message.GetValue("message") << std::endl;
				}
				else if (message.GetValue("packet") == std::to_string(FYP_ON_INVALID_PACKET))
				{
					std::cout << "Server: " << socket_manager->ClientSocket()->GetSocket() << " said: " << message.GetValue("message") << std::endl;
				}
				else if (message.GetValue("packet") == std::to_string(FYP_ON_DISCONNECT))
				{
					std::cout << "Server: " << socket_manager->ClientSocket()->GetSocket() << " said: " << message.GetValue("message") << std::endl;
				}
				else
				{
					std::cout << "Unknown packet from server." << std::endl;
				}
			}
			else
			{
				std::cout << "An error occured." << std::endl;
			}
		}
	}

	socket_manager->Wait(5000);
	socket_manager->ClientDisconnect();

	Stop();

	return 0;
}

/**
* ClientLogic::Stop()
*
* Handles telling the server(s) that this client has disconnected.
*/
void ClientLogic::Stop()
{
	socket_manager->ClientSocket()->GetBuffer(0)->AddValue("packet", FYP_ON_DISCONNECT);
	socket_manager->ClientSocket()->GetBuffer(0)->AddValue("message", "Client has closed.");
	socket_manager->ClientSocket()->Dispatch(0);

	engine_running = false;
}
