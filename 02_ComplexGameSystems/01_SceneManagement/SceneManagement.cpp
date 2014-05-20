#include "SceneManagement.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

SceneManagement::SceneManagement(){}
SceneManagement::~SceneManagement(){}

int	SceneManagement::planeSphereTest(const glm::vec4& a_plane,const glm::vec3& a_center,float a_radius){

	float dist = glm::dot<float>(a_plane.xyz,a_center ) - a_plane.w;

	if (dist > a_radius){
		return 1;
	}else if (dist < -a_radius){
		return -1;
	}
	return 0;
}

void SceneManagement::getFrustrumPlanes(const glm::mat4& a_transform,glm::vec4* a_planes){
	// Right
	a_planes[0] = glm::vec4(a_transform[0][3] - a_transform[0][0],
							a_transform[1][3] - a_transform[1][0],
							a_transform[2][3] - a_transform[2][0],
							a_transform[3][3] - a_transform[3][0]);

	// Left
	a_planes[1] = glm::vec4(a_transform[0][3] + a_transform[0][0],
							a_transform[1][3] + a_transform[1][0],
							a_transform[2][3] + a_transform[2][0],
							a_transform[3][3] + a_transform[3][0]);

	// Top
	a_planes[2] = glm::vec4(a_transform[0][3] - a_transform[0][1],
							a_transform[1][3] - a_transform[1][1],
							a_transform[2][3] - a_transform[2][1],
							a_transform[3][3] - a_transform[3][1]);

	// Bottom
	a_planes[3] = glm::vec4(a_transform[0][3] + a_transform[0][1],
							a_transform[1][3] + a_transform[1][1],
							a_transform[2][3] + a_transform[2][1],
							a_transform[3][3] + a_transform[3][1]);

	// Far
	a_planes[4] = glm::vec4(a_transform[0][3] - a_transform[0][2],
							a_transform[1][3] - a_transform[1][2],
							a_transform[2][3] - a_transform[2][2],
							a_transform[3][3] - a_transform[3][2]);

	// Near
	a_planes[5] = glm::vec4(a_transform[0][3] + a_transform[0][2],
							a_transform[1][3] + a_transform[1][2],
							a_transform[2][3] + a_transform[2][2],
							a_transform[3][3] + a_transform[3][2]);
}

bool SceneManagement::onCreate(int a_argc, char* a_argv[]) {
	Gizmos::create();
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	return true;
}

void SceneManagement::onUpdate(float a_deltaTime) {
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );
	Gizmos::clear();
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );
	for ( int i = 0 ; i < 21 ; ++i ) {
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );	
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}

//!----- TUTORIAL

	glm::vec3 center = glm::vec3(-5,0,0);
	float rad = 0.5f;

	Gizmos::addSphere(center,4,4,rad,glm::vec4(1));

	glm::vec4 planes[6];

	getFrustrumPlanes(m_projectionMatrix,planes);

	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );

	center = (viewMatrix * glm::vec4(center,1)).xyz;

	for (int i = 0; i < 6; i++){
		if (planeSphereTest(planes[i],center,rad) < 0){
			printf("Behind\n");
			break;
		}

		if (i == 5){
			printf("Visible\n");
		}
	}

	// OLD

	glm::vec4 plane = glm::vec4(1,0,0,-4.75f);
	
	int side = planeSphereTest(plane,center,rad);
	
	if (side < 0){
		printf("Behind\n");
	}else if (side > 0){
		printf("Infront\n");
	}else{
		printf("Inside\n");
	}


//!----- TUTORIAL

	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void SceneManagement::onDraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void SceneManagement::onDestroy() {
	Gizmos::destroy();
}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Application* app = new SceneManagement();
	
	if (app->create("AIE - 01_SceneManagement",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	// explicitly control the destruction of our application
	delete app;

	return 0;
}