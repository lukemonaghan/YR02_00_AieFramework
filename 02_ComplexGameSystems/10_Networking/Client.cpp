#include "Client.h"

#include <BitStream.h>

#include <iostream>
#include <Utilities.h>

Client::Client(unsigned short PORT, const char* IP) {
	_IP = (char*)IP;
	_PORT = PORT;
	_fTIMEOUT = 1.0f;
	_bActive = false;
	_pPacket = nullptr;

	_PeerInterface = RakNet::RakPeerInterface::GetInstance();
	RakNet::SocketDescriptor socketDescriptor;
	_PeerInterface->Startup(1,&socketDescriptor,1);
	_PeerInterface->Connect(IP,PORT,0,0);
}

Client::~Client(void) {
	_PeerInterface->Shutdown(1000);
	RakNet::RakPeerInterface::DestroyInstance(_PeerInterface);
}

void Client::Send(char* outstream,unsigned char HEADER,PacketPriority PRIORITY,PacketReliability ORDER,char packetChannel,bool broadcast){
	RakNet::BitStream outStream;
	outStream.Write(HEADER);
	outStream.Write(outstream);
	_PeerInterface->Send(&outStream,PRIORITY,ORDER,packetChannel,_ServerAddress,broadcast);
}

bool Client::Failed(){
	return (!_bActive && _fTIMEOUT <= 0.0f);
}

returnlist Client::Update(commandlist cl_list){
	returnlist returnlist;

	if (!_bActive && _fTIMEOUT >= 0.0f){
		_fTIMEOUT -= Utility::getDeltaTime();
	}else if (!_bActive && _fTIMEOUT <= 0.0f){
		static bool bTold = false;
		if (!bTold){
			bTold = true;
			printf("Connected to server failed : %s\n",_IP);
		}
	}
	
	for (_pPacket = _PeerInterface->Receive(); _pPacket; _PeerInterface->DeallocatePacket(_pPacket), _pPacket = _PeerInterface->Receive()) {
		for (commandlist::iterator it = cl_list.begin(); it != cl_list.end(); it++){
			if (_pPacket->data[0] == (*it)){
				RakNet::BitStream inputStream(_pPacket->data,_pPacket->length,true);
				inputStream.IgnoreBytes(1);

				RakNet::RakString inputString;
				inputStream.Read(inputString);

				PacketInformation PI;
				PI.ADDRESS = _ServerAddress;
				PI.ID = (*it);
				PI.information = (char*)inputString.C_String();
				returnlist.push_back(PI);
				continue;
			}
			switch(_pPacket->data[0]){
				case ID_CONNECTION_REQUEST_ACCEPTED:{
					RakNet::BitStream inputStream(_pPacket->data,_pPacket->length,true);
					inputStream.IgnoreBytes(1);

					RakNet::RakString inputString;
					inputStream.Read(inputString);

					_ServerAddress = _pPacket->systemAddress;
					_CLIENTID = (int)inputString.C_String();
					_bActive = true;
					printf("CLIENT: Connected to server IP : %s\n",_IP);
				break;
				}
				case ID_NEW_INCOMING_CONNECTION:
					printf("CLIENT: A connection is incoming.\n");
				break;
				case ID_NO_FREE_INCOMING_CONNECTIONS:
					printf("CLIENT: The server is full.\n");
					break;
				case ID_DISCONNECTION_NOTIFICATION:
					printf("CLIENT: Server disconnected.\n");
					break;
				case ID_CONNECTION_LOST:
					printf("CLIENT: Lost connection.\n");
					break;
			}
		}
		
	}
	return returnlist;
}
