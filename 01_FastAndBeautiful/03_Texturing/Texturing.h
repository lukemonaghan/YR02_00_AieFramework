#ifndef __Texturing_H_
#define __Texturing_H_

#include "Application.h"
#include <glm/glm.hpp>

#include <Shader.h>
#include <Texture.h>
#include <Plane.h>

// Derived application class that wraps up all globals neatly
class Texturing : public Application
{
public:
	Shader* MyShader;
	Texture* MyTexture;
	Plane *pFloor;

	Texturing();
	virtual ~Texturing();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // __Texturing_H_