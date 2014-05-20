#include "PerPixelLighting.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include "Shader.h"
#include "FBXModel.h"
#include "Lights.h"
#include <Skybox.h>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

Shader *LightShader,*m_Shader;
FBXModel *Model;
PointLight *GreenPoint,*RedPoint,*BluePoint;
SpotLight *GreenSpot,*RedSpot,*BlueSpot;
DirectionalLight *SunLight;
Skybox *m_Skybox;
CubeMap *m_CubeMap;

PerPixelLighting::PerPixelLighting(){}
PerPixelLighting::~PerPixelLighting(){}

//stackoverflow.com/questions/10628538/simple-glsl-spotlight-shader

bool PerPixelLighting::onCreate(int a_argc, char* a_argv[]) {
	// initialise the Gizmos helper class
	Gizmos::create();
	// create a world-space matrix for a camera
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 2000.0f);
	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.2f,0.4f,0.6f,1);//blueish sky
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	LightShader = new Shader("Shaders/05_Lighting.vert","Shaders/05_Lighting.frag");
	LightShader->SetAttribs(4,0,"Position",1,"Colour",2,"Normal",7,"TexCoord1");
	LightShader->SetUniform("Projection","m4fv",1,false,glm::value_ptr(m_projectionMatrix));

	//Model = new FBXModel("Models/soulspear/soulspear.fbx", FBXFile::UNITS_CENTIMETER);	//bump spec map support
	Model = new FBXModel("Models/ruinedtank/tank.fbx", FBXFile::UNITS_CENTIMETER);
	
	LightShader->SetUniform("LightSpecular","3fv",1,&glm::vec3(1));

	//Setup Ambient Light
	LightShader->SetUniform("LightAmbient","3fv",1,&glm::vec3(0.05));

	//DirectionalLights
	SunLight = new DirectionalLight(glm::vec3(1,1,0),glm::vec4(0.6,0.3,0,0));

	LightShader->SetUniform("DirectionalLightDirection","3fv",1,&SunLight->Direction);
	LightShader->SetUniform("DirectionalLightColour","4fv",1,&SunLight->Colour);
	LightShader->SetUniform("DirectionalLightCount","1i",1,1);

	//PointLights
	GreenPoint	= new PointLight(glm::vec3(0,5,0),10,glm::vec4(0,1,0,1));
	RedPoint	= new PointLight(glm::vec3(0,5,5),10,glm::vec4(1,0,0,1));
	BluePoint	= new PointLight(glm::vec3(5,5,0),10,glm::vec4(0,0,1,1));

	LightShader->SetUniform("PointLightDistance","1fv",3,&GreenPoint->fDistance,&RedPoint->fDistance,&BluePoint->fDistance);
	LightShader->SetUniform("PointLightColour","4fv",3,&GreenPoint->Colour,&RedPoint->Colour,&BluePoint->Colour);
	LightShader->SetUniform("PointLightCount","1i",1,3);

	//SpotLights
	GreenSpot	= new SpotLight(glm::vec3(-5,2,-5)	,glm::vec3(1,-1,0)	,20,45,glm::vec4(0,1,0,1));
	RedSpot		= new SpotLight(glm::vec3(5,2,-5)	,glm::vec3(-1,-1,0)	,20,45,glm::vec4(1,0,0,1));
	BlueSpot	= new SpotLight(glm::vec3(-5,2,5)	,glm::vec3(1,-1,1)	,20,45,glm::vec4(0,0,1,1));

	LightShader->SetUniform("SpotLightDirection","3fv",3,&GreenSpot->Direction,&RedSpot->Direction,&BlueSpot->Direction);
	LightShader->SetUniform("SpotLightAngle","1fv",3,&GreenSpot->fAngle,&RedSpot->fAngle,&BlueSpot->fAngle);
	LightShader->SetUniform("SpotLightDistance","1fv",3,&GreenSpot->fDistance,&RedSpot->fDistance,&BlueSpot->fDistance);
	LightShader->SetUniform("SpotLightColour","4fv",3,&GreenSpot->Colour,&RedSpot->Colour,&BlueSpot->Colour);
	LightShader->SetUniform("SpotLightCount","1i",1,3);

	m_Shader = new Shader("Shaders/07_CubeMap.vert","Shaders/07_CubeMap.frag");
	m_Shader->SetAttribs(1,0,"Position");

	m_Skybox = new Skybox(20);
	m_CubeMap = new CubeMap("textures/SkyBox/desert3/",".png");

	return true;
}

