#include "Advanced_Texturing.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

Advanced_Texturing::Advanced_Texturing(){}

Advanced_Texturing::~Advanced_Texturing(){}

bool Advanced_Texturing::onCreate(int a_argc, char* a_argv[]) {
	// initialise the Gizmos helper class
	Gizmos::create();

	// create a world-space matrix for a camera
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	
	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_Shader = new Shader("Shaders/07_AdvancedTexturing.vert","Shaders/07_AdvancedTexturing.frag");
	m_Shader->SetAttribs(5,0,"Position",2,"Normal",3,"Tangent",4,"BiNormal",7,"TexCoord1");

	m_Model = new FBXModel("Models/soulspear/soulspear.fbx", FBXFile::UNITS_CENTIMETER);

	m_DecayTexture = new Texture("textures/decay.png");

	fDecayValue = 0.5f;

	return true;
}

void Advanced_Texturing::onUpdate(float a_deltaTime)  {
	// quit our application when escape is pressed
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS){quit();}

	// clear all gizmos from last frame
	Gizmos::clear();

	// update our camera matrix using the keyboard/mouse
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );

	// add an identity matrix gizmo
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );

	// add a 20x20 grid on the XZ-plane
	for ( int i = 0 ; i < 21 ; ++i ){
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}

//UPDATE

	if (glfwGetKey(m_window,GLFW_KEY_UP))	{if (fDecayValue < 1.0f) fDecayValue += a_deltaTime;}
	if (glfwGetKey(m_window,GLFW_KEY_DOWN))	{if (fDecayValue > 0.0f) fDecayValue -= a_deltaTime;}
	m_Shader->SetUniform("Time","1f",1,Utility::getTotalTime());
	m_Model->Update();

//UPDATE

}

void Advanced_Texturing::onDraw() {
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );

	// draw the gizmos from this frame
	Gizmos::draw(viewMatrix, m_projectionMatrix);

//DRAW

	//m_Shader->SetTexture("DecayTexture",4,m_DecayTexture->ID());//we use 4 as model used 1-3
	m_Shader->SetUniform("View","m4fv",1,false,glm::value_ptr(viewMatrix));
	m_Shader->SetUniform("Projection","m4fv",1,false,glm::value_ptr(m_projectionMatrix));
	//m_Shader->SetUniform("LightPosition","3fv",1,&m_cameraMatrix[3].xyz);

	//m_Shader->SetUniform("AmbientLightColor","3fv",1,glm::value_ptr(glm::vec3(0.4f)));
	//m_Shader->SetUniform("LightColor","3fv",1,glm::value_ptr(glm::vec3(1)));

	m_Model->Draw(m_Shader,viewMatrix);

//DRAW

}

void Advanced_Texturing::onDestroy() {
	// clean up anything we created
	Gizmos::destroy();
}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[]) {
	// explicitly control the creation of our application
	Application* app = new Advanced_Texturing();
	
	if (app->create("AIE - 07_Advanced_Texturing",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	// explicitly control the destruction of our application
	delete app;

	return 0;
}