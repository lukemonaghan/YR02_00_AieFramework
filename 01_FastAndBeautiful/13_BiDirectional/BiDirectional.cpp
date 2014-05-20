#include "BiDirectional.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

bool BiDirectional::onCreate(int a_argc, char* a_argv[]) {
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	Gizmos::create();
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );
	for ( int i = 0 ; i < 21 ; ++i ){
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10), i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i), i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}

	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, Utility::Width/(float)Utility::Height, 0.1f, 1000.0f);

	m_spear = new FBXModel("models/soulspear/soulspear.fbx",FBXFile::UNITS_CENTIMETER);
	m_spear->Update();

	m_Shader = new Shader("shaders/13_BDFR.vert","shaders/13_BDFR.frag");
	m_Shader->SetAttribs(3,0,"Position",1,"Color",7,"TexCoord1");
	m_Shader->SetUniform("Projection","m4fv",1,false,glm::value_ptr(m_projectionMatrix));

	m_lightDirection = glm::vec4(1,0,1,0);

	return true;
}

void BiDirectional::onUpdate(float a_deltaTime) {
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );

	static bool bPressed;
	if (glfwGetKey(m_window,GLFW_KEY_F1)){
		if (!bPressed){
			bPressed = true;
			m_Shader->ReloadShader();
			m_Shader->SetAttribs(3,0,"Position",1,"Color",7,"TexCoord1");
			m_Shader->SetUniform("Projection","m4fv",1,false,glm::value_ptr(m_projectionMatrix));
		}
	}else if (bPressed){
		bPressed = false;
	}

	if (glfwGetKey(m_window,GLFW_KEY_LEFT) == GLFW_PRESS){
		m_lightDirection = glm::rotate(m_lightDirection, 1.0f * a_deltaTime,glm::vec3(1,0,0));
	}else if (glfwGetKey(m_window,GLFW_KEY_RIGHT) == GLFW_PRESS) {
		m_lightDirection = glm::rotate(m_lightDirection,-1.0f * a_deltaTime,glm::vec3(1,0,0));
	}

	m_Shader->SetUniform("LightDir","3fv",1,glm::value_ptr((glm::vec3)m_lightDirection.xyz));

	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS){ quit();}
}

void BiDirectional::onDraw() {
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	m_Shader->SetUniform("View","m4fv",1,false,glm::value_ptr(viewMatrix));
	m_Shader->SetUniform("Camera","3fv",1,glm::value_ptr((glm::vec3)m_cameraMatrix[3].xyz));

	Clear();
	m_spear->Draw(m_Shader ,viewMatrix);
	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void BiDirectional::onDestroy() {
	Gizmos::destroy();
}

int main(int argc, char* argv[]) {
	Application* app = new BiDirectional();
	if (app->create("AIE - 13_BiDirectional",Utility::Width,Utility::Height,argc,argv) == true){
		app->run();
	}
	delete app;
	return 0;
}