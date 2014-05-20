#ifndef __AdvancedAudioSystems_H_
#define __AdvancedAudioSystems_H_

#include "Application.h"
#include <glm/glm.hpp>

// Derived application class that wraps up all globals neatly
class AdvancedAudioSystems : public Application
{
public:

	AdvancedAudioSystems();
	virtual ~AdvancedAudioSystems();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // __AdvancedAudioSystems_H_