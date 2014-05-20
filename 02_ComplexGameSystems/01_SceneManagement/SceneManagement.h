#ifndef _SceneManagement_H_
#define _SceneManagement_H_

#include "Application.h"
#include <glm/glm.hpp>

// Derived application class that wraps up all globals neatly
class SceneManagement : public Application
{
public:

	SceneManagement();
	virtual ~SceneManagement();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	int		planeSphereTest(const glm::vec4& a_plane,const glm::vec3& a_center,float a_radius);

	void	getFrustrumPlanes(const glm::mat4& a_transform,glm::vec4* a_planes);

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // ___01_SceneManagement_H_