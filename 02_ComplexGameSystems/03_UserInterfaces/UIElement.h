#pragma once

#include <glm\ext.hpp>
#include <vector>

class UIElement {
public:
	UIElement();
	virtual ~UIElement();

	void setPosition(const glm::vec2& a_Position)	{m_Position = a_Position;}
	void setSize(const glm::vec2& a_Size)			{m_Size		= a_Size;}
	void setRotation(float a_Rotation)				{m_Rotation = a_Rotation;}

	const glm::vec2& getPosition() const {return m_Position;}
	const glm::vec2& getSize() const {return m_Size;}
	const float&	getRotation() const {return m_Rotation;}

	void			addElement(UIElement *a_Element);

	glm::mat3		getTransform() const;

	virtual void	draw(const glm::mat4& a_Projection);

	virtual void	update(float a_Deltatime);

protected:

	UIElement *m_Parent;

	glm::vec2	m_Position;
	glm::vec2	m_Size;
	float		m_Rotation;

	std::vector<UIElement*> m_Children;
};

