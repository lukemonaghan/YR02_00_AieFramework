#include "NonPhotoRealistic.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

bool NonPhotoRealistic::onCreate(int a_argc, char* a_argv[]) {
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	Gizmos::create();
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );
	for ( int i = 0 ; i < 21 ; ++i ) {
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}

	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, Utility::Width/(float)Utility::Height, 0.1f, 1000.0f);

	fbxModel = new FBXModel("models/soulspear/soulspear.fbx");
	fbxModel->Update();

	sNonRealistic = new Shader("shaders/15_NonRealistic.vert","shaders/15_NonRealistic.frag");
	sNonRealistic->SetAttribs(3,0,"Position",2,"Normal",7,"TexCoord1");
	sNonRealistic->SetUniform("Projection","m4fv",1,false,glm::value_ptr(m_projectionMatrix));

	tHatch0 = new Texture("textures/15_NonRealistic/1to3.png");
	sNonRealistic->SetTexture("hatchSampler0",0,tHatch0->ID());
	tHatch1 = new Texture("textures/15_NonRealistic/4to6.png");
	sNonRealistic->SetTexture("hatchSampler1",1,tHatch1->ID());

	return true;
}

void NonPhotoRealistic::onUpdate(float a_deltaTime) {
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );

	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS){quit();}
	if (glfwGetKey(m_window,GLFW_KEY_F1) == GLFW_PRESS){
		sNonRealistic->ReloadShader();
		sNonRealistic->SetAttribs(3,0,"Position",2,"Normal",7,"TexCoord1");
		sNonRealistic->SetUniform("Projection","m4fv",1,false,glm::value_ptr(m_projectionMatrix));
	}
	sNonRealistic->SetUniform("Time","1f",1,Utility::getTotalTime());
}

void NonPhotoRealistic::onDraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );

	sNonRealistic->SetUniform("View","m4fv",1,false,glm::value_ptr(viewMatrix));
	fbxModel->Draw(sNonRealistic,viewMatrix);

	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void NonPhotoRealistic::onDestroy(){
	Gizmos::destroy();
}

int main(int argc, char* argv[])
{
	Application* app = new NonPhotoRealistic();
	if (app->create("AIE - 15_NonPhotoRealistic",Utility::Width,Utility::Height,argc,argv) == true){
		app->run();
	}
	delete app;
	return 0;
}