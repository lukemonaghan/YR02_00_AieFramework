#ifndef ___01_Introduction_H_
#define ___01_Introduction_H_

#include "Application.h"
#include <glm\glm.hpp>

// Derived application class that wraps up all globals neatly
class _01_Introduction : public Application
{
public:

	_01_Introduction();
	virtual ~_01_Introduction();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;

};

#endif // ___01_Introduction_H_