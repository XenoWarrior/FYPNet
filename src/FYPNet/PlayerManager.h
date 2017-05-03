#ifndef PlayerManager_H
#define PlayerManager_H

#include <map>
#include "Player.h"
#include "Socket.h"

class PlayerManager {
public:
	PlayerManager();
	
	// Add a reference to a player by socket and name
	void AddPlayer(std::string name, SOCKET socket);

	// Get player by name or socket
	std::shared_ptr<Player> GetPlayer(std::string player_name);
	std::shared_ptr<Player> GetPlayer(SOCKET socket);

	// Delete player by name or socket
	void RemovePlayer(std::string player_name);
	void RemovePlayer(SOCKET socket);

	// Get all plyers
	std::map<std::string, std::shared_ptr<Player>> GetPlayerList();

private:
	std::map<std::string, std::shared_ptr<Player>> name_to_player_list;
	std::map<SOCKET, std::shared_ptr<Player>> socket_to_player_list;

};

#endif
