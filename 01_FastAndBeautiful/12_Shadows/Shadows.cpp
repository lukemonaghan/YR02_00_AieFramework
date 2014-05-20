#include "Shadows.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

bool Shadows::onCreate(int a_argc, char* a_argv[]) {
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Gizmos::create();
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );
	for ( int i = 0 ; i < 21 ; ++i ) {
		Gizmos::addLine( glm::vec3(-10 + i, 0.001f, 10), glm::vec3(-10 + i, 0.001f, -10),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );	
		Gizmos::addLine( glm::vec3(10, 0.001f, -10 + i), glm::vec3(-10, 0.001f, -10 + i),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}

	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(1,2,0),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, Utility::Width/(float)Utility::Height, 0.1f, 1000.0f);

	sShadowClass = new Shadow("models/tanks/antiair_tank.fbx",FBXFile::UNITS_METER);
	sShadowClass->create2DQuad(Utility::Width,Utility::Height);

	shdrScene = new Shader("Shaders/ShadowsModel.vert","Shaders/ShadowsModel.frag");
	//shdrScene->SetAttribs(3,0,"Position",1,"Normals",2,"TexCoord");

	shdrCam = new Shader("Shaders/Screen.vert","Shaders/Screen.frag");
	shdrCam->SetAttribs(2,0,"Position",1,"TexCoord1");

	shdrDepth = new Shader("Shaders/DepthShader.vert","Shaders/DepthShader.frag");
	shdrDepth->SetAttribs(1,0,"Position");

	pFloor = new Plane(glm::vec3(-10,0,-10),glm::vec2(20,20),glm::vec2(1),true,20.0f);

	tFloor = new Texture("textures/Grass.png");
	return true;
}

void Shadows::onUpdate(float a_deltaTime) {
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );

	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS){quit();}
}

void Shadows::onDraw() {
	sShadowClass->setUpLightAndShadowMatrix(shdrDepth);
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	shdrScene->SetUniform("ambientLight","3fv",1,glm::value_ptr(glm::vec3(0.5f,0.5f,0.5f)));

	glBindFramebuffer(GL_FRAMEBUFFER, 0); 
	Clear();
	sShadowClass->drawScene(shdrScene,Utility::Width,Utility::Height,m_cameraMatrix,m_projectionMatrix);
	sShadowClass->displayShadowMap(shdrCam);

	shdrScene->SetUniform("world","m4fv",1,false,glm::value_ptr(glm::mat4(1)));
	shdrScene->SetTexture("diffuseMap",0,tFloor->ID());
	pFloor->Draw();
	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void Shadows::onDestroy(){
	Gizmos::destroy();
}

int main(int argc, char* argv[]) {
	Application* app = new Shadows();
	
	if (app->create("AIE - 12_Shadows",Utility::Width,Utility::Height,argc,argv) == true){
		app->run();
	}
	delete app;
	return 0;
}