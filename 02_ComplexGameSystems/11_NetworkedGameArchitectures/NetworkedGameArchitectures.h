#ifndef __NetworkedGameArchitectures_H_
#define __NetworkedGameArchitectures_H_

#include "Application.h"
#include <glm/glm.hpp>

//--TUTORIAL

#include <thread>
#include <RakPeerInterface.h>
#include <BitStream.h>
#include <MessageIdentifiers.h>
#include <stdio.h>
#include <map>

//--TUTORIAL

// Derived application class that wraps up all globals neatly
class NetworkedGameArchitectures : public Application {
public:

	NetworkedGameArchitectures();
	virtual ~NetworkedGameArchitectures();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;

	//--TUTORIAL

	std::thread					m_badServer;

	int							m_id;
	std::map<int,glm::vec3>		m_players;

	RakNet::SystemAddress		m_serverAddress;
	RakNet::RakPeerInterface	*m_peerInterface;

	//--TUTORIAL

};

#endif // __NetworkedGameArchitectures_H_