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

void PlayerManager::RemovePlayer(std::string player_name)
{
	SOCKET s = name_to_player_list[player_name]->player_socket->GetSocket();

	std::cout << "[PlayerManager] RemovePlayer(by SOCKET): Removing player [" << player_name << "] on socket [" << s << "]." << std::endl;

	name_to_player_list.erase(player_name);
	socket_to_player_list.erase(s);
	
	std::cout << "s2p: " << socket_to_player_list.size() << ", n2p: " << name_to_player_list.size() << std::endl;
}

void PlayerManager::RemovePlayer(int id)
{
	std::string n = socket_to_player_list[id]->player_name;

	std::cout << "[PlayerManager] RemovePlayer(by SOCKET): Removing player [" << n << "] on socket [" << id << "]." << std::endl;

	socket_to_player_list.erase(id);
	name_to_player_list.erase(n);

	std::cout << "s2p: " << socket_to_player_list.size() << ", n2p: " << name_to_player_list.size() << std::endl;
}

std::map<std::string, std::shared_ptr<Player>> PlayerManager::GetPlayerList()
{
	return name_to_player_list;
}