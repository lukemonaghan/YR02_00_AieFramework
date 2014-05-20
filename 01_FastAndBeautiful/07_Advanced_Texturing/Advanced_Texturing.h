#ifndef __Advanced_Texturing_H_
#define __Advanced_Texturing_H_

#include "Application.h"
#include <glm/glm.hpp>
#include <FBXModel.h>
#include <Shader.h>
#include <Texture.h>

// Derived application class that wraps up all globals neatly
class Advanced_Texturing : public Application
{
public:

	//Files 07_AdvancedTexturing
	Shader *m_Shader;
	FBXModel *m_Model;
	Texture *m_DecayTexture;

	float fDecayValue;
	//Files 07_AdvancedTexturing

	Advanced_Texturing();
	virtual ~Advanced_Texturing();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // __Advanced_Texturing_H_