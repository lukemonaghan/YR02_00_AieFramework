#ifndef __HDRL_H_
#define __HDRL_H_

#include "Application.h"
#include <glm/glm.hpp>

#include <Shader.h>
#include <FBXModel.h>
#include <FrameBuffer.h>
#include <HUDItems.h>

// Derived application class that wraps up all globals neatly
class HDRL : public Application {
public:

	FrameBuffer *fbScene,*fbBrightPass,*fbBlur;
	Shader *sScene,*sBloomPass,*sBrightPass,*sScreen;
	FBXModel *fbxModel;
	HUDTexture *htScreen;

	HDRL(){};
	virtual ~HDRL(){};

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // __HDRL_H_