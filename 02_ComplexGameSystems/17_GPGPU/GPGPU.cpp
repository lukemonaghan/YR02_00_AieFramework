#include "GPGPU.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

//--TUTORIAL
#include <windows.h>
#include <thread>
#include <vector>

#define CL_CHECK(result) if (result != CL_SUCCESS) { printf("Failed %i\n",result); return false;}

//--TUTORIAL

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

GPGPU::GPGPU(){}

GPGPU::~GPGPU(){}

bool GPGPU::onCreate(int a_argc, char* a_argv[]) {
	Gizmos::create();
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//--TUTORIAL

#ifdef DEBUG
	printf("DEBUG MODE\n");
#else
	printf("RELEASE MODE\n");
#endif

	// Autofill
	for (auto &vec : m_vec4){
		vec = glm::vec4(1,1,1,1);
	}
	
	// CPU test
	double t = glfwGetTime();
	for (auto &vec : m_vec4){
		vec = glm::normalize(vec);
	}
	printf("CPU : %f \n",(float)(glfwGetTime() - t));

	// OpenCL setup
	cl_int result;
	result = clGetPlatformIDs(1,&m_platform,nullptr);
	CL_CHECK(result);

	result = clGetDeviceIDs(m_platform,CL_DEVICE_TYPE_GPU,1,&m_device,nullptr);
	CL_CHECK(result);
	
	cl_context_properties contextProperties[] = {
		CL_GL_CONTEXT_KHR,(cl_context_properties)wglGetCurrentContext(),
		CL_WGL_HDC_KHR,(cl_context_properties)wglGetCurrentDC(),
		CL_CONTEXT_PLATFORM,(cl_context_properties)m_platform,
		0, 0,
	};

	m_context = clCreateContext(contextProperties,1,&m_device,nullptr,nullptr,&result);
	CL_CHECK(result);

	m_queue = clCreateCommandQueue(m_context, m_device, CL_QUEUE_PROFILING_ENABLE,&result);
	CL_CHECK(result);

	const char* kernelSource = "__kernel void kernelMain(__global float4* a_vectors)\n"
		"{\n"
		"int i = get_global_id(0);\n"
		"a_vectors[i] = normalize(a_vectors[i]);\n"
		"}";

	unsigned int size = strlen(kernelSource);

	m_program = clCreateProgramWithSource(m_context,1,&kernelSource,(const size_t*)&size,&result);
	CL_CHECK(result);

	result = clBuildProgram(m_program,1,&m_device,nullptr,nullptr,nullptr);
	CL_CHECK(result);

	m_kernel = clCreateKernel(m_program,"kernelMain",&result);
	CL_CHECK(result);

	m_buffer = clCreateBuffer(m_context,CL_MEM_READ_WRITE,sizeof(glm::vec4) * VECTOR_COUNT, nullptr, &result);
	CL_CHECK(result);

	result = clEnqueueWriteBuffer(m_queue,m_buffer,CL_TRUE,0,sizeof(glm::vec4) * VECTOR_COUNT, m_vec4,0, nullptr,nullptr);
	CL_CHECK(result);

	result = clSetKernelArg(m_kernel,0,sizeof(cl_mem),(void*)&m_buffer);
	CL_CHECK(result);

	unsigned int globalWorldSize = VECTOR_COUNT;
	cl_event eventID = nullptr;
	result = clEnqueueNDRangeKernel(m_queue,m_kernel,1,nullptr,&globalWorldSize,nullptr,0,nullptr,&eventID);
	CL_CHECK(result);

	result = clWaitForEvents(1,&eventID);
	CL_CHECK(result);

	result = clEnqueueReadBuffer(m_queue,m_buffer,CL_TRUE,0,sizeof(glm::vec4) * VECTOR_COUNT, m_vec4, 0, nullptr, nullptr);
	CL_CHECK(result);

	// GPU timer
	cl_ulong startTime = 0;
	cl_ulong endTime = 0;
	result = clGetEventProfilingInfo(eventID,CL_PROFILING_COMMAND_START,sizeof(cl_ulong),&startTime,nullptr);
	CL_CHECK(result);
	result = clGetEventProfilingInfo(eventID,CL_PROFILING_COMMAND_END,sizeof(cl_ulong),&endTime,nullptr);
	CL_CHECK(result);

	double clTime = (endTime - startTime) * 0.000000001;

	printf("GPU : %f \n",(float)clTime);
	//--TUTORIAL

	return true;
}

void GPGPU::onUpdate(float a_deltaTime) {
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );
	Gizmos::clear();
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );
	for ( int i = 0 ; i < 21 ; ++i ){
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}

	//--TUTORIAL

	//--TUTORIAL

	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void GPGPU::onDraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	//--TUTORIAL

	//--TUTORIAL

	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void GPGPU::onDestroy() {
	clFlush(m_queue);
	clFinish(m_queue);
	clReleaseKernel(m_kernel);
	clReleaseProgram(m_program);
	clReleaseMemObject(m_buffer);
	clReleaseCommandQueue(m_queue);
	clReleaseContext(m_context);
	Gizmos::destroy();
}

int main(int argc, char* argv[]) {
	Application* app = new GPGPU();
	if (app->create("AIE - 17_GPGPU",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();

	delete app;
	return 0;
}