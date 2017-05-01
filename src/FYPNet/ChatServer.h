#ifndef ChatServer_H
#define ChatServer_H

#include <memory>

#include "SocketManager.h"
#include "PacketData.h"

#include "LogicInterface.h"

class ChatServer : public LogicInterface {
public:
	ChatServer();

	int Run();
	void Stop();

private:
	bool engine_running = true;

	std::shared_ptr<SocketManager> socket_manager;
};

#endif