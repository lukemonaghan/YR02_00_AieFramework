#ifndef ___02_PhysicsAPI_H_
#define ___02_PhysicsAPI_H_

#include "Application.h"
#include <glm/glm.hpp>

// Derived application class that wraps up all globals neatly
class _02_PhysicsAPI : public Application
{
public:

	_02_PhysicsAPI();
	virtual ~_02_PhysicsAPI();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
	
	//!- TUTORIAL
	
	//!- TUTORIAL
};

#endif // ___02_PhysicsAPI_H_