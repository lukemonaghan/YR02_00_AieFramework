#ifndef __FBX_H_
#define __FBX_H_

#include "Application.h"
#include <glm/ext.hpp>

#include "Texture.h"
#include "Shader.h"
#include "FBXModel.h"
#include "SpotLight.h"

// Derived application class that wraps up all globals neatly
class FBX : public Application {
public:
	FBXModel* m_fbxModel;
	Shader* ShaderDefault;

	SpotLight* Light;

	FBX(){};
	virtual ~FBX(){};

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
	glm::mat4	viewMatrix;



};

#endif // __FBX_H_