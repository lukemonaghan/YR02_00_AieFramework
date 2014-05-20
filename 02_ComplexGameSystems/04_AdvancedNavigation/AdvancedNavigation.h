#ifndef __AdvancedNavigation_H_
#define __AdvancedNavigation_H_

#include "Application.h"
#include <glm/glm.hpp>

#include <FBXFile.h>
#include <vector>

// Derived application class that wraps up all globals neatly
class AdvancedNavigation : public Application
{
public:

	AdvancedNavigation();
	virtual ~AdvancedNavigation();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	struct NavNodeTri{
		glm::vec3 position;
		glm::vec3 vertices[3];
		NavNodeTri* edgeTarget[3];
	};

	std::vector<NavNodeTri*> m_graph;

	void	buildNavMesh(FBXMeshNode* a_mesh,std::vector<NavNodeTri*>& a_graph);

	void	createOpenGLBuffers(FBXFile* a_fbx);
	void	cleanupOpenGLBuffers(FBXFile* a_fbx);

	struct GLData
	{
		unsigned int	vao, vbo, ibo;
	};

	FBXFile*	m_sponza;
	FBXFile*	m_navMesh;

	unsigned int	m_shader;

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // __AdvancedNavigation_H_