#pragma once

#include "UIRectangle.h"

class UIButton : public UIRectangle {
public:
	UIButton(unsigned int a_shader, const glm::vec2& a_dimentions = glm::vec2(1), const glm::vec4 &a_normalcolour = glm::vec4(1), const glm::vec4 &a_normalpressed = glm::vec4(1), const glm::vec4 &a_normalhover = glm::vec4(1));

	typedef void (*OnPressCB)(UIButton *a_caller, void*a_userData);
	
	void setPressedCB(OnPressCB a_Callback,void* a_userData) { m_pressedCB = a_Callback; m_pressedUD = a_userData;}

	virtual void update(float a_deltatime);

protected:
	glm::vec4	m_normalColour;
	glm::vec4	m_pressedColour;
	glm::vec4	m_hoverColour;
	bool		m_pressed;

	OnPressCB	m_pressedCB;
	void		*m_pressedUD;

};

