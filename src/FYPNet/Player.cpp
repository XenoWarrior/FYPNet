#include "Player.h"

Player::Player(std::string p)
{
	std::map<std::string, int> pos;
	pos["x"] = 0;
	pos["y"] = 0;

	player_name = p;
	player_pos = pos;
	player_char = "0";
}