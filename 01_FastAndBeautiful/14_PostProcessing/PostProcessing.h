#ifndef __PostProcessing_H_
#define __PostProcessing_H_

#include "Application.h"
#include <glm/ext.hpp>

#include <FBXModel.h>
#include <Shader.h>
#include <FrameBuffer.h>
#include <HUDItems.h>

// Derived application class that wraps up all globals neatly
class PostProcessing : public Application
{
public:
	FBXModel *m_spear;
	Shader *m_Shader,*m_ShaderScreen;
	HUDTexture *htScreen;
	FrameBuffer *fbPostProcessing;

	int ShaderType,MaxTypes, count;
	glm::mat4 *Matricies;

	PostProcessing(){};
	virtual ~PostProcessing(){};
protected:

	unsigned int m_quadVAO,m_quadVBO;

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // __PostProcessing_H_