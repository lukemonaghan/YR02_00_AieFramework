#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

class RenderTarget
{
public:
	RenderTarget(int Width,int Height);
	~RenderTarget(void);
	void StartDraw();
	void EndDraw();
	unsigned int GetTexture();
	static int NORMAL_FBO;
protected:
	unsigned int m_FBO,m_FBO_texture,m_FBO_depthtexture;
};

