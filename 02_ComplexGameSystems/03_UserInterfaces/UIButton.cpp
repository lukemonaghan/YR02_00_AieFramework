#include "UIButton.h"
#include <GLFW\glfw3.h>

UIButton::UIButton(unsigned int a_shader, 
				   const glm::vec2 & a_dimentions, 
				   const glm::vec4 &a_normalcolour, 
				   const glm::vec4 &a_normalpressed, 
				   const glm::vec4 &a_normalhover)
				   : UIRectangle(a_shader,a_dimentions) {
	m_normalColour = a_normalcolour;
	m_pressedColour = a_normalpressed;
	m_hoverColour = a_normalhover;

	m_pressed = false;

	m_pressedCB = nullptr;
	m_pressedUD = nullptr;
}

void UIButton::update(float a_deltatime){

	GLFWwindow *window = glfwGetCurrentContext();

	int w = 0,	 h = 0;
	glfwGetWindowSize(window,&w,&h);

	double x, y;
	glfwGetCursorPos(window,&x,&y);

	glm::vec3 cursor = glm::vec3(x,h - y, 1);

	cursor = glm::inverse(getTransform()) * cursor;

	glm::vec2 shd = m_dimentions * m_Size * 0.5f;

	if (cursor.x >= -shd.x && cursor.x <=  shd.x && cursor.y >= -shd.y && cursor.y <=  shd.y) {
		m_colour = m_hoverColour;
		if (glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_1) == GLFW_PRESS){
			m_pressed = true;
			m_colour = m_pressedColour;
		}else{
			if (m_pressed){
				m_pressed = false;
				if (m_pressedCB != nullptr){
					m_pressedCB(this,m_pressedUD);
				}
			}
		}
	}else{
		m_pressed = false;
		m_colour = m_normalColour;
	}

	UIRectangle::update(a_deltatime);
}
