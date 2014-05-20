#include "GameGenreCaseStudy2.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

//--TUTORIAL

void GameGenreCaseStudy2::setupSelectionBox()
{
	// load a simple shader and create a unit-size quad

	unsigned int vs = Utility::loadShader("shaders/12_GameGenreCaseStudy2.vert", GL_VERTEX_SHADER);
	unsigned int fs = Utility::loadShader("shaders/12_GameGenreCaseStudy2.frag", GL_FRAGMENT_SHADER);
	m_selectionShader = Utility::createProgram(vs,0,0,0,fs);
	glDeleteShader(vs);
	glDeleteShader(fs);

	glUseProgram(m_selectionShader);
	glUniform2f( glGetUniformLocation(m_selectionShader, "screenDimensions"), DEFAULT_SCREENWIDTH, DEFAULT_SCREENHEIGHT );
	glUseProgram(0);

	glm::vec2 vertices[4] = {
		glm::vec2(-.5f,.5f), glm::vec2(-.5f,-.5f),
		glm::vec2(.5f,.5f), glm::vec2(.5f,-.5f)
	};

	glGenVertexArrays(1, &m_selectionVAO);
	glBindVertexArray(m_selectionVAO);

	glGenBuffers(1, &m_selectionVBO);
	glBindBuffer(GL_ARRAY_BUFFER,m_selectionVBO);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec2), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0);
	glBindVertexArray(0);
}

void GameGenreCaseStudy2::drawSelectionBox(const glm::vec2& a_start, const glm::vec2& a_end)
{
	glUseProgram(m_selectionShader);

	// get the center of the selection box
	// note: need to invert Y
	int location = glGetUniformLocation(m_selectionShader, "center");
	glUniform2f(location, (a_start.x + a_end.x) * 0.5f, DEFAULT_SCREENHEIGHT - (a_start.y + a_end.y) * 0.5f);

	// get the size of the selection box
	location = glGetUniformLocation(m_selectionShader, "extents");
	glUniform2f(location, abs(a_start.x - a_end.x), abs(a_start.y - a_end.y));

	glBindVertexArray(m_selectionVAO);

	// draw with blending enabled
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDisable(GL_BLEND);
}

glm::vec3	GameGenreCaseStudy2::getPickRayDirection(){
	double x = 0, y = 0;
	glfwGetCursorPos(m_window,&x, &y);

	glm::vec3 screenPos ( x / DEFAULT_SCREENWIDTH * 2 - 1, (y / DEFAULT_SCREENHEIGHT * 2 - 1) * -1, -1);

	screenPos.x /= m_projectionMatrix[0][0];
	screenPos.y /= m_projectionMatrix[1][1];

	return (glm::normalize(m_cameraMatrix * glm::vec4(screenPos,0) ).xyz());

}

glm::vec3	GameGenreCaseStudy2::rayPlaneIntersection(const glm::vec3& a_start,const glm::vec3& a_dir, const glm::vec4& a_plane){
	float d = (a_plane.w - glm::dot(a_start, a_plane.xyz())) / glm::dot(a_dir, a_plane.xyz());
	return (a_start + a_dir * d);
}

//--TUTORIAL

GameGenreCaseStudy2::GameGenreCaseStudy2(){}

GameGenreCaseStudy2::~GameGenreCaseStudy2(){}

bool GameGenreCaseStudy2::onCreate(int a_argc, char* a_argv[]) {
	Gizmos::create();
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//--TUTORIAL

	setupSelectionBox();

	m_mouseDown = false;

	//--TUTORIAL

	return true;
}

void GameGenreCaseStudy2::onUpdate(float a_deltaTime) {
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );
	Gizmos::clear();
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );
	for ( int i = 0 ; i < 21 ; ++i ){
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}

	//--TUTORIAL

	if (glfwGetMouseButton(m_window,GLFW_MOUSE_BUTTON_3) == GLFW_PRESS){
		glm::vec3 rayDir = getPickRayDirection();
		glm::vec4 plane (0,1,0,0);
		m_target = rayPlaneIntersection(m_cameraMatrix[3].xyz,rayDir,plane);
	}

	Gizmos::addAABBFilled(m_target,glm::vec3(0.5f),glm::vec4(1,1,0,1));

	if (glfwGetMouseButton(m_window,GLFW_MOUSE_BUTTON_1) == GLFW_PRESS){
		if (m_mouseDown == false){
			double x = 0, y = 0;
			glfwGetCursorPos(m_window,&x,&y);

			m_mouseDownPos.x = (float)x;
			m_mouseDownPos.y = (float)y;
		}
		m_mouseDown = true;
	} else {
		m_mouseDown = false;
	}

	//--TUTORIAL

	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void GameGenreCaseStudy2::onDraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	Gizmos::draw(viewMatrix, m_projectionMatrix);
	
	//--TUTORIAL

	if (m_mouseDown == true){
		double x = 0, y = 0;
		glfwGetCursorPos(m_window,&x,&y);
		drawSelectionBox(m_mouseDownPos,glm::vec2(x,y));
	}

	//--TUTORIAL

}

void GameGenreCaseStudy2::onDestroy() {

	//--TUTORIAL
	glDeleteProgram(m_selectionShader);
	glDeleteVertexArrays(1, &m_selectionVAO);
	glDeleteBuffers(1, &m_selectionVBO);
	//--TUTORIAL

	Gizmos::destroy();
}

int main(int argc, char* argv[]) {
	Application* app = new GameGenreCaseStudy2();
	if (app->create("AIE - 12_GameGenreCaseStudy 2",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();

	delete app;
	return 0;
}

