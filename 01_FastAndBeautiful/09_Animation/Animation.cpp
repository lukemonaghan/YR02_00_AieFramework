#include "Animation.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

bool Animation::onCreate(int a_argc, char* a_argv[]) {
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

	s09Default = new Shader("shaders/09_Animation.vert","shaders/09_Animation.frag");
	s09Default->SetAttribs(4,0,"position",7,"texCoord",5,"indices",6,"weights");
	s09Default->SetUniform("Projection","m4fv",1,false,glm::value_ptr(m_projectionMatrix));

	//fbxModel = new FBXModel("models/characters/Demolition/Demolition.fbx", FBXFile::UNITS_METER);
	//fbxModel = new FBXModel("models/characters/EnemyElite/EnemyElite.fbx", FBXFile::UNITS_METER);
	//fbxModel = new FBXModel("models/characters/enemynormal/enemynormal.fbx", FBXFile::UNITS_METER);
	fbxModel = new FBXModel("models/characters/Enemytank/Enemytank.fbx", FBXFile::UNITS_METER);
	//fbxModel = new FBXModel("models/characters/Marksman/Marksman.fbx", FBXFile::UNITS_METER);
	//fbxModel = new FBXModel("models/characters/Pyro/Pyro.fbx", FBXFile::UNITS_METER);
	fbxModel->Update();
	fbxModel->HasAnimation = true;
	return true;
}

void Animation::onUpdate(float a_deltaTime) {
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );

	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS){quit();}
}

void Animation::onDraw() {
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	s09Default->SetUniform("View","m4fv",1,false,glm::value_ptr(viewMatrix * glm::scale(glm::vec3(0.2f))));

	Clear();
	fbxModel->Draw(s09Default,viewMatrix);

	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void Animation::onDestroy(){
	Gizmos::destroy();
}

int main(int argc, char* argv[]){
	Application* app = new Animation();
	if (app->create("AIE - 09_Animation",Utility::Width,Utility::Height,argc,argv) == true){
		app->run();
	}
	delete app;
	return 0;
}