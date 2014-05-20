#include "ParticleSystem.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

ParticleSystem::ParticleSystem(){}

ParticleSystem::~ParticleSystem(){}

bool ParticleSystem::onCreate(int a_argc, char* a_argv[]) {
	glClearColor(0,0,0,1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);

	Gizmos::create();
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );
	for ( int i = 0 ; i < 21 ; ++i ){
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10),  glm::vec4(1,1,1,1));
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i),  glm::vec4(1,1,1,1));
	}

	//Create a shader and set attribs
	ParticleShader = new Shader("Shaders/04_Particle.vert","Shaders/04_Particle.frag");
	ParticleShader->SetAttribs(3,0,"Position",1,"Colour",7,"TexCoord1");

	// create a world-space matrix for a camera
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );

	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);
	ParticleShader->SetUniform("Projection","m4fv",1,false,glm::value_ptr(m_projectionMatrix));

	// create a texture and push it to GPU
	ParticleTexture = new Texture("textures/04_Particles/Particle.png");
	ParticleShader->SetTexture("DiffuseTexture",0,ParticleTexture->ID());

	//create emitter and set it up
	PEEmit = new ParticleEmitter();
	PEEmit->initalise(1000, 200, 0.1f, 3.0f, 2, 3, 0.5f, 0.0f,glm::vec4(1,1,1,1), glm::vec4(0.4,0.4,0.4,0));

	return true;
}

void ParticleSystem::onUpdate(float a_deltaTime) {
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );
	m_viewMatrix = glm::inverse( m_cameraMatrix );
	ParticleShader->SetUniform("View","m4fv",1,false,glm::value_ptr(m_viewMatrix));

	// Update our particle emitters
	PEEmit->update(a_deltaTime,m_cameraMatrix);

	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS) { quit(); }
}

void ParticleSystem::onDraw() {
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ParticleShader->Bind();
	PEEmit->draw();
	// draw the gizmos from this frame
	Gizmos::draw(m_viewMatrix, m_projectionMatrix);
}

void ParticleSystem::onDestroy() {
	// clean up anything we created
	Gizmos::destroy();
}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Application* app = new ParticleSystem();
	
	if (app->create("AIE - 04_ParticleSystem",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	// explicitly control the destruction of our application
	delete app;

	return 0;
}