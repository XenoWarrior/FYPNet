#ifndef Player_H
#define Player_H

#include <memory>

#include "Socket.h"

class Player {
public:
	Player(std::string p);

	std::string player_name;
	std::string player_char;
	std::map<std::string, int> player_pos;
	std::shared_ptr<Socket> player_socket;

private:

};

#endif
