#ifndef GameServer_H
#define GameServer_H

#include <memory>

#include "SocketManager.h"
#include "PacketData.h"

#include "LogicInterface.h"

class GameServer : public LogicInterface {
public:
	GameServer();

	int Run();
	void Stop();

private:
	bool engine_running = true;

	std::shared_ptr<SocketManager> socket_manager;
};

#endif