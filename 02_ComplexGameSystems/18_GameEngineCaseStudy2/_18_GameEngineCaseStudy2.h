#ifndef ___18_GameEngineCaseStudy2_H_
#define ___18_GameEngineCaseStudy2_H_

#include "Application.h"
#include <glm/glm.hpp>

// Derived application class that wraps up all globals neatly
class _18_GameEngineCaseStudy2 : public Application
{
public:

	_18_GameEngineCaseStudy2();
	virtual ~_18_GameEngineCaseStudy2();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // ___18_GameEngineCaseStudy2_H_