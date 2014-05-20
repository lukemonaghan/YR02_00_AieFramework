#include "Server.h"

#include <BitStream.h>

#include <iostream>
#include <Utilities.h>

Server::Server(unsigned int MAX_CONNECTIONS,unsigned short PORT) {
	_PeerInterface = RakNet::RakPeerInterface::GetInstance();
	_PORT = PORT;
	_MAX_CONNECTIONS = MAX_CONNECTIONS;

	RakNet::SocketDescriptor socketDescriptor(_PORT,0);
	_PeerInterface->Startup(_MAX_CONNECTIONS,&socketDescriptor,1);
	_PeerInterface->SetMaximumIncomingConnections(_MAX_CONNECTIONS);

	printf("server started \n");
}


Server::~Server(void){
	_PeerInterface->Shutdown(1000);
	RakNet::RakPeerInterface::DestroyInstance(_PeerInterface);
}

void Server::Send(RakNet::SystemAddress ADDRESS,unsigned char HEADER,char* outstream,PacketPriority PRIORITY,PacketReliability ORDER,char packetChannel,bool broadcast) {
	RakNet::BitStream outStream;
	outStream.Write(HEADER);
	outStream.Write(outstream);
	_PeerInterface->Send(&outStream,PRIORITY,ORDER,packetChannel,ADDRESS,broadcast);
}

returnlist Server::Update(commandlist cl_list){
	returnlist returnlist;

	for (_pPacket = _PeerInterface->Receive(); _pPacket; _PeerInterface->DeallocatePacket(_pPacket), _pPacket = _PeerInterface->Receive()) {
		for (commandlist::iterator it = cl_list.begin(); it != cl_list.end(); it++){
			if (_pPacket->data[0] == (*it)){
				RakNet::BitStream inputStream(_pPacket->data,_pPacket->length,true);
				inputStream.IgnoreBytes(1);

				RakNet::RakString inputString;
				inputStream.Read(inputString);

				PacketInformation PI;
				PI.ADDRESS = _pPacket->systemAddress;
				PI.ID = (*it);
				PI.information = (char*)inputString.C_String();
				returnlist.push_back(PI);
				continue;
			}
			switch(_pPacket->data[0]){
				case ID_REMOTE_DISCONNECTION_NOTIFICATION:
					printf("SERVER: A client has disconnected.\n");
					break;
				case ID_REMOTE_CONNECTION_LOST:
					printf("SERVER: A client has lost the connection.\n");
					break;
				case ID_REMOTE_NEW_INCOMING_CONNECTION:
					printf("SERVER: A client has connected.\n");
					break;
				case ID_NEW_INCOMING_CONNECTION:
					printf("SERVER: A connection is incoming.\n");
				break;
				case ID_NO_FREE_INCOMING_CONNECTIONS:
					printf("SERVER: The server is full.\n");
					break;
				case ID_DISCONNECTION_NOTIFICATION:
					printf("SERVER: A client has disconnected.\n");
					break;
				case ID_CONNECTION_LOST:
					printf("SERVER: A client lost the connection.\n");
					break;
			}
		}
	}

	return returnlist;
}

