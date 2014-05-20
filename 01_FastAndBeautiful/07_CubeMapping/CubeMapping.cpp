#include "CubeMapping.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

CubeMapping::CubeMapping(){}
CubeMapping::~CubeMapping(){}

bool CubeMapping::onCreate(int a_argc, char* a_argv[]) {
	Gizmos::create();

	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, Utility::Width/(float)Utility::Height, 0.1f, 1000.0f);

	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	m_Shader = new Shader("Shaders/07_CubeMap.vert","Shaders/07_CubeMap.frag");
	m_Shader->SetAttribs(1,0,"Position");
	m_SkyBox = new Skybox(10);
	m_SkyBox2 = new Skybox(1,0,false);
	m_CubeMap = new CubeMap("textures/CubeMap/",".png");

	return true;
}

void CubeMapping::onUpdate(float a_deltaTime) {
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );
	Gizmos::clear();
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );
	for ( int i = 0 ; i < 21 ; ++i ) {
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );	
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS) {quit();}
}

void CubeMapping::onDraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	m_SkyBox->Draw(m_CubeMap,m_Shader,(m_projectionMatrix * viewMatrix));
	m_SkyBox2->Draw(m_CubeMap,m_Shader,(m_projectionMatrix * viewMatrix));

	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void CubeMapping::onDestroy() {
	Gizmos::destroy();
}

int main(int argc, char* argv[]) {
	Application* app = new CubeMapping();
	if (app->create("AIE - 07_CubeMapping",Utility::Width,Utility::Height,argc,argv) == true) {
		app->run();
	}
	delete app;
	return 0;
}