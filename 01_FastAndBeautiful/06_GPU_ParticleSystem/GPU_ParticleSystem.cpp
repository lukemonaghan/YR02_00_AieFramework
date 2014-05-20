#include "GPU_ParticleSystem.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include "GPUParticle.h"
#include "Texture.h"

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

GPUParticle *m_emitter;
Texture *m_Texture;

float fTimer;

GPU_ParticleSystem::GPU_ParticleSystem(){ }

GPU_ParticleSystem::~GPU_ParticleSystem(){ }

bool GPU_ParticleSystem::onCreate(int a_argc, char* a_argv[]) {
	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	// initialise the Gizmos helper class
	Gizmos::create();

	// create a world-space matrix for a camera
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	
	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);

	m_emitter = new GPUParticle();
	m_emitter->initalise(300000, 0.1f, 5.0f, 25, 100, 1.0f, 0.0f, glm::vec4(0,1,0,1), glm::vec4(0,0,1,0));
	m_Texture = new Texture("textures/06_GPUParticles/Particle.png");
	//5 million particles max
	return true;
}

void GPU_ParticleSystem::onUpdate(float a_deltaTime) {
	// quit our application when escape is pressed
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)quit();

	// update our camera matrix using the keyboard/mouse
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );

	// clear all gizmos from last frame
	Gizmos::clear();
	
	// add an identity matrix gizmo
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );

	// add a grid on the XZ-plane
	int size = 40;
	for ( int i = 0 ; i <= size ; ++i ) {
		Gizmos::addLine( glm::vec3(-size/2 + i, 0, size/2 ), glm::vec3(-size/2 + i, 0, -size/2),  i == size/2 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
		Gizmos::addLine( glm::vec3( size/2, 0, -size/2 + i), glm::vec3(-size/2, 0, -size/2 + i),  i == size/2 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}
//UPDATE
	fTimer -= a_deltaTime;
	if (fTimer <= 0){
		fTimer = 1.0f;
		std::cout << (1/a_deltaTime) << '\n';
	}
//UPDATE
}

void GPU_ParticleSystem::onDraw() {
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// get the view matrix from the world-space camera matrix
	m_viewMatrix = glm::inverse( m_cameraMatrix );
	// draw the gizmos from this frame
	Gizmos::draw(m_viewMatrix, m_projectionMatrix);
//DRAW
	m_emitter->draw(m_cameraMatrix,m_projectionMatrix,m_Texture->ID());
//DRAW
}

void GPU_ParticleSystem::onDestroy(){
	// clean up anything we created
	Gizmos::destroy();
}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[]) {
	// explicitly control the creation of our application
	Application* app = new GPU_ParticleSystem();
	
	if (app->create("AIE - GPU_ParticleSystem",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	// explicitly control the destruction of our application
	delete app;

	return 0;
}