#ifndef ___14_AdvancedAudioSystemds_H_
#define ___14_AdvancedAudioSystemds_H_

#include "Application.h"
#include <glm/glm.hpp>

// Derived application class that wraps up all globals neatly
class _14_AdvancedAudioSystemds : public Application
{
public:

	_14_AdvancedAudioSystemds();
	virtual ~_14_AdvancedAudioSystemds();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // ___14_AdvancedAudioSystemds_H_