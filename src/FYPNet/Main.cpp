// Interface for logics
#include "LogicInterface.h"

// Debug logics
#include "ServerLogic.h"
#include "ClientLogic.h"

// Prototype logics
#include "GameServer.h"
#include "ChatServer.h"

std::shared_ptr<LogicInterface> logic;

/**
* Handles sending stop signal to server.
*/
BOOL WINAPI ConsoleHandler(DWORD signal)
{
	switch (signal)
	{
		default:
		{
			std::cout << "[----] Shutdown event detected" << std::endl;
			logic->Stop();
		}
	}

	return TRUE;
}

/**
* Entry point into the program.
*/
int main()
{
	// Server type input and error flag
	bool error = false;
	int type = 0;

	// Take input values from console
	std::cout << "Select Logic:\n>> 0: Server\n>> 1: Client\n>> 2: GameServer Prototype\n3: ChatServer Prototype\n(Enter Option): ";
	std::cin >> type;

	// Identify type, give error if type does not exist
	switch (type)
	{
		case 0:
			std::cout << "-----------Starting ServerLogic-----------" << std::endl;
			logic = std::make_shared<ServerLogic>();
			break;
		case 1:
			std::cout << "-----------Starting ClientLogic-----------" << std::endl;
			logic = std::make_shared<ClientLogic>();
			break;
		case 2:
			std::cout << "-----------Starting GameServer-----------" << std::endl;
			logic = std::make_shared<GameServer>();
			break;
		case 3:
			std::cout << "-----------Starting ChatServer-----------" << std::endl;
			logic = std::make_shared<ChatServer>();
			break;
		default:
			std::cout << "Unknown logic case " << type << std::endl;
			error = true;
			break;
	}

	// Register termination signal handler
	if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE))
	{
		std::cout << "Unable to register console handler" << std::endl;
		error = true;
	}

	// Run the selected logic component
	if (!error)
	{
		logic->Run();
	}

	// For exit, gives some time to see output
	Sleep(5000);

	// End program
	return 0;
}
