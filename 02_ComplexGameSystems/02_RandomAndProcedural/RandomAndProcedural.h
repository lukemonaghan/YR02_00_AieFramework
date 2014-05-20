#ifndef _RandomAndProcedural_H_
#define _RandomAndProcedural_H_

#include "Application.h"
#include <glm/glm.hpp>

// Derived application class that wraps up all globals neatly
class RandomAndProcedural : public Application
{
public:

	RandomAndProcedural();
	virtual ~RandomAndProcedural();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::ivec2		m_dimentions;
	float			*m_height;

	unsigned int	m_texture;

	unsigned int	m_vao,m_vbo,m_ibo;
	unsigned int	m_shader;

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // ___02_RandomAndProcedural_H_