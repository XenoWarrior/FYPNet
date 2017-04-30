#ifndef ClientLogic_H
#define ClientLogic_H

#include <memory>
#include <algorithm>
#include <iostream>

#include "json.hpp"
#include "SocketManager.h"
#include "PacketData.h"

#pragma comment(lib, "Ws2_32.lib")

class ClientLogic {
public:
	ClientLogic();

	int Run();

private:
	bool engine_running = true;

};

#endif