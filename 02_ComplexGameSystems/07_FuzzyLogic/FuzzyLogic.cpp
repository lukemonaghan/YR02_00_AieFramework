#include "FuzzyLogic.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

Fuzzy fuzzyEngine;  //not much of an engine but it's a start

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

FuzzyLogic::FuzzyLogic(){}

FuzzyLogic::~FuzzyLogic(){}

void FuzzyLogic::FuzzyLogicExample() {
	printf("Fuzzy Logic Tutorial\n");

	worldController = new WorldController();

	SimpleAI* simpleAI = new SimpleAI(glm::vec3(0,0,0),glm::vec4(1,1,0,1),4);
	worldController->addObject(simpleAI);

	Water* water1 = new Water(glm::vec3(50,0,0));
	worldController->addObject(water1);

	Cave* cave = new Cave(glm::vec3(-50,0,0));
	worldController->addObject(cave);

	Food* food = new Food(glm::vec3(0,40,0));
	worldController->addObject(food);


	fuzzyEngine.tired = new leftShoulderMembershipFunction(0.2f,0.4f,"tired");
	fuzzyEngine.awake = new TrapezoidFunction(0.2f,0.4f,0.6f,0.8f,"awake");
	fuzzyEngine.superActive = new rightShoulderMembershipFunction(0.6f,0.8f,"SuperActive");

	fuzzyEngine.veryHungry = new leftShoulderMembershipFunction(0.2f,0.4f,"very hungry");
	fuzzyEngine.hungry = new TrapezoidFunction(.2f,.4f,.8f,.9f,"hungry");
	fuzzyEngine.full = new rightShoulderMembershipFunction(.8f,.9f,"full");

	fuzzyEngine.WeekFromThirsty = new leftShoulderMembershipFunction(0.1f,0.3f,"week from thirst");
	fuzzyEngine.veryThirsty = new TriangleFunction(0.1f,0.3f,0.5f,"very thristy");
	fuzzyEngine.thirsty = new TriangleFunction(0.3f,0.5f,0.7f,"thristy");
	fuzzyEngine.notThirsty = new rightShoulderMembershipFunction(.5f,.7f,"not thirsty");

	fuzzyEngine.veryNear = new leftShoulderMembershipFunction(2,4,"very close");
	fuzzyEngine.mediumRange = new TrapezoidFunction(2,4,50,70,"medium range");
	fuzzyEngine.farAway = new rightShoulderMembershipFunction(50,70,"far away");

	fuzzyEngine.undesirable = new leftShoulderMembershipFunction(0.3f,0.5f,"undesirable");
	fuzzyEngine.desirable = new TriangleFunction(0.3f,0.5f,0.7f,"desirable");
	fuzzyEngine.veryDesirable = new rightShoulderMembershipFunction(0.5f,0.7f,"very desirable");
}

bool FuzzyLogic::onCreate(int a_argc, char* a_argv[]) {
	Gizmos::create();
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//--TUTORIAL

	FuzzyLogicExample();

	//--TUTORIAL

	return true;
}

void FuzzyLogic::onUpdate(float a_deltaTime) {
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );
	Gizmos::clear();
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );
	for ( int i = 0 ; i < 21 ; ++i ) {
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}

	//--TUTORIAL

	static float count = 0;
	worldController->update(a_deltaTime);
	worldController->draw();
	glm::mat4 planeRotateMatrix = glm::rotate(11/7.0f,1.0f,0.0f,0.0f);

	//--TUTORIAL

	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void FuzzyLogic::onDraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );

	//--TUTORIAL

	//--TUTORIAL

	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void FuzzyLogic::onDestroy() {
	Gizmos::destroy();
}

int main(int argc, char* argv[]) {
	Application* app = new FuzzyLogic();
	if (app->create("AIE - 07_FuzzyLogic",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	delete app;
	return 0;
}

