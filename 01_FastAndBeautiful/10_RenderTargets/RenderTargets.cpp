#include "RenderTargets.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

bool RenderTargets::onCreate(int a_argc, char* a_argv[]) {
	
	glClearColor(0.25f,0.25f,0.25f,0.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	Gizmos::create();
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );
	for ( int i = 0 ; i < 21 ; ++i ) {
		Gizmos::addLine( glm::vec3(-10 + i, 0.1f, 10), glm::vec3(-10 + i, 0.1f, -10),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );	
		Gizmos::addLine( glm::vec3(10, 0.1f, -10 + i), glm::vec3(-10, 0.1f, -10 + i),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, Utility::Width/(float)Utility::Height, 0.1f, 1000.0f);

	fbTexture = new RenderTarget(Utility::Width,Utility::Height);

	fbxm_SoulSpear = new FBXModel("models/soulspear/soulspear.fbx",FBXFile::UNITS_CENTIMETER);
	fbxm_SoulSpear->Update();

	p_Floor = new Plane(glm::vec3(-10,0,-10),glm::vec2(20),glm::vec2(1),true);

	s_FBX = new Shader("Shaders/02_FBX.vert","Shaders/02_FBX.frag");
	s_FBX->SetAttribs(3,0,"Position",1,"Normal",7,"TexCoord1");
	s_FBX->SetUniform("Projection","m4fv",1,false,glm::value_ptr(m_projectionMatrix));

	s_Plane = new Shader("Shaders/04_BasicTexture.vert","Shaders/04_BasicTexture.frag");
	s_Plane->SetAttribs(3,0,"Position",1,"Colour",7,"TexCoord1");
	s_Plane->SetUniform("Projection","m4fv",1,false,glm::value_ptr(m_projectionMatrix));

	return true;
}

void RenderTargets::onUpdate(float a_deltaTime) {
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );

	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS){quit();}
}

void RenderTargets::onDraw() {

	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	glm::mat4 LightView = glm::lookAt(glm::vec3(5,15,5),glm::vec3(0), glm::vec3(0,1,0));

	fbTexture->StartDraw();
	Clear();

	s_FBX->SetUniform("View","m4fv",1,false,glm::value_ptr(LightView));
	fbxm_SoulSpear->Draw(s_FBX ,viewMatrix);

	fbTexture->EndDraw();
	Clear();

	s_Plane->SetUniform("View","m4fv",1,false,glm::value_ptr(viewMatrix));
	s_Plane->SetTexture("DiffuseTexture",0,fbTexture->GetTexture());
	p_Floor->Draw();

	s_FBX->SetUniform("View","m4fv",1,false,glm::value_ptr(viewMatrix));
	fbxm_SoulSpear->Draw(s_FBX ,viewMatrix);
	
	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void RenderTargets::onDestroy() {
	Gizmos::destroy();
}

int main(int argc, char* argv[]){
	Application* app = new RenderTargets();
	if (app->create("AIE - 10_RenderTargets",Utility::Width,Utility::Height,argc,argv) == true){
		app->run();
	}
	delete app;
	return 0;
}