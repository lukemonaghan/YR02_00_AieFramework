#include "NetworkedGameArchitectures.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

//--TUTORIAL

enum MESSAGE_ID
{
	ID_USER_NEW_CLIENT = ID_USER_PACKET_ENUM,
	ID_USER_CLIENT_DISCONNECTED,
	ID_USER_ID,

	ID_USER_CUSTOM_DATA,
	ID_USER_POSITION = ID_USER_CUSTOM_DATA
};

// simulate lost packets for a bad network
float packetLoss = 0.3f;
bool running = true;
const int SERVER_PORT = 12001;
const float SPEED = 5;

void BadServer(){
	// start server
	RakNet::RakPeerInterface* raknet = RakNet::RakPeerInterface::GetInstance();
	
	RakNet::SocketDescriptor desc(SERVER_PORT, nullptr);
	raknet->Startup(16, &desc, 1); 
	raknet->SetMaximumIncomingConnections(16);

	// store clients IDs
	int newClientID = 0;
	std::map<RakNet::SystemAddress,int> clientIDs;

	printf("SERVER : Server running...\n");

	while (running == true)
	{
		RakNet::Packet* packet = raknet->Receive();
		while (packet != nullptr)
		{
			switch (packet->data[0])
			{
			case ID_NEW_INCOMING_CONNECTION:
				{
					printf("SERVER : New client connected: %i\n", newClientID);	
					clientIDs[packet->systemAddress] = newClientID++;

					// tell the client his ID and all others connected
					RakNet::BitStream outStream;
					outStream.Write( (unsigned char)ID_USER_ID );
					outStream.Write( newClientID - 1 );
					for (auto& client : clientIDs)
						if (client.second != (newClientID - 1)) outStream.Write( client.second );
					raknet->Send(&outStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);

					// tell other clients
					RakNet::BitStream outStream2;
					outStream2.Write( (unsigned char)ID_USER_NEW_CLIENT );
					outStream2.Write( newClientID - 1 );
					raknet->Send(&outStream2, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, true);

					break;
				}
			case ID_DISCONNECTION_NOTIFICATION:
			case ID_CONNECTION_LOST:
				{
					printf("SERVER : Client disconnected: %i\n", clientIDs[packet->systemAddress]);	

					// tell clients they disconnected
					RakNet::BitStream outStream;
					outStream.Write( (unsigned char)ID_USER_CLIENT_DISCONNECTED );
					outStream.Write( clientIDs[packet->systemAddress] );
					raknet->Send(&outStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, true);

					clientIDs.erase( clientIDs.find( packet->systemAddress ) );

					break;
				}
			default:
				{
					if (packet->data[0] >= ID_USER_CUSTOM_DATA)
					{
						// just send the data on!
						if ((rand() / (float)RAND_MAX) >= packetLoss)
						{
							RakNet::BitStream outStream(packet->data, packet->length, false);
							raknet->Send(&outStream, MEDIUM_PRIORITY, UNRELIABLE, 0, packet->systemAddress, true);
						}
					}
				}
				break;
			};

			raknet->DeallocatePacket(packet);
			packet = raknet->Receive();
		}
	}

	raknet->Shutdown(1000);

	RakNet::RakPeerInterface::DestroyInstance(raknet);

	return;
}

//--TUTORIAL

NetworkedGameArchitectures::NetworkedGameArchitectures(){}
NetworkedGameArchitectures::~NetworkedGameArchitectures(){}

bool NetworkedGameArchitectures::onCreate(int a_argc, char* a_argv[]) {
	Gizmos::create();
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//--TUTORIAL
	char buffer[32];
	std::cin >> buffer;
	if (buffer[0] == 'y')
		m_badServer = std::thread(BadServer);

	m_peerInterface = RakNet::RakPeerInterface::GetInstance();

	RakNet::SocketDescriptor desc;
	m_peerInterface->Startup(1,&desc,1);
	m_peerInterface->Connect("localhost",SERVER_PORT,nullptr,0);

	m_id = -1;

	//--TUTORIAL

	return true;
}

