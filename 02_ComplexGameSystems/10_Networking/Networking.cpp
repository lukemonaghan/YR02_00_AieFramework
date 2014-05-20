#include "Networking.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

Networking::Networking(){}
Networking::~Networking(){}

//--TUTORIAL

#include "Client.h"
#include "Server.h"

#include <Shader.h>
#include <HUDItems.h>
#include <Texture.h>

BitFont *bfArial;
Button *bClient, *bServer,*bBoth;
Texture *tBar;

Shader *sScreen;
char	cServerClient;

Server *sServer;
Client *cClient;

commandlist cList;

int myplayerid = 0;

void Networking::MainLoop(){
	if (bClient->Update()){
		cServerClient = 2;
	}
	if (bServer->Update()){
		cServerClient = 1;
	}
	if (bBoth->Update()){
		cServerClient = 3;
	}
}

void Networking::ServerLoop(){
	if (sServer == nullptr){
		sServer = new Server(10,1337);
	}

	returnlist clReturn = sServer->Update(cList);
	for (returnlist::iterator it = clReturn.begin(); it != clReturn.end(); it++){
		if (it->ID == STARTID+1){
			float val = atof(it->information);
			char buffer[64];
			sprintf(buffer,"%f",val);
			printf("SERVER: %s %s \n",buffer,it->ADDRESS.ToString());
			sServer->Send(it->ADDRESS,STARTID+2,it->information,HIGH_PRIORITY,RELIABLE_ORDERED,0,false);
		}
	}
}

void Networking::ClientLoop(){
	if (cClient == nullptr){
		cClient = new Client(1337);//,"10.10.24.7");
	}

	returnlist clReturn = cClient->Update(cList);

	if (cClient->Failed()){
		cServerClient = 0;
		delete cClient;
		cClient = nullptr;
		return;
	}
	GLFWwindow* window = glfwGetCurrentContext();

	static bool bPressed = false;
	if (glfwGetKey(window,GLFW_KEY_F1)){
		if (bPressed == false){
			bPressed = true;
			int in = rand()%100000;
			char buffer[64];
			itoa(in,buffer,10);
			cClient->Send(buffer,STARTID+1);
		}
	} else if (!glfwGetKey(window,GLFW_KEY_F1) && bPressed == true){
		bPressed = false;
	}

	for (returnlist::iterator it = clReturn.begin(); it != clReturn.end(); it++){
		if (it->ID == STARTID+2){
			printf("CLIENT: %s %s \n",it->information,it->ADDRESS.ToString());
		}
	}
}

void Networking::MainDraw(){
	bfArial->Draw(sScreen);
	sScreen->SetTexture("DiffuseTexture",1,tBar->ID());
	bClient->Draw();
	bServer->Draw();
	bBoth->Draw();
}

void Networking::ServerDraw(){}
void Networking::ClientDraw(){}

//--TUTORIAL

bool Networking::onCreate(int a_argc, char* a_argv[]) {

	Gizmos::create();

	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);

	srand(time(NULL));
	glClearColor(rand()%100*0.01f,rand()%100*0.01f,rand()%100*0.01f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//--TUTORIAL

	tBar			= new Texture("textures/Bar.png");
	
	bfArial			= new BitFont("fonts/CourierNew.fnt");

	bClient			= new Button(glm::vec2(Utility::Width/2 ,Utility::Height/2 -  70),glm::vec2(400,40));
	bfArial->AddString("Client",Utility::Width/2 ,Utility::Height/2 -  70 - 20,0,true);

	bServer			= new Button(glm::vec2(Utility::Width/2 ,Utility::Height/2 +  70),glm::vec2(400,40));
	bfArial->AddString("Server",Utility::Width/2 ,Utility::Height/2 +  70 - 20,0,true);

	bBoth			= new Button(glm::vec2(Utility::Width/2 ,Utility::Height/2),glm::vec2(400,40));
	bfArial->AddString("Both",Utility::Width/2 ,Utility::Height/2 - 20,0,true);

	sScreen			= new Shader("shaders/Screen.vert","shaders/Screen.frag");
	sScreen->SetAttribs(3,0,"Position",1,"Colour",2,"TexCoord1");

	bfArial->Update();

	cServerClient = 0;

	sServer = nullptr;
	cClient = nullptr;
	
	cList.push_back(STARTID+1);
	cList.push_back(STARTID+2);

	//--TUTORIAL

	return true;
}

void Networking::onUpdate(float a_deltaTime) {
	Gizmos::clear();
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );
	for ( int i = 0 ; i < 21 ; ++i ) {
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}

	//--TUTORIAL

	switch(cServerClient){
	case 0:
		MainLoop();
		break;
	case 1:
		ServerLoop();
		break;
	case 2:
		ClientLoop();
		break;
	case 3:
		ServerLoop();
		ClientLoop();
		break;
	}

	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );

	//--TUTORIAL

	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS) { quit(); }
}

void Networking::onDraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );

	//--TUTORIAL

	switch(cServerClient){
	case 0:
		MainDraw();
		break;
	case 1:
		ServerDraw();
		break;
	case 2:
	case 3:
		ClientDraw();
		break;
	}

	//--TUTORIAL

	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void Networking::onDestroy() {
	Gizmos::destroy();
}

int main(int argc, char* argv[]) {
	Application* app = new Networking();
	if (app->create("AIE - 10_Networking",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true){
		app->run();
	}
	delete app;
	return 1+1-1-1;
}