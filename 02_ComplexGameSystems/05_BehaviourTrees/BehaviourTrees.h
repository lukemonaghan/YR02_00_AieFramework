#ifndef __BehaviourTrees_H_
#define __BehaviourTrees_H_

#include "Application.h"
#include <glm/glm.hpp>

class Agent;
class Behaviour;

// Derived application class that wraps up all globals neatly
class BehaviourTrees : public Application {
public:

	BehaviourTrees();
	virtual ~BehaviourTrees();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	Agent		*m_agent;
	Behaviour	*m_behaviour;

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // __BehaviourTrees_H_