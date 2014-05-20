#pragma once

#include "UIElement.h"

class UIRectangle  : public UIElement {
public:
	UIRectangle(unsigned int a_Shader,const glm::vec2& a_dimentions = glm::vec2(1,1),const glm::vec4& a_colour = glm::vec4(1));
	virtual ~UIRectangle();

	virtual void draw(const glm::mat4& a_Projection);
	virtual void update(float a_Deltatime);

	void		setTexture(unsigned int a_texture) {m_texture = a_texture;}

protected:
	unsigned int	m_shader;
	unsigned int	m_vao,m_vbo;

	glm::vec2		m_dimentions;
	glm::vec4		m_colour;

	unsigned int	m_texture;
	glm::vec2		m_textureCoordinates[4];
};

