#ifndef __GPU_ParticleSystem_H_
#define __GPU_ParticleSystem_H_

#include "Application.h"
#include <glm/glm.hpp>

// Derived application class that wraps up all globals neatly
class GPU_ParticleSystem : public Application
{
public:

	GPU_ParticleSystem();
	virtual ~GPU_ParticleSystem();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
	glm::mat4	m_viewMatrix;
};

#endif // __GPU_ParticleSystem_H_