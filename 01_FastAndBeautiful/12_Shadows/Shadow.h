#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <Shader.h>
#include <FBXModel.h>

class Shadow {
public:
	Shadow(char* modelpath,FBXFile::UNIT_SCALE a_Scale = FBXFile::UNITS_CENTIMETER);
	~Shadow(void);
	void setUpLightAndShadowMatrix(Shader* s_DepthShader);
	void create2DQuad(int W,int H);
	void displayShadowMap(Shader* s_Shader);
	void drawScene(Shader* s_Shader,int W,int H,glm::mat4 &m_cameraMatrix,glm::mat4& m_projectionMatrix);

	unsigned int m_shadowTexture;
protected:
	FBXFile *m_file;
	struct GLData{
		unsigned int vbo;
		unsigned int vao;
	}m_2dQuad;
	glm::vec4 m_lightDirection;
	glm::mat4 m_shadowProjectionViewMatrix;
	int m_shadowSize;
	unsigned int m_shadowFramebuffer;
};

