#ifndef __Shadows_H_
#define __Shadows_H_

#include "Application.h"
#include <glm/ext.hpp>
#include "Shadow.h"
#include <Plane.h>
#include <Texture.h>



// Derived application class that wraps up all globals neatly
class Shadows : public Application {
public:
	Shadow *sShadowClass;
	Shader *shdrScene,*shdrCam,*shdrDepth;
	Plane *pFloor;
	Texture *tFloor;

	Shadows(){};
	virtual ~Shadows(){};

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // __Shadows_H_