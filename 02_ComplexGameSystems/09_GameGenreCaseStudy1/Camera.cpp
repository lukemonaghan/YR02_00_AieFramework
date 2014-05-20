#include "Camera.h"


Camera::Camera(float a_elasticity){
	m_elasticity = a_elasticity;
}

Camera::~Camera(){

}

void Camera::update(float a_deltatime){
	if (m_target == nullptr){
		return;
	}

	glm::vec3 dispos = ((*m_target) * glm::vec4(m_targetOffset,1)).xyz;

	float step = m_elasticity * a_deltatime;
	if (step >= 1.0f){
		m_transform[3].xyz = dispos;
		m_velocity = glm::vec3(0);
	}else{
		glm::vec3 diff = dispos - m_transform[3].xyz;
		glm::vec3 force = (diff - (m_velocity * 2.0f));

		m_velocity += force * step;
		m_transform[3].xyz += m_velocity * step;
	}

	if (glm::distance2(m_transform[3], (*m_target)[3]) > 0 ){
		m_transform[2].xyz = glm::normalize(m_transform[3].xyz - (*m_target)[3].xyz);
		m_transform[0].xyz = glm::cross(glm::vec3(0,1,0),glm::vec3(m_transform[2].xyz));
		m_transform[1].xyz = glm::cross(glm::vec3(m_transform[2].xyz),glm::vec3(m_transform[0].xyz));
	}

	m_viewTransform = glm::inverse(m_transform);

}

void Camera::setTarget(const glm::mat4* a_target, const glm::vec3& a_offset){
	m_target = a_target;
	m_targetOffset = a_offset;

	m_transform[3] = (*m_target) * glm::vec4(m_targetOffset,1);
}

