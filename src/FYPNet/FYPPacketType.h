/**
* FYPPacketType.h
*
* Author: Ashley Scott
* Date: 2016 - 2017
*
*/

#ifndef FYPPacketType_H
#define FYPPacketType_H

// Different response identifiers
enum FYPPacketType {
    // FYPNet Core
    FYP_ON_CONNECT,         // 0
    FYP_ON_MESSAGE,         // 1
    FYP_ON_INVALID_PACKET,  // 2
    FYP_ON_DISCONNECT,      // 3

	// ChatServer Prototype
	FYPCP_ON_ACCEPT,        // 4
	FYPCP_ON_MESSAGE,       // 5
	FYPCP_ON_NEWUSER,       // 6

    // GameServer Prototype
    FYPGP_ON_LOGIN,         // 7
    FYPGP_ON_CREATECHAR,    // 8
    FYPGP_ON_GETCHARS,      // 9
	FYPGP_ON_GETZONES,      // 10
	FYPGP_ON_UPDATEPOS,     // 11
	FYPGP_ON_JOINZONE,		// 12
	FYPGP_ON_ZONEACCEPT     // 13
};

#endif
