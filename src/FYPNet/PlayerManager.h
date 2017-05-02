#ifndef PlayerManager_H
#define PlayerManager_H

#include <map>
#include "Player.h"
#include "Socket.h"

class PlayerManager {
public:
	PlayerManager();
	
	void AddPlayer(std::string name, SOCKET socket);

	std::shared_ptr<Player> GetPlayer(std::string player_name);
	std::shared_ptr<Player> GetPlayer(SOCKET socket);

	std::map<std::string, std::shared_ptr<Player>> GetPlayerList();

private:
	std::map<std::string, std::shared_ptr<Player>> name_to_player_list;
	std::map<SOCKET, std::shared_ptr<Player>> socket_to_player_list;

};

#endif
