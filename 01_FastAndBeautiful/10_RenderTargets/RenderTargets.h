#ifndef __RenderTargets_H_
#define __RenderTargets_H_

#include "Application.h"
#include "RenderTarget.h"
#include <glm/ext.hpp>
#include "FrameBuffer.h"
#include <FBXModel.h>
#include <Plane.h>
#include <Shader.h>

// Derived application class that wraps up all globals neatly
class RenderTargets : public Application {
public:
	RenderTarget *fbTexture;
	FBXModel *fbxm_SoulSpear;
	Plane *p_Floor;
	Shader *s_FBX,*s_Plane;

	RenderTargets(){};
	virtual ~RenderTargets(){};

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // __RenderTargets_H_