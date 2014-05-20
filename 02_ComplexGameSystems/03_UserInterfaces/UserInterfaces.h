#ifndef _UserInterfaces_H_
#define _UserInterfaces_H_

#include "Application.h"
#include <glm/glm.hpp>
#include "UIRectangle.h"

// Derived application class that wraps up all globals neatly
class UserInterfaces : public Application
{
public:

	UserInterfaces();
	virtual ~UserInterfaces();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
	glm::mat4	m_cameraProjectionMatrix;

	unsigned int m_guishader;
	unsigned int m_texture;

	UIRectangle* m_button;
};

#endif // ___03_UserInterfaces_H_