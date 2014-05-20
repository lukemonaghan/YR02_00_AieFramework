#ifndef __Threading_H_
#define __Threading_H_

#include "Application.h"
#include <glm/glm.hpp>

#include <FBXFile.h>
#include <thread>
#include <mutex>

// Derived application class that wraps up all globals neatly
class Threading : public Application {
public:

	Threading();
	virtual ~Threading();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	void createOpenGLBuffers(FBXFile *a_fbx);
	void cleanupOpenGLBuffers(FBXFile *a_fbx);

	struct GLData
	{
		unsigned int vao,vbo,ibo;
	};

	FBXFile		*m_fbx;
	std::thread	*m_loadingThread;
	bool		m_loadingComplete;
	bool		m_ready;
	unsigned int m_shader;

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // __Threading_H_