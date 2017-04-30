/**
* ServerLogic.h
*
* Author: Ashley Scott
* Date: 2016 - 2017
*
*/

#ifndef ServerLogic_H
#define ServerLogic_H

#include <memory>
#include <algorithm>
#include <iostream>

#include "json.hpp"
#include "SocketManager.h"
#include "PacketData.h"
#include "LogicInterface.h"

#pragma comment(lib, "Ws2_32.lib")

class ServerLogic : public LogicInterface {
public:
	ServerLogic();

	int Run();
	void Stop();

private:
	bool engine_running = true;

	std::shared_ptr<SocketManager> socket_manager;

};

#endif
