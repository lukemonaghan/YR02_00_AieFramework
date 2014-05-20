#include "GoalOrientatedBehaviousAndPlanning.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

//!--TUTORIAL

class GoToTask : public Task {
public:
	GoToTask(const glm::vec3 &a_location, float a_range) : m_location(a_location), m_range(a_range) { }
	virtual ~GoToTask() { }

	virtual bool performTask(Agent* a_agent){
		// Check
		glm::vec3 diff = m_location - a_agent->getPosition();
		m_complete = glm::length2( diff ) <= m_range;

		// Move
		diff = glm::normalize( diff ) * Utility::getDeltaTime();
		a_agent->setPosition( a_agent->getPosition() + diff );

		// Return
		return m_complete;
	}

protected:
	glm::vec3 m_location;
	float m_range;

};

class TimeoutTask : public Task {
public:
	TimeoutTask(float a_duration) : m_duration(a_duration) { }
	virtual ~TimeoutTask() { }

	virtual bool performTask(Agent* a_agent){
		m_duration -= Utility::getDeltaTime();

		m_complete = (m_duration <= 0);

		return m_complete;
	}

protected:
	float m_duration;
};

//!--TUTORIAL

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

GoalOrientatedBehaviousAndPlanning::GoalOrientatedBehaviousAndPlanning(){}
GoalOrientatedBehaviousAndPlanning::~GoalOrientatedBehaviousAndPlanning(){}

bool GoalOrientatedBehaviousAndPlanning::onCreate(int a_argc, char* a_argv[]) {
	Gizmos::create();
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//!--TUTORIAL

	m_agent = new Agent();

	CompoundTask	*goBuyEggs		= new CompoundTask();

	CompoundTask	*goToShops		= new CompoundTask();
	Task			*getInCar		= new GoToTask(glm::vec3(2,0,0),0.1f);
	Task			*driveToShops	= new GoToTask(glm::vec3(2,0,10),0.1f);
	Task			*walkInToShops	= new GoToTask(glm::vec3(0,0,10),0.1f);

	Task			*shopping		= new TimeoutTask(5);

	CompoundTask	*GoHome			= new CompoundTask();
	Task			*getBackInCar	= new GoToTask(glm::vec3(2,0,10),0.1f);
	Task			*driveBackHome	= new GoToTask(glm::vec3(2,0,0),0.1f);
	Task			*goInside		= new GoToTask(glm::vec3(0,0,0),0.1f);

	goBuyEggs->addTask(goToShops);
		goToShops->addTask(getInCar);
		goToShops->addTask(driveToShops);
		goToShops->addTask(walkInToShops);

	goBuyEggs->addTask(shopping);

	goBuyEggs->addTask(GoHome);
		GoHome->addTask(getBackInCar);
		GoHome->addTask(driveBackHome);
		GoHome->addTask(goInside);

	m_task = goBuyEggs;

	//!--TUTORIAL

	return true;
}

void GoalOrientatedBehaviousAndPlanning::onUpdate(float a_deltaTime) {
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );
	Gizmos::clear();
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );
	for ( int i = 0 ; i < 21 ; ++i ) {
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );	
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}
		
	//!--TUTORIAL

	if (m_task->isComplete() == false){
		m_task->performTask(m_agent);
	}

	Gizmos::addAABBFilled(m_agent->getPosition(),glm::vec3(0.2f),glm::vec4(1,1,0,1));

	//!--TUTORIAL

	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void GoalOrientatedBehaviousAndPlanning::onDraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	//!--TUTORIAL

	//!--TUTORIAL

	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void GoalOrientatedBehaviousAndPlanning::onDestroy() {
	//!--TUTORIAL

	delete m_agent;
	delete m_task;	

	//!--TUTORIAL

	Gizmos::destroy();

}

int main(int argc, char* argv[]) {
	Application* app = new GoalOrientatedBehaviousAndPlanning();
	if (app->create("AIE - 06_GoalOrientatedBehaviousAndPlanning",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	delete app;
	return 0;
}