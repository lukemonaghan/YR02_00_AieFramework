#ifndef __NonPhotoRealistic_H_
#define __NonPhotoRealistic_H_

#include "Application.h"
#include <glm/glm.hpp>

#include <Shader.h>
#include <FBXModel.h>
#include <Texture.h>

// Derived application class that wraps up all globals neatly
class NonPhotoRealistic : public Application {
public:

	Shader *sNonRealistic;
	FBXModel *fbxModel;
	Texture *tHatch0,*tHatch1;

	NonPhotoRealistic(){};
	virtual ~NonPhotoRealistic(){};

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // __NonPhotoRealistic_H_