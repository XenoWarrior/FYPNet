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
	FYP_ON_CONNECT,			// 0
	FYP_ON_MESSAGE,			// 1
	FYP_ON_INVALID_PACKET,	// 2
	FYP_ON_DISCONNECT,		// 3

	// GameServer Prototype
	FYPGP_ON_LOGIN,			// 4
	FYPGP_ON_CREATECHAR,	// 5
	FYPGP_ON_GETCHARS,		// 6

	// ChatServer Prototype
	FYPCP_ON_ACCEPT,        // 7
	FYPCP_ON_MESSAGE,       // 8
	FYPCP_ON_NEWUSER        // 9
};

#endif
