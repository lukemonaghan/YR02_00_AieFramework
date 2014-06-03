#ifndef ___16_SoftBodies_H_
#define ___16_SoftBodies_H_

#include "Application.h"
#include <glm/glm.hpp>
#include <PxPhysicsAPI.h>

// Derived application class that wraps up all globals neatly
class _16_SoftBodies : public Application {
public:

	_16_SoftBodies();
	virtual ~_16_SoftBodies();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
	
	//!- TUTORIAL
	physx::PxCloth* createCloth(const glm::vec3& a_position,unsigned int& a_vertexCount, unsigned int& a_indexCount,const glm::vec3* a_vertices,unsigned int* a_indices);
	physx::PxCloth* m_cloth;
	//!- TUTORIAL
};

#endif // ___16_SoftBodies_H_