#pragma once

#include <RakPeerInterface.h>
#include "PacketInformation.h"

class Client {
public:
	Client(unsigned short PORT, const char* IP = "127.0.0.1");
	~Client(void);

	//Send a char* to the server
	void Send(char* outstream,unsigned char HEADER,PacketPriority PRIORITY = HIGH_PRIORITY,PacketReliability ORDER = RELIABLE_ORDERED,char packetChannel = 0,bool broadcast = false);

	bool Failed();

	//returns a list of activated commands
	returnlist Update(commandlist cl_list);
	
protected:

	RakNet::RakPeerInterface* _PeerInterface;
	RakNet::SystemAddress _ServerAddress;

	unsigned short _PORT;
	char* _IP;
	bool _bActive;
	float _fTIMEOUT;
	int _CLIENTID;

	RakNet::Packet *_pPacket;
};

