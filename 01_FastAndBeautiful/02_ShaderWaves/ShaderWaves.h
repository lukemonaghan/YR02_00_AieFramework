#ifndef __ShaderWaves_H_
#define __ShaderWaves_H_

#include "Application.h"	//Application inheritance class
#include <glm/glm.hpp>		//glm::mat4

#include "Shader.h"			//Shader class
#include "Plane.h"			//Plane class
#include "Texture.h"		//Texture Class

// Derived application class that wraps up all globals neatly
class ShaderWaves : public Application {
public:

	ShaderWaves();
	virtual ~ShaderWaves();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
	glm::mat4	m_viewMatrix;

	float fHeight;

	Plane *WaterPlane,*SandPlane;
	Shader *WaterShader,*TextureShader;
	Texture *WaterTexture,*SandTexture;
};

#endif // __ShaderWaves_H_