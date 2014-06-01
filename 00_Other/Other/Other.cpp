#include "Other.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include "VertexBatch.hpp"
#include <Gizmos.h>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

#define CUBE_COUNT 100

Other::Other(){}
Other::~Other(){}

Osiris::VertexBatch *vb;

#define MAXDIMENS 100
unsigned int xNext = 0,yNext = 0,zNext = 0;

bool Other::onCreate(int a_argc, char* a_argv[]) {
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	sScene = new Shader("shaders/Gizmo/Gizmo.vert","shaders/Gizmo/Gizmo.frag",0,0,"shaders/Gizmo/Gizmo.geom");
	sScene->SetAttribs(5,0,"Position",1,"Information",2,"Colour",3,"Bitset",4,"Model");
	
	//!--TUTORIAL
	vb = new Osiris::VertexBatch();
	vb->SetShader(sScene->id);
	xNext = zNext = yNext = 0;
	//!--TUTORIAL
	return true;
}

int type = 0;
void Other::onUpdate(float a_deltaTime) {
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );

	//!--TUTORIAL
	static bool bf1 = false;
	if (glfwGetKey(m_window,GLFW_KEY_F1) == GLFW_PRESS) {
		if (!bf1){
			bf1 = true;
			printf("%i elements\n",vb->getCount());
			printf("%f\n",1/Utility::getDeltaTime());
		}
	}else{
		if (bf1){
			bf1 = false;
		}
	}

	int count = vb->getCount();
	#define MAX_LOOP 1000
	while (glfwGetKey(m_window,GLFW_KEY_F2) == GLFW_PRESS) {
		if (zNext > MAXDIMENS){zNext = 0;yNext++;}
		if (xNext > MAXDIMENS){xNext = 0;zNext++;}else{xNext++;}
		switch (type){
		case 0:
			vb->Add(new Osiris::Gizmo::Point(glm::vec3(xNext,yNext,zNext),glm::vec4(0,0,0,1) + glm::vec4(rand()%10 / 10.0f)));
			break;
		case 1:
			vb->Add(new Osiris::Gizmo::Box(glm::vec3(xNext,yNext,zNext),glm::vec3(0.5f),glm::vec4(0,0,0,1) + glm::vec4(rand()%10 / 10.0f)));
			break;
		case 2:
			vb->Add(new Osiris::Gizmo::Plane(glm::vec3(xNext,yNext,zNext),glm::vec3(0.5f),glm::vec4(0,0,0,1) + glm::vec4(rand()%10 / 10.0f)));
			break;
		case 3:
			vb->Add(new Osiris::Gizmo::Sphere(glm::vec3(xNext,yNext,zNext),0.5f,3,3,glm::vec4(0,0,0,1) + glm::vec4(rand()%10 / 10.0f)));
			break;
		case 4:
			vb->Add(new Osiris::Gizmo::Cylinder(glm::vec3(xNext,yNext,zNext),0.5f,1,8,glm::vec4(0,0,0,1) + glm::vec4(rand()%10 / 10.0f)));
			break;
		case 5:
			vb->Add(new Osiris::Gizmo::Ring(glm::vec3(xNext,yNext,zNext),0.5f,1.0f,8,glm::vec4(0,0,0,1) + glm::vec4(rand()%10 / 10.0f)));
			break;
		case 6:
			vb->Add(new Osiris::Gizmo::Disk(glm::vec3(xNext,yNext,zNext),0.5f,16,glm::vec4(0,0,0,1) + glm::vec4(rand()%10 / 10.0f)));
			break;		
		case 7:
			vb->Add(new Osiris::Gizmo::Cone(glm::vec3(xNext,yNext,zNext),0.5f,glm::vec4(0,0,0,1) + glm::vec4(rand()%10 / 10.0f)));
			break;
		}
		if (vb->getCount() > count + MAX_LOOP){vb->Update();break;}
	}

	if (glfwGetKey(m_window,GLFW_KEY_F3) == GLFW_PRESS) {
		const Osiris::GizmoMap *map = vb->getMap();
		if ((*map).size() != 0){
			size_t key = rand()%(*map).size();
			(*map).at(key)->move();
			vb->Update();
		}
	}

	static bool bF4 = false;
	if (glfwGetKey(m_window,GLFW_KEY_F4) == GLFW_PRESS) {
		if (!bF4){
			bF4 = true;
			if (type >= TYPE_COUNT){type = 0;}else{type++;}
			printf("%s \n",GETYPE(type));
		}
	}else{
		bF4 = false;
	}

	if (glfwGetKey(m_window,GLFW_KEY_F11) == GLFW_PRESS) {
		sScene->ReloadShader();
		sScene->SetAttribs(5,0,"Position",1,"Information",2,"Colour",3,"Bitset",4,"Model");
	}

	//!--TUTORIAL
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		quit();
	}

}
	
void Other::onDraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	//!--TUTORIAL
	sScene->SetUniform("ViewProjection","m4fv",1,false,glm::value_ptr(m_projectionMatrix * viewMatrix));
	vb->Draw();
	//!--TUTORIAL	
}

void Other::onDestroy() {

	//!--TUTORIAL
	delete vb;
	//!--TUTORIAL
}

int main(int argc, char* argv[]) {
	Application* app = new Other();
	
	if (app->create("AIE - Other",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true) {
		app->run();
	}
	delete app;
	return 0;
}

