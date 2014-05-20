#include "GameGenreCaseStudy1.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

GameGenreCaseStudy1::GameGenreCaseStudy1() {}

GameGenreCaseStudy1::~GameGenreCaseStudy1() {}

bool GameGenreCaseStudy1::onCreate(int a_argc, char* a_argv[]) {
	Gizmos::create();
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//--TUTORIAL

	m_player = new Player(5);
	m_camera = new Camera(10);
	m_camera->setTarget(&m_player->getTransform(),glm::vec3(0,2.5f,5));
	//--TUTORIAL

	return true;
}

void GameGenreCaseStudy1::onUpdate(float a_deltaTime) {
	Gizmos::clear();
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );
	for ( int i = 0 ; i < 21 ; ++i ) {
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );	
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}

	//--TUTORIAL
	m_player->update(a_deltaTime);
	m_camera->update(a_deltaTime);
	//--TUTORIAL

	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void GameGenreCaseStudy1::onDraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//--TUTORIAL
	m_player->draw();
	//--TUTORIAL

	Gizmos::draw(m_camera->getViewTransform(), m_projectionMatrix);
}

void GameGenreCaseStudy1::onDestroy(){	
	Gizmos::destroy();
}

int main(int argc, char* argv[]) {
	Application* app = new GameGenreCaseStudy1();
	if (app->create("AIE - 09_GameGenreCaseStudy1",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();

	delete app;
	return 0;
}