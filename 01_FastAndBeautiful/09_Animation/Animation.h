#ifndef __Animation_H_
#define __Animation_H_

#include "Application.h"
#include <glm/ext.hpp>
#include <Shader.h>
#include <FBXModel.h>

// Derived application class that wraps up all globals neatly
class Animation : public Application {
public:
	Shader *s09Default;
	FBXModel *fbxModel; 

	Animation(){};
	virtual ~Animation(){};

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // __Animation_H_