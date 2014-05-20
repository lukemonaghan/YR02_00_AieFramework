#pragma once

#include <glm/ext.hpp>

class Camera {
public:
	Camera(float a_elasticity);
	virtual ~Camera();

	virtual void update(float a_deltatime);

	void				setTarget(const glm::mat4* a_target, const glm::vec3& a_offset);	

	const glm::mat4&	getTransform() const { return m_transform; }
	const glm::mat4&	getViewTransform() const { return m_viewTransform; }

protected:

	float			m_elasticity;
	glm::vec3		m_velocity;

	glm::vec3		m_targetOffset;
	const glm::mat4	*m_target;

	glm::mat4		m_transform;
	glm::mat4		m_viewTransform;
};

