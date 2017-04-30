#include "ServerLogic.h"
#include "ClientLogic.h"

// Lousy global variable, but for testing so it's all cool
std::shared_ptr<ServerLogic> logic = std::make_shared<ServerLogic>();

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
* Entry point into the program, here I decide which logic to run for testing.
*/
int main()
{
	bool error = false;
	if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE))
	{
		std::cout << "Unable to register console handler" << std::endl;
		error = true;
	}

	if (!error)
	{
		logic->Run();
	}

	return 0;
}