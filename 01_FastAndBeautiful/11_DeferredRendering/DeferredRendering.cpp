#include "DeferredRendering.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

bool DeferredRendering::onCreate(int a_argc, char* a_argv[]) {
	glClearColor(0.25f,0.25f,0.25f,0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	Gizmos::create();
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );
	for ( int i = 0 ; i < 21 ; ++i ) {
		Gizmos::addLine( glm::vec3(-10 + i, 0.1f, 10), glm::vec3(-10 + i, 0.1f, -10), i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );	
		Gizmos::addLine( glm::vec3(10, 0.1f, -10 + i), glm::vec3(-10, 0.1f, -10 + i),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, Utility::Width/(float)Utility::Height, 0.1f, 50.0f);

	fbBuff = new FrameBuffer(3,Utility::Width,Utility::Height);

	fbxmModel = new FBXModel("models/soulspear/soulspear.fbx",FBXFile::UNITS_CENTIMETER);
	fbxmModel->Update();

	sShader = new Shader("shaders/11_DeferredRendering.vert","shaders/11_DeferredRendering.frag");
	sShader->SetAttribs(3,0,"Position",2, "Normal",7, "TexCoord");
	sShader->SetFragOuts(3,0,"oDiffuse",1,"oNormal",2,"oWorld");
	sShader->SetUniform("Projection","m4fv",1,false,glm::value_ptr(m_projectionMatrix));

	s_Plane = new Shader("Shaders/Screen.vert","Shaders/Screen.frag");	
	s_Plane->SetAttribs(2,0,"Position",2,"TexCoord1");

	HuDTex = new HUDTexture(glm::vec2(Utility::Width/2,Utility::Height/2),glm::vec2(Utility::Width,Utility::Height));
	return true;
}

void DeferredRendering::onUpdate(float a_deltaTime) {
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );

	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS){quit();}
}

void DeferredRendering::onDraw() {
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	sShader->SetUniform("View","m4fv",1,false,glm::value_ptr(viewMatrix));
	
	fbBuff->Bind();	
	Clear();
	
	fbxmModel->Draw(sShader,viewMatrix);

	FrameBuffer::Unbind();
	Clear();

	if (glfwGetKey(m_window,GLFW_KEY_F1)){
		s_Plane->SetTexture("DiffuseTexture",0,fbBuff->GetTexture(1));
		HuDTex->Draw();
	}else if (glfwGetKey(m_window,GLFW_KEY_F2)){
		s_Plane->SetTexture("DiffuseTexture",0,fbBuff->GetTexture(2));
		HuDTex->Draw();
	}else if (glfwGetKey(m_window,GLFW_KEY_F3)){
		s_Plane->SetTexture("DiffuseTexture",0,fbBuff->GetTexture(3));
		HuDTex->Draw();
	}
	s_Plane->SetTexture("DiffuseTexture",0,fbBuff->GetTexture(0));
	HuDTex->Draw();
}

void DeferredRendering::onDestroy() {
	Gizmos::destroy();
}

int main(int argc, char* argv[]) {
	Application* app = new DeferredRendering();
	
	if (app->create("AIE - 11_DeferredRendering",Utility::Width,Utility::Height,argc,argv) == true){
		app->run();
	}
	delete app;

	return 0;
}