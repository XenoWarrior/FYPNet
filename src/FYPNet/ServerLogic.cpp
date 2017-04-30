/**
* ServerClient.cpp
*
* Author: Ashley Scott
* Date: 2016 - 2017
*
* Demonstration of a server running the FYPNet core API (built on WinSock2).
* Shows how to initialise a server using the API and listen for connections.
*
* This API uses JSON to handle all of the data transport between the client and server,
* as well as sending the data, a tag "type" is sent with the value to identify which datatype the value is.
* This should work well with all programming languages which can decode JSON strings.
* 
* As a developer of a server, regardless of which language, you should be able to "cast" the string values into their types easily.
*
* Comments in ServerLogic::Run() should help guide through the flow of setting up a server.
*/

#include "ServerLogic.h"

ServerLogic::ServerLogic() : LogicInterface()
{

}

/**
* ServerLogic::Run()
* 
* Handles configuring of a server to listen for connections.
*/
int ServerLogic::Run()
{
	// Setup a Socket Manager, handles keeping track of all connected clients.
	socket_manager = std::make_shared<SocketManager>();

	// Before listening for a connection, initialise the API (some internal WinSock2 get setup here).
	if (!socket_manager->Initialise())
	{
		std::cout << "Unable to initialise socket manager." << std::endl;
		engine_running = false;
	}

	// Start listening for connections, the first number is port, second is max connections, third is mode (blocking false, non-blocking true).
	if (!socket_manager->Listen(12500, 1000, true))
	{
		std::cout << "Unable to open socket on port 12500." << std::endl;
		engine_running = false;
	}

	// If all is good, then this loop will run forever until the program gets a termination request.
	// Termination from keyboard is either closing the program, or CTRL+C
	while (engine_running)
	{
		// Allow new clients to be accepted on non-blocking port.
		int new_client = socket_manager->Accept(true);

		// If we actually have a socket connection (will ID be more than 0 if this is the case).
		if(new_client > -1)
		{
			// Start by sending back a welcome message, this is entirely optional and is used to just test the program out.
			std::cout << "Accepted new client!" << std::endl;
			socket_manager->GetSocket(new_client)->GetBuffer(0)->AddValue("packet", FYP_ON_CONNECT);
			socket_manager->GetSocket(new_client)->GetBuffer(0)->AddValue("message", std::string("Welcome to the server."));
			socket_manager->GetSocket(new_client)->Dispatch(0);
		}

		// Now we fetch the SocketList to loop through and get all possible messages from every client connected to the server.
		for (int i = 0; i < socket_manager->GetSocketList().size(); i++)
		{
			// Start by getting the message, it will first return a message size so we know how much to read from the buffer.
			int message_size = socket_manager->GetSocket(i)->ReceiveMessage();

			// If there is no message (not even a ping from the client), it has disconnected. We can disconnect it properly here.
			if (message_size == 0)
			{
				std::cout << "Socket: " << socket_manager->GetSocket(i)->GetSocket() << " disconnected." << std::endl;
				socket_manager->Disconnect(i);
			}

			// If there is a message and it is more than 0, we actually have data to fetch.
			if (message_size > 0)
			{
				// Start by getting the raw JSON string from the client. 
				std::string raw_message = socket_manager->GetSocket(i)->ReadMessage(message_size);

				// Some debug output, can be removed.
				std::cout << "----------------------------------------------------------------------------" << std::endl;
				std::cout << "MESSAGE FROM CLIENT: " << socket_manager->GetSocket(i)->GetSocket() << ":" << std::endl;
				std::cout << raw_message << std::endl;
				std::cout << "----------------------------------------------------------------------------" << std::endl;

				// Parse this data into a PacketData object which makes our JSON easy to get values from.
				PacketData message = PacketData(std::string(raw_message));

				// In the case that there was an error parsing the JSON, we should check for errors.
				if (!message.CheckError())
				{
					// Main server packet selection block.
					// We should be sending identical packet IDs from the client and server.
					// (Types can be changed and added in FPYPacketType.h).

					// This will handle any connecting clients. Here we can inform all other clients someone has joined (or not, optional really).
					if (message.GetValue("packet") == std::to_string(FYP_ON_CONNECT))
					{
						std::cout << "Client: " << socket_manager->GetSocket(i)->GetSocket() << " said: " << message.GetValue("message") << std::endl;

						// For game logic, server can inform all clients that this client connected here.
						for (std::shared_ptr<Socket> s : socket_manager->GetSocketList())
						{
							// Send back that the client has connected to every client which is not this specific client.
							if (s != socket_manager->GetSocket(i))
							{
								// The AddValue method can be overloaded with different types.
								// You can add more types if you wish to Buffer.cpp / .h.
								// Simply adds data to the buffer (which starts of as a list (map)).
								s->GetBuffer(0)->AddValue("packet", FYP_ON_MESSAGE);
								s->GetBuffer(0)->AddValue("message", std::string("Client " + std::to_string(s->GetSocket()) + " has connected!"));

								// This will now send the buffer data to the socket. (Socket->Dispatch(buffer_id)).
								s->Dispatch(0);
							}
						}
					}

					// This is more for a test than anything (can be used for a chat client or an in-game chat, or whatever).
					// We get a message from the server and send it back to every client (excluding the one who sent it).
					else if (message.GetValue("packet") == std::to_string(FYP_ON_MESSAGE))
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

					// Useful in game logic when informing all clients that a player has disconnected.
					else if (message.GetValue("packet") == std::to_string(FYP_ON_DISCONNECT))
					{
						// For game logics, server can inform all clients that this client connected here.
						for (std::shared_ptr<Socket> s : socket_manager->GetSocketList())
						{
							if (s != socket_manager->GetSocket(i))
							{
								s->GetBuffer(0)->AddValue("packet", FYP_ON_MESSAGE);
								s->GetBuffer(0)->AddValue("message", std::string("Client " + std::to_string(s->GetSocket()) + " has disconnected!"));
								s->Dispatch(0);
							}
						}
					}
				}
				else
				{
					// We can also send back some information if somethign that is not valid was sent.
					// Useful against scriptkiddies who like to type the server IP into a web-browser or whatever.
					socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("packet", FYP_ON_INVALID_PACKET);
					socket_manager->GetSocket(i)->GetBuffer(0)->AddValue("message", message.GetErrorMessage());
					socket_manager->GetSocket(i)->Dispatch(0);
				}
			}
		}
	}

	// On exit, there might have been an error so we wait for a few seconds to give user time to read it.
	socket_manager->Wait(5000);

	// Close the server socket.
	socket_manager->Close();

	// Rather than duplicating code, here we call Stop() which just sends out a packet informing clients the server has gone down.
	Stop();

	// End the logic.
	return 0;
}

/**
* ServerLogic::Stop()
*
* Handles telling users and stopping the main loop if the server has been terminated.
*/
void ServerLogic::Stop()
{
	for (std::shared_ptr<Socket> s : socket_manager->GetSocketList())
	{
		s->GetBuffer(0)->AddValue("packet", FYP_ON_DISCONNECT);
		s->GetBuffer(0)->AddValue("message", "Server has closed.");
		s->Dispatch(0);
	}

	engine_running = false;
}
