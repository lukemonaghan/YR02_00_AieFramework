#ifndef __VolumeRendering_H_
#define __VolumeRendering_H_

#include "Application.h"
#include <glm/glm.hpp>
#include "MarchingCubes.h"
#include <Shader.h>

// Derived application class that wraps up all globals neatly
class VolumeRendering : public Application
{
public:

	VolumeRendering(){};
	virtual ~VolumeRendering(){};

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	MarchingCubes		m_marcher;

	// volume data
	float***			m_volume;
	glm::ivec3			m_gridSize;
	glm::vec3			m_cubeSize;
	float				m_volumeThreshold;

	// blob particles
	unsigned int		m_blobCount;
	glm::vec3*			m_blobPoints;
	glm::vec3*			m_blobVelocities;

	// vertices
	glm::vec3*			m_vertices;
	unsigned int		m_maxVertices;

	// fluid colour and position
	glm::vec4			m_fluidColour;
	glm::mat4			m_fluidTransform;
	
	// opengl handles
	unsigned int		m_vao, m_vbo;

	Shader				*sWorld;

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // __VolumeRendering_H_