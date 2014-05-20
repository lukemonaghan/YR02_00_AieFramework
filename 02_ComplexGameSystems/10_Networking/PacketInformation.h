#pragma once

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <vector>

struct PacketInformation{
	int ID;
	RakNet::SystemAddress ADDRESS;
	char* information;
};

typedef std::vector<int> commandlist;
typedef std::vector<PacketInformation> returnlist;

const unsigned char STARTID = ID_USER_PACKET_ENUM;