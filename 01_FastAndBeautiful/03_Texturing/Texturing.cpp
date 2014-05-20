#include "Texturing.h"
#include "Gizmos.h"
#include "Utilities.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

Texturing::Texturing(){}
Texturing::~Texturing(){}

bool Texturing::onCreate(int a_argc, char* a_argv[]) {
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);

	MyTexture = new Texture("models/soulspear/soulspear_diffuse.tga",SOIL_LOAD_AUTO);

	MyShader = new Shader("Shaders/04_BasicTexture.vert","Shaders/04_BasicTexture.frag");
	MyShader->SetAttribs(3,0,"Position",1,"Colour",7,"TexCoord1");
	MyShader->SetUniform("Projection","m4fv",1,false,glm::value_ptr(m_projectionMatrix));
	MyShader->SetTexture("DiffuseTexture",0,MyTexture->ID());

	pFloor = new Plane(glm::vec3(-5,0.1f,-5),glm::vec2(10),glm::vec2(1));

	Gizmos::create();
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );
	for ( int i = 0 ; i <= 20 ; ++i )	{
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );		
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}

	return true;
}

void Texturing::onUpdate(float a_deltaTime) {	
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS){ quit(); }	
}

void Texturing::onDraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	MyShader->SetUniform("View","m4fv",1,false,glm::value_ptr(viewMatrix));
	
	// bind out shader and draw the plane
	MyShader->Bind();
	pFloor->Draw();

	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void Texturing::onDestroy() {
	Gizmos::destroy();
}

int main(int argc, char* argv[]) {
	Application* app = new Texturing();
	if (app->create("AIE - 03_Texturing",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
	delete app;
	return 0;
}