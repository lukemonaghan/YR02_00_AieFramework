#ifndef ___07_FuzzyLogic_H_
#define ___07_FuzzyLogic_H_

#include "Application.h"
#include <glm/glm.hpp>

#include "SimpleAgent.h"
#include "DIYPhysicsEngine.h"
#include "FuzzyLogicEngine.h"

// Derived application class that wraps up all globals neatly
class FuzzyLogic : public Application
{
public:

	FuzzyLogic();
	virtual ~FuzzyLogic();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	void FuzzyLogicExample();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;

	WorldController* worldController;

};

#endif // ___07_FuzzyLogic_H_