#ifndef ___15_GameEngineCaseStudy1_H_
#define ___15_GameEngineCaseStudy1_H_

#include "Application.h"
#include <glm/glm.hpp>

// Derived application class that wraps up all globals neatly
class _15_GameEngineCaseStudy1 : public Application
{
public:

	_15_GameEngineCaseStudy1();
	virtual ~_15_GameEngineCaseStudy1();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // ___15_GameEngineCaseStudy1_H_