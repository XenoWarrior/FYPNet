#ifndef ZoneServer_H
#define ZoneServer_H

#include <memory>

#include "SocketManager.h"
#include "PlayerManager.h"
#include "PacketData.h"

#include "LogicInterface.h"

class ZoneServer : public LogicInterface {
public:
	ZoneServer();

	int Run();
	void Stop();

private:
	bool engine_running = true;

	std::shared_ptr<SocketManager> socket_manager;
	std::shared_ptr<PlayerManager> player_manager;
};

#endif
