#include "SolarSystem.h"
#include "Gizmos.h"
#include "Utilities.h"
#include "FBXFile.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#define DEFAULT_SCREENWIDTH 1280	
#define DEFAULT_SCREENHEIGHT 720

/*Rules
http://www.scalesolarsystem.66ghz.com/
*/

SolarSystem::SolarSystem(){
	UNITSCALE = 1000000.0f;
	fSpeed = 1.0f;
	//SUN
	Planets[0] = new Planet(1392000/2/UNITSCALE,0/UNITSCALE);
	Planets[0]->offset = glm::vec3(0/UNITSCALE,0.0f,0.0f);
	Planets[0]->SetColour(glm::vec4(1.0f,1.0f,0.0f,1.0f));
	//Mercury
	Planets[1] = new Planet(4800/2/UNITSCALE,172332/UNITSCALE);
	Planets[1]->offset = glm::vec3(57950000/UNITSCALE,0.0f,0.0f);
	Planets[1]->SetColour(glm::vec4(1.0f,0.2f,0.0f,1.0f));
	Planets[1]->distance = 57950000/UNITSCALE;
	//Venus
	Planets[2] = new Planet(12100/2/UNITSCALE,126071/UNITSCALE);
	Planets[2]->offset = glm::vec3(108110000/UNITSCALE,0.0f,0.0f);
	Planets[2]->SetColour(glm::vec4(1.0f,0.8f,0.5f,1.0f));
	Planets[2]->distance = 108110000/UNITSCALE;
	//Earth
	Planets[3] = new Planet(12700/2/UNITSCALE,107208/UNITSCALE);
	Planets[3]->offset = glm::vec3(149570000/UNITSCALE,0.0f,0.0f);
	Planets[3]->SetColour(glm::vec4(0.0f,0.2f,1.0f,1.0f));
	Planets[3]->distance = 149570000/UNITSCALE;
	//Mars
	Planets[4] = new Planet(6700/2/UNITSCALE,86677.7f/UNITSCALE);
	Planets[4]->offset = glm::vec3(227840000/UNITSCALE,0.0f,0.0f);
	Planets[4]->SetColour(glm::vec4(0.8f,0.8f,0.8f,1.0f));
	Planets[4]->distance = 227840000/UNITSCALE;
	//Jupiter
	Planets[5] = new Planet(142900/2/UNITSCALE,47050.8f/UNITSCALE);
	Planets[5]->offset = glm::vec3(778140000/UNITSCALE,0.0f,0.0f);
	Planets[5]->SetColour(glm::vec4(0.4f,0.4f,0.4f,1.0f));
	Planets[5]->distance = 778140000/UNITSCALE;
	//Saturn
	Planets[6] = new Planet(116438/2/UNITSCALE,34884.1f/UNITSCALE);
	Planets[6]->offset = glm::vec3(1427000000/UNITSCALE,0.0f,0.0f);
	Planets[6]->SetColour(glm::vec4(1.0f,1.0f,0.0f,1.0f));
	Planets[6]->distance = 1427000000/UNITSCALE;
	//Uranus
	Planets[7] = new Planet(46940/2/UNITSCALE,24516.7f/UNITSCALE);
	Planets[7]->offset = glm::vec3(2870300000/UNITSCALE,0.0f,0.0f);
	Planets[7]->SetColour(glm::vec4(0.1f,0.7f,0.8f,1.0f));
	Planets[7]->distance = 2870300000/UNITSCALE;
	//Neptune
	Planets[8] = new Planet(45400/2/UNITSCALE,19548.7f/UNITSCALE);
	Planets[8]->offset = glm::vec3(4499900000/UNITSCALE,0.0f,0.0f);
	Planets[8]->SetColour(glm::vec4(0.1f,0.7f,1.0f,1.0f));
	Planets[8]->distance = 4499900000/UNITSCALE;
	//Pluto
	Planets[9] = new Planet(2200/2/UNITSCALE,16798.3f/UNITSCALE);
	Planets[9]->offset = glm::vec3(5913000000/UNITSCALE,0.0f,0.0f);
	Planets[9]->SetColour(glm::vec4(0.1f,1.0f,1.0f,1.0f));
	Planets[9]->distance = 5913000000/UNITSCALE;
	//EARTHS MOON
	Moons[0] = new Planet(10/2/UNITSCALE,Planets[3]->fSpeed*(365.25f/27.322f),Planets[3]);//Luna
	Moons[0]->offset = glm::vec3(400000/UNITSCALE,0.0f,0.0f);
	Moons[0]->SetColour(glm::vec4(0.7f,0.7f,0.7f,1.0f));
	//MARS MOONS
	Moons[1] = new Planet(8/2/UNITSCALE,Planets[4]->fSpeed*(686.98f/1.263f),Planets[4]);//Deimos
	Moons[1]->offset = glm::vec3(23460/UNITSCALE,0.0f,0.0f);
	Moons[1]->SetColour(glm::vec4(0.7f,0.7f,0.7f,1.0f));

	Moons[2] = new Planet(24/2/UNITSCALE,Planets[4]->fSpeed*(686.98f/0.319f),Planets[4]);//Phobos
	Moons[2]->offset = glm::vec3(9270/UNITSCALE,0.0f,0.0f);
	Moons[2]->SetColour(glm::vec4(0.7f,0.7f,0.7f,1.0f));

	glmLookat = Planets[0];
}

