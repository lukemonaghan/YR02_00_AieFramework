#include "UIElement.h"


UIElement::UIElement(){
	m_Position = glm::vec2(0);
	m_Size = glm::vec2(1);
	m_Rotation = 0;
	m_Parent = nullptr;
}

UIElement::~UIElement(){
	for (auto child : m_Children){
		delete child;
	}
}

void UIElement::addElement(UIElement *a_Element){
	if (a_Element == nullptr){
		return;
	}

	if (a_Element->m_Parent == nullptr){
		a_Element->m_Parent = this;
		m_Children.push_back(a_Element);
	}
}

glm::mat3 UIElement::getTransform() const {
	glm::mat3 trans, rot, scale;
	trans[2] = glm::vec3(m_Position,1);
	scale[0][0] = m_Size.x;
	scale[1][1] = m_Size.y;

	rot[0][0] = cos(m_Rotation);
	rot[0][1] = -sin(m_Rotation);
	rot[1][0] = sin(m_Rotation);
	rot[1][1] = cos(m_Rotation);

	if (m_Parent == nullptr){
		return trans * rot * scale;
	}else{
		return m_Parent->getTransform() * (trans * rot * scale);
	}
}

void UIElement::draw(const glm::mat4& a_Projection){
	for(auto child : m_Children){
		child->draw(a_Projection);
	}
}

void UIElement::update(float a_Deltatime){
		for(auto child : m_Children){
		child->update(a_Deltatime);
	}
}