void PerPixelLighting::onUpdate(float a_deltaTime) {
	// update our camera matrix using the keyboard/mouse
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );
	LightShader->SetUniform("cameraPosition","3fv",1,&m_cameraMatrix[3].xyz);

	// clear all gizmos from last frame
	Gizmos::clear();
	
	// add an identity matrix gizmo
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );

	// add a grid on the XZ-plane
	int size = 40;
	for ( int i = 0 ; i <= size ; ++i ) {
		Gizmos::addLine( glm::vec3(-size/2 + i, 0, size/2 ), glm::vec3(-size/2 + i, 0, -size/2),  i == size/2 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
		Gizmos::addLine( glm::vec3( size/2, 0, -size/2 + i), glm::vec3(-size/2, 0, -size/2 + i),  i == size/2 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}

	// quit our application when escape is pressed
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS) quit();

	Model->Update();

	if (glfwGetKey(m_window,GLFW_KEY_F1) == GLFW_PRESS)SunLight->Update(m_window,a_deltaTime);
	LightShader->SetUniform("DirectionalLightDirection","3fv",1,&SunLight->Direction);

	if (glfwGetKey(m_window,GLFW_KEY_F2) == GLFW_PRESS)	GreenPoint->Update(m_window,a_deltaTime);
	if (glfwGetKey(m_window,GLFW_KEY_F3) == GLFW_PRESS)	RedPoint->Update(m_window,a_deltaTime);
	if (glfwGetKey(m_window,GLFW_KEY_F4) == GLFW_PRESS) BluePoint->Update(m_window,a_deltaTime);
	LightShader->SetUniform("PointLightPosition","3fv",3,&GreenPoint->Position,&RedPoint->Position,&BluePoint->Position);

	if (glfwGetKey(m_window,GLFW_KEY_F5) == GLFW_PRESS) GreenSpot->Update(m_window,a_deltaTime);
	if (glfwGetKey(m_window,GLFW_KEY_F6) == GLFW_PRESS) RedSpot->Update(m_window,a_deltaTime);
	if (glfwGetKey(m_window,GLFW_KEY_F7) == GLFW_PRESS) BlueSpot->Update(m_window,a_deltaTime);
	LightShader->SetUniform("SpotLightPosition","3fv",3,&GreenSpot->Position,&RedSpot->Position,&BlueSpot->Position);
}

void PerPixelLighting::onDraw() {
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );

	if (!glfwGetKey(m_window,GLFW_KEY_F12)){
		Shader::Unbind();
		SunLight->Draw();
		GreenPoint->Draw();
		RedPoint->Draw();
		BluePoint->Draw();
		GreenSpot->Draw();
		RedSpot->Draw();
		BlueSpot->Draw();
		Gizmos::draw(viewMatrix, m_projectionMatrix);
	}
	LightShader->SetUniform("View","m4fv",1,false,glm::value_ptr(viewMatrix));
	Model->Draw(LightShader,viewMatrix);
	m_Skybox->Draw(m_CubeMap,m_Shader,(m_projectionMatrix * viewMatrix));
}

void PerPixelLighting::onDestroy() {
	// clean up anything we created
	Gizmos::destroy();
}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[]) {
	// explicitly control the creation of our application
	Application* app = new PerPixelLighting();
	if (app->create("AIE - PerPixelLighting",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true) app->run();	
	// explicitly control the destruction of our application
	delete app;
	return 0;
}