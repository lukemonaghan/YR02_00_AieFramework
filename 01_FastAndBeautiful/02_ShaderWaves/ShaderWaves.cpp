#include "ShaderWaves.h"
#include "Gizmos.h"
#include "Utilities.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

ShaderWaves::ShaderWaves(){
	fHeight = 1.0f;
}
ShaderWaves::~ShaderWaves(){ }

bool ShaderWaves::onCreate(int a_argc, char* a_argv[]) {

	//setup OpenGL stuffs
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Init the Gizmos
	Gizmos::create();

	//setup our matricies	
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(0,1,25),glm::vec3(25,0,25), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.3f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.01f, 10000.0f);
	
	//Load images
	WaterTexture = new Texture("textures/02_ShaderWaves/Water.png",SOIL_LOAD_AUTO);
	SandTexture = new Texture("textures/02_ShaderWaves/Sand.png",SOIL_LOAD_AUTO);

	//Import and setup our water shader
	WaterShader = new Shader("Shaders/02_Water.vert","Shaders/02_Water.frag");
	WaterShader->SetAttribs(3,0,"Position",1,"Colour",2,"TexCoord1");
	WaterShader->SetUniform("Projection","m4fv",1,false,glm::value_ptr(m_projectionMatrix));
	WaterShader->SetUniform("Height","1f",1,fHeight);
	WaterShader->SetTexture("DiffuseTexture",0,WaterTexture->ID());

	//import and setup our Basic Texture shader
	TextureShader = new Shader("Shaders/04_BasicTexture.vert","Shaders/04_BasicTexture.frag");
	TextureShader->SetAttribs(3,0,"Position",1,"Colour",7,"TexCoord1");
	TextureShader->SetUniform("Projection","m4fv",1,false,glm::value_ptr(m_projectionMatrix));
	TextureShader->SetTexture("DiffuseTexture",1,SandTexture->ID());

	//Setup the planes, position, size, subdivisions,tile,tileval
	WaterPlane = new Plane(glm::vec3(0,0,0)	,glm::vec2(50,50),	glm::vec2(500,500)	,true,0.1f);
	SandPlane = new Plane(glm::vec3(0,-1.0f,0)	,glm::vec2(50,50),	glm::vec2(1,1)	,true,100.0f);

	//yay we did it!
	return true;
}

void ShaderWaves::onUpdate(float a_deltaTime) {
	//Check for ESC key to quit program
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS){ quit(); }
	//check for holding F1 to enable wireframe
	if (glfwGetKey(m_window,GLFW_KEY_F1) == GLFW_PRESS){ glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ); }else{ glPolygonMode( GL_FRONT_AND_BACK, GL_FILL ); }

	//update the view
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );

	m_viewMatrix = glm::inverse( m_cameraMatrix );
	TextureShader->SetUniform("View","m4fv",1,false,glm::value_ptr(m_viewMatrix));
	WaterShader->SetUniform("View","m4fv",1,false,glm::value_ptr(m_viewMatrix));

	Gizmos::clear();
	Gizmos::addTransform(glm::mat4(1),1);

	WaterShader->SetUniform("Time","1f",1,Utility::getTotalTime());
}

void ShaderWaves::onDraw() {
	//clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
//DRAW
	//Set the shader to the default textured one and draw our sand
	TextureShader->Bind();
	SandPlane->Draw();
	//Set the shader to the water one and draw our water
	WaterShader->Bind();
	WaterPlane->Draw();
//DRAW
	//Reset the shader, Just incase
	Shader::Unbind();
	//setup our boring black walls
	Gizmos::addAABBFilled(glm::vec3(0,0,25),glm::vec3(1,fHeight,25),glm::vec4(0,0,0,1));
	Gizmos::addAABBFilled(glm::vec3(50,0,25),glm::vec3(1,fHeight,25),glm::vec4(0,0,0,1));
	Gizmos::addAABBFilled(glm::vec3(25,0,0),glm::vec3(25,fHeight,1),glm::vec4(0,0,0,1));
	Gizmos::addAABBFilled(glm::vec3(25,0,50),glm::vec3(25,fHeight,1),glm::vec4(0,0,0,1));
	//DRAW our boring walls
	Gizmos::draw(m_viewMatrix, m_projectionMatrix);
}

void ShaderWaves::onDestroy(){ Gizmos::destroy(); }

// main that controls the creation/destruction of an application
int main(int argc, char* argv[]){
	Application* app = new ShaderWaves();
	if (app->create("AIE - 02_ShaderWaves",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
	delete app;
	return 0;
}