void NetworkedGameArchitectures::onUpdate(float a_deltaTime) {
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );
	Gizmos::clear();
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );
	for ( int i = 0 ; i < 21 ; ++i ) {
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );	
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}

	//--TUTORIAL

	RakNet::Packet *packet = m_peerInterface->Receive();
	while (packet != nullptr){

		switch(packet->data[0]){
		case ID_CONNECTION_REQUEST_ACCEPTED: {
				printf("Connected \n");
				m_serverAddress = packet->systemAddress;
				break;
			}
		case ID_USER_ID: {
				RakNet::BitStream input(packet->data,packet->length,true);
				input.IgnoreBytes(1);

				input.Read(m_id);
				m_players[m_id] = glm::vec3(0);

				printf("My ID : %i\n",m_id);

				int id = -1;
				while(input.Read(id)){
					m_players[m_id] = glm::vec3(0);
				}
				break;
			}

		case ID_USER_NEW_CLIENT: {
				RakNet::BitStream input(packet->data,packet->length,true);
				input.IgnoreBytes(1);

				int id = -1;
				input.Read(id);
				m_players[id] = glm::vec3(0);
				printf("new player ID : %i\n",id);
				break;
			}

		case ID_USER_CLIENT_DISCONNECTED: {
				RakNet::BitStream input(packet->data,packet->length,true);
				input.IgnoreBytes(1);

				int id = -1;
				input.Read(id);
				printf("new player ID : %i\n",id);

				m_players.erase(m_players.find(id));
				break;
			}
		case ID_USER_POSITION: {
				RakNet::BitStream input(packet->data,packet->length,true);
				input.IgnoreBytes(1);
				int id = -1;
				input.Read(id);
				glm::vec3 pos(0);
				input.Read(pos);
				m_players[id] = pos;
				break;
			}
		};

		m_peerInterface->DeallocatePacket(packet);
		packet = m_peerInterface->Receive();
	}

	if (m_id != -1){
		glm::vec3 movement(0);
		if (glfwGetKey(m_window,GLFW_KEY_UP) == GLFW_PRESS)
			movement.z -= 1;
		if (glfwGetKey(m_window,GLFW_KEY_DOWN) == GLFW_PRESS)
			movement.z += 1;
		if (glfwGetKey(m_window,GLFW_KEY_LEFT) == GLFW_PRESS)
			movement.x -= 1;
		if (glfwGetKey(m_window,GLFW_KEY_RIGHT) == GLFW_PRESS)
			movement.x += 1;
		
		if (glm::length2(movement) > 0){
			m_players[m_id] += glm::normalize(movement) * a_deltaTime * SPEED;
			RakNet::BitStream output;
			output.Write((unsigned char)ID_USER_POSITION);
			output.Write(m_id);
			output.Write(m_players[m_id]);
			m_peerInterface->Send(&output,HIGH_PRIORITY,RELIABLE_ORDERED,0,m_serverAddress,false);
		}
	}

	for (auto child : m_players){
		Gizmos::addAABBFilled(child.second,glm::vec3(0.5f),glm::vec4(1,1,0,0));
	}

	//--TUTORIAL

	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void NetworkedGameArchitectures::onDraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	//--TUTORIAL

	//--TUTORIAL

	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void NetworkedGameArchitectures::onDestroy() {
	m_peerInterface->Shutdown(100);
	RakNet::RakPeerInterface::DestroyInstance(m_peerInterface);

	running = false;
	m_badServer.join();

	Gizmos::destroy();
}

int main(int argc, char* argv[]) {
	Application* app = new NetworkedGameArchitectures();
	if (app->create("AIE - 11_NetworkedGameArchitectures",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();

	delete app;
	return 0;
}


