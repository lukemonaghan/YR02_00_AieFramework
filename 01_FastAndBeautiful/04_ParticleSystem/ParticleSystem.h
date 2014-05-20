#ifndef __ParticleSystem_H_
#define __ParticleSystem_H_

#include "Application.h"
#include <glm/glm.hpp>

#include <Particle.h>
#include <Shader.h>
#include <Texture.h>

// Derived application class that wraps up all globals neatly
class ParticleSystem : public Application
{
public:

	Shader* ParticleShader;
	ParticleEmitter *PEEmit;
	Texture* ParticleTexture;

	ParticleSystem();
	virtual ~ParticleSystem();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
	glm::mat4	m_viewMatrix;
};

#endif // __ParticleSystem_H_