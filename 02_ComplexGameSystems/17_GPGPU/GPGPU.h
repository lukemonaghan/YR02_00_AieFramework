#ifndef __GPGPU_H_
#define __GPGPU_H_

#include "Application.h"
#include <glm/glm.hpp>

#include <CL/opencl.h>

// Derived application class that wraps up all globals neatly
class GPGPU : public Application {
public:

	GPGPU();
	virtual ~GPGPU();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	static const int	VECTOR_COUNT = 50000000;
	glm::vec4			m_vec4[VECTOR_COUNT];

	cl_platform_id		m_platform;
	cl_device_id		m_device;

	cl_context			m_context;
	cl_command_queue	m_queue;
	cl_program			m_program;
	cl_kernel			m_kernel;
	cl_mem				m_buffer;

	glm::mat4			m_cameraMatrix;
	glm::mat4			m_projectionMatrix;
};

#endif // ___17_GPGPU_H_