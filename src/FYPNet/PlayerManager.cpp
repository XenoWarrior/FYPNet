#include "PlayerManager.h"

PlayerManager::PlayerManager()
{

}

void PlayerManager::AddPlayer(std::string name, SOCKET socket)
{
	std::shared_ptr<Player> p = std::make_shared<Player>(name);
	name_to_player_list[name] = p;
	socket_to_player_list[socket] = p;
}

std::shared_ptr<Player> PlayerManager::GetPlayer(std::string player_name)
{
	return name_to_player_list[player_name];
}

std::shared_ptr<Player> PlayerManager::GetPlayer(SOCKET socket)
{
	return socket_to_player_list[socket];
}

std::map<std::string, std::shared_ptr<Player>> PlayerManager::GetPlayerList()
{
	return name_to_player_list;
}