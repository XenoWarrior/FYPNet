/**
* ClientLogic.h
*
* Author: Ashley Scott
* Date: 2016 - 2017
*
*/

#ifndef ClientLogic_H
#define ClientLogic_H

#include <memory>
#include <algorithm>
#include <iostream>

#include "json.hpp"
#include "SocketManager.h"
#include "PacketData.h"
#include "LogicInterface.h"

#pragma comment(lib, "Ws2_32.lib")

class ClientLogic : public LogicInterface {
public:
	ClientLogic();

	int Run();
	void Stop();

private:
	bool engine_running = true;

	std::shared_ptr<SocketManager> socket_manager;

};

#endif
