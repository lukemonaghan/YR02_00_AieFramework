#ifndef __BiDirectional_H_
#define __BiDirectional_H_

#include "Application.h"
#include <glm/ext.hpp>
#include <FBXModel.h>
#include <Shader.h>


// Derived application class that wraps up all globals neatly
class BiDirectional : public Application {
public:
	FBXModel *m_spear;
	Shader *m_Shader;
	glm::vec4 m_lightDirection;

	BiDirectional(){};
	virtual ~BiDirectional(){};

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // __BiDirectional_H_