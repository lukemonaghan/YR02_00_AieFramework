#include "FBX.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

bool FBX::onCreate(int a_argc, char* a_argv[]) {
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Gizmos::create();
	Gizmos::addTransform( glm::mat4(1),1);
	for ( int i = 0 ; i <= 20 ; ++i ){
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(15,15,15),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, Utility::Width/(float)Utility::Height, 0.01f, 1000.0f);

	m_fbxModel = new FBXModel("Models/ruinedtank/tank.fbx", FBXFile::UNITS_CENTIMETER);
	m_fbxModel->Update();
	
	ShaderDefault = new Shader("Shaders/02_FBX.vert","Shaders/02_FBX.frag");
	ShaderDefault->SetAttribs(3,0,"Position",1,"Color",7,"TexCoord1");
	ShaderDefault->SetUniform("Projection","m4fv",1,false,glm::value_ptr(m_projectionMatrix));

	return true;
}

void FBX::onUpdate(float a_deltaTime) {
	if (glfwGetKey(m_window,GLFW_KEY_F1) == GLFW_PRESS){ glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ); }else{ glPolygonMode( GL_FRONT_AND_BACK, GL_FILL ); }
	
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );

	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS){ quit(); }	

}

void FBX::onDraw() {
	viewMatrix = glm::inverse( m_cameraMatrix );
	ShaderDefault->SetUniform("View","m4fv",1,false,glm::value_ptr(viewMatrix));

	Clear();
	m_fbxModel->Draw( ShaderDefault ,viewMatrix);

	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void FBX::onDestroy(){
	delete m_fbxModel;
	m_fbxModel = NULL;
	Gizmos::destroy();
}

int main(int argc, char* argv[]){
	Application* app = new FBX();
	if (app->create("AIE - 02_FBX",Utility::Width,Utility::Height,argc,argv) == true){ app->run(); }
	delete app;
	return 0;
}