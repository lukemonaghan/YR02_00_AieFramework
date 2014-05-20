#include "Player.h"
#include <Gizmos.h>
#include <GLFW\glfw3.h>

Player::Player(float a_speed){
	m_speed = a_speed;
}

Player::~Player(){

}

void Player::update(float a_deltaTime){
	GLFWwindow *window = glfwGetCurrentContext();

	if (glfwGetKey(window,'W') == GLFW_PRESS){
		m_transform[3] -= m_transform[2] * m_speed * a_deltaTime;
	}else if (glfwGetKey(window,'S') == GLFW_PRESS){
		m_transform[3] += m_transform[2] * m_speed * a_deltaTime;
	}

	if (glfwGetKey(window,'A') == GLFW_PRESS){
		m_transform *= glm::rotate( m_speed * a_deltaTime,0.0f,1.0f,0.0f);
	}else if (glfwGetKey(window,'D') == GLFW_PRESS){
		m_transform *= glm::rotate(-m_speed * a_deltaTime,0.0f,1.0f,0.0f);
	}
}

void Player::draw(){
	// Body
	Gizmos::addAABBFilled(m_transform[3].xyz + glm::vec3(0,  0.5f,0),glm::vec3(0.5f,0.25f,0.5f),glm::vec4(0.2f,0.5f,1.0f,0.1f),&m_transform);
	Gizmos::addAABBFilled(m_transform[3].xyz + glm::vec3(0,  0.75f,0),glm::vec3(0.5f,0.01f,0.5f),glm::vec4(1.0f,1.0f,1.0f,1.0f),&m_transform);

	Gizmos::addAABBFilled(m_transform[3].xyz + glm::vec3(0,0.125f,0),glm::vec3(0.6f,0.25f,1.0f),glm::vec4(1.0f,1.0f,1.0f,1.0f),&m_transform);
}

