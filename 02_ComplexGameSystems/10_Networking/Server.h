#pragma once

#include <RakPeerInterface.h>
#include "PacketInformation.h"

class Server{
public:
	Server(unsigned int MAX_CONNECTIONS,unsigned short PORT);
	~Server(void);

	//Send a char* to the client
	void Send(RakNet::SystemAddress ADDRESS,unsigned char HEADER,char* outstream,PacketPriority PRIORITY = HIGH_PRIORITY,PacketReliability ORDER = RELIABLE_ORDERED,char packetChannel = 0,bool broadcast = true);

	//returns a list of activated commands
	returnlist Update(commandlist cl_list);

protected:
	unsigned short _PORT;
	unsigned short _MAX_CONNECTIONS;

	RakNet::Packet *_pPacket;

	RakNet::RakPeerInterface* _PeerInterface;
};

