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
	FYP_ON_CONNECT,
	FYP_ON_MESSAGE,
	FYP_ON_INVALID_PACKET,
	FYP_ON_DISCONNECT
};

#endif
