#ifndef ___03_LinearForceMomentum_H_
#define ___03_LinearForceMomentum_H_

#include "Application.h"
#include <glm/glm.hpp>

// Derived application class that wraps up all globals neatly
class _03_LinearForceMomentum : public Application
{
public:

	_03_LinearForceMomentum();
	virtual ~_03_LinearForceMomentum();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
	
	//!- TUTORIAL
	
	//!- TUTORIAL
};

#endif // ___03_LinearForceMomentum_H_