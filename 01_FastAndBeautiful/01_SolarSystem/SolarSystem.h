#ifndef __SolarSystem_H_
#define __SolarSystem_H_

#include "Application.h"
#include <glm/glm.hpp>

#include "Planet.h"

#define COUNT_PLANETS 10
#define COUNT_MOONS 3

// Derived application class that wraps up all globals neatly
class SolarSystem : public Application {
public:

	SolarSystem();
	virtual ~SolarSystem();
	void DebugSizes(int iType = 0);
	float UNITSCALE;
	float fSpeed;
	bool bf2,bf3;
	bool bPressed,bAddPressed;

	Planet* Planets[COUNT_PLANETS];
	Planet* Moons[COUNT_MOONS];
	Planet* glmLookat;
protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // __SolarSystem_H_