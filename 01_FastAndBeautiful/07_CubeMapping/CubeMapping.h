#ifndef __CubeMapping_H_
#define __CubeMapping_H_

#include "Application.h"
#include <glm/glm.hpp>

#include <skybox.h>
#include <Shader.h>

// Derived application class that wraps up all globals neatly
class CubeMapping : public Application
{
public:
	Skybox *m_SkyBox,*m_SkyBox2;
	CubeMap *m_CubeMap;
	Shader *m_Shader;

	CubeMapping();
	virtual ~CubeMapping();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // __CubeMapping_H_