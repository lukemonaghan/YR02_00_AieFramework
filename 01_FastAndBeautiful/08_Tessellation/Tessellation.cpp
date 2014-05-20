#include "Tessellation.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <SOIL.h>
#include <Plane.h>
#include <Shader.h>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

Plane *m_Plane;
Shader *m_Shader;

unsigned int m_vao,m_vbo,m_ibo,m_texture,m_displacement;
float TessLevel = 1;
float displacementScale = 1;
float size = 20;

Tessellation::Tessellation(){}

Tessellation::~Tessellation(){}

bool Tessellation::onCreate(int a_argc, char* a_argv[]) {

	Gizmos::create();
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);

	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	m_texture = SOIL_load_OGL_texture("textures/pattern_178/diffuse.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_TEXTURE_REPEATS);
	m_displacement = SOIL_load_OGL_texture("textures/pattern_178/Height.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_TEXTURE_REPEATS);

	m_Shader = new Shader("shaders/08_Tessellation.vert","shaders/08_Tessellation.frag","shaders/08_Tessellation.cont","shaders/08_Tessellation.eval");

	m_Plane = new Plane(glm::vec3(-size/2,0,-size/2),glm::vec2(size,size),glm::vec2(size,size),true,1.0f);

	return true;
}

void Tessellation::onUpdate(float a_deltaTime) {
	Gizmos::clear();
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );
	for ( int i = 0 ; i < 21 ; ++i ){
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );	
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}

	// Keyboard functions
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );

	//check ESC and F1(Lines/fill)
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)quit();
	if (glfwGetKey(m_window,GLFW_KEY_F1) == GLFW_PRESS){ glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ); }else{ glPolygonMode( GL_FRONT_AND_BACK, GL_FILL ); }

	//update the tesselation scale
	if (glfwGetKey(m_window,GLFW_KEY_LEFT) == GLFW_PRESS){if (displacementScale < 10){ displacementScale+=a_deltaTime;}}
	if (glfwGetKey(m_window,GLFW_KEY_RIGHT) == GLFW_PRESS){if (displacementScale > -10){ displacementScale-=a_deltaTime;}}

	//update the amount of tesselation we are using
	if (glfwGetKey(m_window,GLFW_KEY_UP) == GLFW_PRESS){if (TessLevel < 64){ TessLevel+= 30 * a_deltaTime;}if (TessLevel > 64){TessLevel = 64;}}
	if (glfwGetKey(m_window,GLFW_KEY_DOWN) == GLFW_PRESS){if (TessLevel > 1){ TessLevel-= 30 * a_deltaTime;}if (TessLevel < 1){TessLevel = 1;}}

}

void Tessellation::onDraw() {
	//basic ogl stuff
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	//draw gizmos
	Gizmos::draw(viewMatrix, m_projectionMatrix);

	m_Shader->SetUniform("projectionView","m4fv",1, false, glm::value_ptr(m_projectionMatrix * viewMatrix) );
	m_Shader->SetUniform("displacementScale","1f",1, displacementScale );
	m_Shader->SetUniform("TessLevel","1f",1, TessLevel );
	//set textures
	m_Shader->SetTexture("textureMap",0,m_texture);
	m_Shader->SetTexture("displacementMap",1,m_displacement);
	//draw the plane with Tesselation
	m_Plane->DrawTess();
}

void Tessellation::onDestroy() {
	// clean up anything we created
	Gizmos::destroy();
}

int main(int argc, char* argv[]) {
	// explicitly control the creation of our application
	Application* app = new Tessellation();
	
	if (app->create("AIE - 08_Tessellation",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	// explicitly control the destruction of our application
	delete app;

	return 0;
}