SolarSystem::~SolarSystem(){}

void SolarSystem::DebugSizes(int iType){
	//Set all planets to sun size
	if (iType == 0 || iType == 2){
		for(int i = 1; i < COUNT_PLANETS;++i){
			Planets[i]->size = Planets[0]->size;
		}
	}
	//do same with moons
	if (iType == 0 || iType == 1){
		for(int i = 0; i < COUNT_MOONS;++i){
			Moons[i]->size = Planets[0]->size;
		}
	}
}

bool SolarSystem::onCreate(int a_argc, char* a_argv[]) {

	DebugSizes(0);//0 = all, 1 = planets, 2 = moons, anything else = null

	//Defaults
	Gizmos::create(100,16384*8);//10 lines, so we dont have lines on our planets
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glmLookat->position, glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.5f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 3.402823466e+38F/2);
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	//enables
	glEnable(GL_DEPTH_TEST);	//depth buffer
	glEnable(GL_CULL_FACE);		//dont draw backfaces

	return true;
}

void SolarSystem::onUpdate(float a_deltaTime) {
	// quit our application when escape is pressed
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE)){quit();}
	//Clear buffers
	Gizmos::clear();
	//Speed Controls
	if (glfwGetKey(m_window,'=') && !bAddPressed){//check if we are pressing +(= without shift) and if we are aloud
		bAddPressed = true;
		fSpeed *= 2.0f;
	}else if (glfwGetKey(m_window,'-')&& !bAddPressed){//else check if we are pressing - and if we are aloud
		bAddPressed = true;
		fSpeed *= 0.5f;
	}
	//reset bool
	if (!glfwGetKey(m_window,'=') && !glfwGetKey(m_window,'-') && bAddPressed){
		bAddPressed = false;
		std::cout << fSpeed << '\n';
	}
	//Planet Snapp pointer setting
	if (glfwGetKey(m_window,'0')){
		glmLookat = Planets[0];
	}else if (glfwGetKey(m_window,'1')){
		glmLookat = Planets[1];
	}else if (glfwGetKey(m_window,'2')){
		glmLookat = Planets[2];
	}else if (glfwGetKey(m_window,'3')){
		glmLookat = Planets[3];
	}else if (glfwGetKey(m_window,'4')){
		glmLookat = Planets[4];
	}else if (glfwGetKey(m_window,'5')){
		glmLookat = Planets[5];
	}else if (glfwGetKey(m_window,'6')){
		glmLookat = Planets[6];
	}else if (glfwGetKey(m_window,'7')){
		glmLookat = Planets[7];
	}else if (glfwGetKey(m_window,'8')){
		glmLookat = Planets[8];
	}else if (glfwGetKey(m_window,'9')){
		glmLookat = Planets[9];
	} 
//UPDATE
	for(int p = 0; p < COUNT_PLANETS;++p){
		Planets[p]->Update(a_deltaTime*fSpeed);
	}	
	for(int m = 0; m < COUNT_MOONS;++m){
		Moons[m]->Update(a_deltaTime*fSpeed);
	}
//UPDATE
	//setup WSAD free camera
	if (glfwGetKey(m_window,GLFW_KEY_LEFT_CONTROL)){//if holding control snap else freemove
		m_cameraMatrix = glm::inverse( glm::lookAt(glmLookat->position + glm::vec3(glmLookat->size*1.1f,0.5f,glmLookat->size*1.1f),glmLookat->position, glm::vec3(0,1,0)) );
	}else{
		Utility::freeMovement( m_cameraMatrix, a_deltaTime, 100.0f );
	}
}

void SolarSystem::onDraw() {
	//clear
	Clear();
	//Setup view
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );

	//Planet Lines toggle
	if (glfwGetKey(m_window,GLFW_KEY_F2) && !bPressed){
		bPressed = true;
		bf2 = !bf2;
	}

	//Moon Lines toggle
	if (glfwGetKey(m_window,GLFW_KEY_F3) && !bPressed){
		bPressed = true;
		bf3 = !bf3;
	}	
	//check if we are aloud to draw lines, if yes, add to gizmos
	if (bf2){
		for(int i = 0; i < COUNT_PLANETS;++i){
			Gizmos::addLine( glm::vec3(0,0,0), Planets[i]->position,  Planets[0]->colour, Planets[i]->colour);
		}
	}
	if (bf3){
		for(int i = 0; i < COUNT_MOONS;++i){
			Gizmos::addLine( Moons[i]->parent->position, Moons[i]->position,  Moons[i]->parent->colour,Moons[0]->colour);
		}
	}
	//Check released on both
	if (!glfwGetKey(m_window,GLFW_KEY_F3) && !glfwGetKey(m_window,GLFW_KEY_F2) && bPressed){
		bPressed = false;
	}
	//Planets
	for(int p = 0; p < COUNT_PLANETS;++p){
		Planets[p]->Draw();
	}	
	//Moons
	for(int m = 0; m < COUNT_MOONS;++m){
		Moons[m]->Draw();
	}
	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void SolarSystem::onDestroy(){Gizmos::destroy();}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[]){
	// explicitly control the creation of our application
	Application* app = new SolarSystem();
	if (app->create("AIE - SolarSystem",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();	
	// explicitly control the destruction of our application
	delete app;
	return 0;
}