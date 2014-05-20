#ifndef _PLANET_H_
#define _PLANET_H_

#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

class Planet{
public:
	Planet(float f_size,float f_speed,Planet* p_Planet = nullptr);
	~Planet();
	void Update(float f_Delta);
	void Draw();
	void SetColour(glm::vec4 v4_colour);
	float size,fSpeed;
	glm::vec4 colour;
	glm::vec3 position,offset;
	Planet* parent;
	float distance;
};

#endif