#ifndef __Other_H_
#define __Other_H_

#include "Application.h"
#include <glm/glm.hpp>
#include <Shader.h>

// Derived application class that wraps up all globals neatly
class Other : public Application
{
public:

	Other();
	virtual ~Other();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
	
	//!--TUTORIAL

	Shader *sScene;

	//!--TUTORIAL
};

#endif // __Other_H_