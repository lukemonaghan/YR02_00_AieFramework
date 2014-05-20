#include "HDRL.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

bool HDRL::onCreate(int a_argc, char* a_argv[]) {
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, Utility::Width/(float)Utility::Height, 0.1f, 1000.0f);

	Gizmos::create();
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );
	for ( int i = 0 ; i < 21 ; ++i ){
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );	
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}

	fbxModel = new FBXModel("models/soulspear/soulspear.fbx");
	fbxModel->Update();

	fbScene = new FrameBuffer(1,Utility::Width,Utility::Height);
	sScene = new Shader("shaders/16_HDR_Scene.vert","shaders/16_HDR_Scene.frag");
	sScene->SetAttribs(5,0,"position",2,"normal",3,"tanget",4,"biNormal",7,"texCoord");
	sScene->SetUniform("Projection","m4fv",1,false,glm::value_ptr(m_projectionMatrix));

	fbBrightPass = new FrameBuffer(1,Utility::Width,Utility::Height);
	sBrightPass = new Shader("shaders/16_HDR_Screen.vert","shaders/16_HDR_BrightPass.frag");
	sBrightPass->SetAttribs(1,0,"position");
	sBrightPass->SetFragOuts(1,0,"fragColour");

	fbBlur = new FrameBuffer(1,Utility::Width,Utility::Height);
	sBloomPass = new Shader("shaders/16_HDR_Screen.vert","shaders/16_HDR_Blur.frag");
	sBloomPass->SetAttribs(1,0,"position");
	sBloomPass->SetFragOuts(1,0,"fragColour");

	sScreen = new Shader("shaders/16_HDR_Screen.vert","shaders/16_HDR_Bloom.frag");
	sScreen->SetAttribs(1,0,"position");
	sScreen->SetFragOuts(1,0,"fragColour");

	htScreen = new HUDTexture(glm::vec2(Utility::Width/2,Utility::Height/2),glm::vec2(Utility::Width,Utility::Height));

	return true;
}

void HDRL::onUpdate(float a_deltaTime) {
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );
	sScene->SetUniform("time","1f",1,Utility::getTotalTime());
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS){quit();}
}

void HDRL::onDraw() {
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	sScene->SetUniform("View","m4fv",1,false,glm::value_ptr(viewMatrix));
	sScene->SetUniform("cameraPosition","3fv",1,glm::value_ptr((glm::vec3)m_cameraMatrix[3].xyz));

	fbScene->Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	fbxModel->Draw(sScene,viewMatrix);

	fbBrightPass->Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	sBrightPass->SetTexture("originalMap",0,fbScene->GetTexture(0));
	htScreen->Draw();

	fbBlur->Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	sBloomPass->SetTexture("originalMap",0,fbBrightPass->GetTexture(0));
	sBloomPass->SetUniform("horizontal","1i",1,0);
	htScreen->Draw();
	sBloomPass->SetUniform("horizontal","1i",1,1);
	sBloomPass->SetTexture("originalMap",0,fbBlur->GetTexture(0));
	htScreen->Draw();

	FrameBuffer::Unbind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	sScreen->SetTexture("originalMap",0,fbScene->GetTexture(0));
	sScreen->SetTexture("blurredMap",1,fbBlur->GetTexture(0));
	htScreen->Draw();

	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void HDRL::onDestroy() {
	Gizmos::destroy();
}

int main(int argc, char* argv[]) {
	Application* app = new HDRL();
	if (app->create("AIE - 16_HDRL",Utility::Width,Utility::Height,argc,argv) == true){
		app->run();
	}
	delete app;
	return 0;
}