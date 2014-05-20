#include "Other.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include "VertexBatch.hpp"

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

Other::Other(){}
Other::~Other(){}

Osiris::VertexBatch *vb;

bool Other::onCreate(int a_argc, char* a_argv[]) {
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	//!--TUTORIAL
	vb = new Osiris::VertexBatch();
	vb->Add(&Osiris::Gizmo::BoxSingleFace(glm::vec3(0),glm::vec3(1),glm::vec4(1,0,0,1)));
	vb->Add(&Osiris::Gizmo::Plane(glm::vec3(0),glm::vec2(100),glm::vec4(1,1,1,1)));
	vb->Update();
	//!--TUTORIAL
	return true;
}

void Other::onUpdate(float a_deltaTime) {
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );
	//!--TUTORIAL

	//!--TUTORIAL
	
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		quit();
	}
}

void Other::onDraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	//!--TUTORIAL
	vb->Draw(viewMatrix,m_projectionMatrix);
	//!--TUTORIAL
	
}

void Other::onDestroy() {
	//!--TUTORIAL

	//!--TUTORIAL
}

int main(int argc, char* argv[]) {
	Application* app = new Other();
	
	if (app->create("AIE - Other",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true) {
		app->run();
	}
	delete app;
	return 0;
}

