#ifndef __GlobalIllumination_H_
#define __GlobalIllumination_H_

#include "Application.h"
#include <glm/glm.hpp>

#include <Shader.h>
#include <FBXModel.h>
#include <FrameBuffer.h>
#include <HUDItems.h>
#include <Texture.h>

// Derived application class that wraps up all globals neatly
class GlobalIllumination : public Application {
public:
	FrameBuffer *fbScene;
	FBXModel *fbxModel;
	Shader *sScene,*sScreen;
	HUDTexture *htScreen;
	Texture *tRandom;

	GlobalIllumination(){};
	virtual ~GlobalIllumination(){};

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // __GlobalIllumination_H_