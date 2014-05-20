#ifndef __DeferredRendering_H_
#define __DeferredRendering_H_

#include "Application.h"
#include <glm/ext.hpp>
#include <FrameBuffer.h>
#include <FBXModel.h>
#include <Shader.h>
#include <HUDItems.h>

// Derived application class that wraps up all globals neatly
class DeferredRendering : public Application{
public:

	FrameBuffer *fbBuff;
	FBXModel *fbxmModel;
	Shader * sShader,*s_Plane;
	HUDTexture *HuDTex;

	DeferredRendering(){};
	virtual ~DeferredRendering(){};

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // __DeferredRendering_H_