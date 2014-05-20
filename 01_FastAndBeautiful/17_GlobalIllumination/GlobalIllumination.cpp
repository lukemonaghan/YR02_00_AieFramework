#include "GlobalIllumination.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

bool GlobalIllumination::onCreate(int a_argc, char* a_argv[]) {
	glClearColor(0.5f,0.5f,0.5f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(8,5,0),glm::vec3(-8,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, Utility::Width/(float)Utility::Height, 0.1f, 1000.0f);

	Gizmos::create();
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );
	for ( int i = 0 ; i < 21 ; ++i ){
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}

	fbxModel = new FBXModel("models/sponza/sponzalight.fbx",FBXFile::UNITS_METER);
	fbxModel->Update();

	fbScene = new FrameBuffer(2,Utility::Width,Utility::Height);

	sScene = new Shader("shaders/17_GlobalNormal.vert","shaders/17_GlobalDeferred.frag");
	sScene->SetAttribs(2,0,"position",2,"normal");
	sScene->SetFragOuts(2,0,"positionMap",1,"normalMap");
	sScene->SetUniform("Projection","m4fv",1,false,glm::value_ptr(m_projectionMatrix));

	sScreen = new Shader("shaders/17_GlobalScreen.vert","shaders/17_GlobalSSAO.frag");
	sScreen->SetAttribs(2,0,"Position",2,"TexCoord1");
	sScreen->SetUniform("uradius","1f",1,0.1f);
	sScreen->SetUniform("uintensity","1f",1,0.5f);
	sScreen->SetUniform("uscale","1f",1,0.1f);
	sScreen->SetUniform("ubias","1f",1,0.1f);

	htScreen = new HUDTexture(glm::vec2(Utility::Width/2,Utility::Height/2),glm::vec2(Utility::Width,Utility::Height));

	tRandom = new Texture("textures/random.png");
	return true;
}

void GlobalIllumination::onUpdate(float a_deltaTime) {
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );

	if (glfwGetKey(m_window,GLFW_KEY_F1) == GLFW_PRESS){
		sScene->ReloadShader();
		sScene->SetAttribs(2,0,"position",2,"normal");
		sScene->SetFragOuts(2,0,"positionMap",1,"normalMap");
		sScene->SetUniform("Projection","m4fv",1,false,glm::value_ptr(m_projectionMatrix));

		sScreen->ReloadShader();
		sScreen->SetAttribs(2,0,"Position",2,"TexCoord1");
		sScreen->SetUniform("uradius","1f",1,0.1f);
		sScreen->SetUniform("uintensity","1f",1,0.5f);
		sScreen->SetUniform("uscale","1f",1,0.1f);
		sScreen->SetUniform("ubias","1f",1,0.1f);
	}

	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS){quit();}
}

void GlobalIllumination::onDraw() {
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	sScene->SetUniform("View","m4fv",1,false,glm::value_ptr(viewMatrix));

	fbScene->Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	fbxModel->Draw(sScene,viewMatrix);

	FrameBuffer::Unbind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	sScreen->SetTexture("positionMap",0,fbScene->GetTexture(0));
	sScreen->SetTexture("normalMap"	 ,1,fbScene->GetTexture(1));
	sScreen->SetTexture("randomMap"	 ,2,tRandom->ID());
	htScreen->Draw();

	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void GlobalIllumination::onDestroy(){
	Gizmos::destroy();
}

int main(int argc, char* argv[]){
	Application* app = new GlobalIllumination();
	if (app->create("AIE - 17_GlobalIllumination",Utility::Width,Utility::Height,argc,argv) == true){
		app->run();
	}
	delete app;
	return 0;
}