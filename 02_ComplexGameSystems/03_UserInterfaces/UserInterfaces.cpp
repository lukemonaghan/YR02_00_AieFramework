#include "UserInterfaces.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <SOIL.h>
#include "UIButton.h"

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

UserInterfaces::UserInterfaces(){}
UserInterfaces::~UserInterfaces(){}

void Button1Callback(UIButton *a_this,void* a_data){
	printf("Button 1 \n");
	UIRectangle* parent = (UIRectangle*)a_data;
	parent->setPosition(glm::vec2(rand()%DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT / 2));
}

void Button2Callback(UIButton *a_this,void* a_data){
	printf("Button 2 \n");
	UIRectangle* parent = (UIRectangle*)a_data;
	parent->setPosition(glm::vec2(rand()%DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT / 1.5));
}

void Button3Callback(UIButton *a_this,void* a_data){
	printf("Button 3 \n");
	UIRectangle* parent = (UIRectangle*)a_data;
	parent->setPosition(glm::vec2(rand()%DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT / 3));
}

bool UserInterfaces::onCreate(int a_argc, char* a_argv[]) {
	Gizmos::create();
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//!--TUTORIAL

	unsigned int vs = Utility::loadShader("shaders/03_UserInterface.vert",GL_VERTEX_SHADER);
	unsigned int fs = Utility::loadShader("shaders/03_UserInterface.frag",GL_FRAGMENT_SHADER);
	m_guishader = Utility::createProgram(vs,0,0,0,fs);
	glDeleteShader(vs);
	glDeleteShader(fs);

	m_cameraProjectionMatrix = glm::ortho<float>(0,DEFAULT_SCREENWIDTH,0,DEFAULT_SCREENHEIGHT);
	m_texture = SOIL_load_OGL_texture("textures/Bricks.jpg",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,0);

	m_button = new UIRectangle(m_guishader,glm::vec2(300,500));
	m_button->setPosition(glm::vec2(DEFAULT_SCREENWIDTH / 2, DEFAULT_SCREENHEIGHT / 2));
	m_button->setTexture(m_texture);

	UIButton *button1 = new UIButton(m_guishader,glm::vec2(100,50),glm::vec4(1),glm::vec4(1,0,0,1),glm::vec4(1,0.5,0.5,1));
	button1->setPosition(glm::vec2(0,100));
	button1->setTexture(m_texture);
	button1->setPressedCB(Button1Callback,m_button);
	m_button->addElement(button1);

	UIButton *button2 = new UIButton(m_guishader,glm::vec2(100,50),glm::vec4(1),glm::vec4(0,1,0,1),glm::vec4(0.5,1,0.5,1));
	button2->setTexture(m_texture);
	button2->setPressedCB(Button2Callback,m_button);
	m_button->addElement(button2);

	UIButton *button3 = new UIButton(m_guishader,glm::vec2(100,50),glm::vec4(1),glm::vec4(0,0,1,1),glm::vec4(0.5,0.5,1,1));
	button3->setPosition(glm::vec2(0,-100));
	button3->setTexture(m_texture);
	button3->setPressedCB(Button3Callback,m_button);
	m_button->addElement(button3);

	//!--TUTORIAL

	return true;
}

void UserInterfaces::onUpdate(float a_deltaTime) {
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );
	Gizmos::clear();
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );
	for ( int i = 0 ; i < 21 ; ++i ){
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}

	//!--TUTORIAL
	glm::vec2 pos = glm::mix(glm::vec2(DEFAULT_SCREENWIDTH * 0.25f, DEFAULT_SCREENHEIGHT / 2),glm::vec2(DEFAULT_SCREENWIDTH * 0.75f, DEFAULT_SCREENHEIGHT / 2),sin(Utility::getTotalTime()) * 0.5f + 0.5f);
	float rot = Utility::getTotalTime();
	
	//m_button->setPosition(pos);
	//m_button->setRotation(rot);

	m_button->update(a_deltaTime);

	//!--TUTORIAL

	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void UserInterfaces::onDraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	Gizmos::draw(viewMatrix, m_projectionMatrix);

	//!--TUTORIAL

	int depthfunc = GL_LESS;
	glGetIntegerv(GL_DEPTH_FUNC,&depthfunc);
	glDepthFunc(GL_LEQUAL);
	m_button->draw(m_cameraProjectionMatrix);
	glDepthFunc(depthfunc);

	//!--TUTORIAL
}

void UserInterfaces::onDestroy() {
	Gizmos::destroy();
	delete m_button;
	glDeleteProgram(m_guishader);
}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Application* app = new UserInterfaces();
	
	if (app->create("AIE - 03_UserInterfaces",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	// explicitly control the destruction of our application
	delete app;

	return 0;
}