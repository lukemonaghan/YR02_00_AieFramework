#ifndef __GoalOrientatedBehaviousAndPlanning_H_
#define __GoalOrientatedBehaviousAndPlanning_H_

#include "Application.h"
#include <glm/glm.hpp>

#include "HTN.h"

// Derived application class that wraps up all globals neatly
class GoalOrientatedBehaviousAndPlanning : public Application
{
public:

	GoalOrientatedBehaviousAndPlanning();
	virtual ~GoalOrientatedBehaviousAndPlanning();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	Agent*		m_agent;
	Task*		m_task;

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // __GoalOrientatedBehaviousAndPlanning_H_