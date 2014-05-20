#ifndef __Networking_H_
#define __Networking_H_

#include "Application.h"
#include <glm/glm.hpp>

// Derived application class that wraps up all globals neatly
class Networking : public Application
{
public:

	Networking();
	virtual ~Networking();

	void MainLoop();
	void ServerLoop();
	void ClientLoop();

	void MainDraw();
	void ServerDraw();
	void ClientDraw();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // ___10_Networking_H_