#ifndef __GameGenreCaseStudy1_H_
#define __GameGenreCaseStudy1_H_

#include "Application.h"
#include <glm/glm.hpp>
#include "Player.h"
#include "Camera.h"

// Derived application class that wraps up all globals neatly
class GameGenreCaseStudy1 : public Application {
public:

	GameGenreCaseStudy1();
	virtual ~GameGenreCaseStudy1();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_projectionMatrix;

	Player		*m_player;
	Camera		*m_camera;
};

#endif // __GameGenreCaseStudy1_H_