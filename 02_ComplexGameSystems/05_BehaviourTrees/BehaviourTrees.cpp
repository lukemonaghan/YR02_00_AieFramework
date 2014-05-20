#include "BehaviourTrees.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

//!--TUTORIAL

#include "Agent.h"

class WithinRange : public Behaviour{
public:
	WithinRange(float a_range) : m_range(a_range) {}
	virtual ~WithinRange() {}

	virtual bool excecute(Agent *a_Agent){
		float dist2 = glm::distance2(a_Agent->getPosition(),a_Agent->getTarget() );

		if (dist2 < m_range){
			return true;
		}
		return false;
	}

	float m_range;
};

class RandomiseTarget : public Behaviour{
public:
	RandomiseTarget(float a_radius) : m_radius(a_radius) {}
	virtual ~RandomiseTarget() {}

	virtual bool excecute(Agent *a_Agent){
		glm::vec3 target(0);

		target.xz = glm::circularRand(m_radius);

		a_Agent->setTarget(target);

		return true;
	}

	float m_radius;
};

class SeekTarget : public Behaviour{
public:
	SeekTarget(float a_speed) : m_speed(a_speed) {}
	virtual ~SeekTarget() {}

	virtual bool excecute(Agent *a_Agent){
		glm::vec3 pos = a_Agent->getPosition();
		glm::vec3 dir = glm::normalize(a_Agent->getTarget() - pos);
		a_Agent->setPosition(pos + dir * m_speed  * Utility::getDeltaTime());
		return true;
	}

	float m_speed;
};

//!--TUTORIAL

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

BehaviourTrees::BehaviourTrees(){}
BehaviourTrees::~BehaviourTrees(){}

bool BehaviourTrees::onCreate(int a_argc, char* a_argv[]) {
	Gizmos::create();
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//!--TUTORIAL

	m_agent = new Agent();

	Behaviour *seek = new SeekTarget(3);
	Behaviour *random = new RandomiseTarget(10);
	Behaviour *within = new WithinRange(0.5f);

	Sequence *seq = new Sequence();
	seq->addChild(within);
	seq->addChild(random);

	Selector *root = new Selector();
	root->addChild(seq);
	root->addChild(seek);

	m_behaviour = root;

	m_agent->setBehaviour(m_behaviour);

	//!--TUTORIAL

	return true;
}

void BehaviourTrees::onUpdate(float a_deltaTime) {
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );
	Gizmos::clear();
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );
	for ( int i = 0 ; i < 21 ; ++i ) {
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );	
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i), i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}

	//!--TUTORIAL

	m_agent->update(a_deltaTime);
	Gizmos::addAABBFilled(m_agent->getPosition(),glm::vec3(0.5f),glm::vec4(1,1,0,1));
	Gizmos::addAABBFilled(m_agent->getTarget(),glm::vec3(0.2f),glm::vec4(1,0,0,1));

	//!--TUTORIAL

	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void BehaviourTrees::onDraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );

	//!--TUTORIAL


	//!--TUTORIAL

	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void BehaviourTrees::onDestroy() {
	delete m_agent;
	delete m_behaviour;
	Gizmos::destroy();
}

int main(int argc, char* argv[]) {
	Application* app = new BehaviourTrees();
	
	if (app->create("AIE - 05_BehaviourTrees",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	delete app;

	return 0;
}