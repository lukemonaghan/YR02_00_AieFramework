#pragma once

#include <glm\ext.hpp>

class Player {
public:
	Player(float a_speed);
	virtual ~Player();

	virtual void update(float a_deltaTime);

	virtual void draw();

	const glm::mat4& getTransform() const {return m_transform; }

protected:
	glm::mat4 m_transform;
	float m_speed;
};

