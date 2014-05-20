#include "Planet.h"

Planet::Planet(float f_size,float f_Speed,Planet* p_Planet){
	size = f_size;
	fSpeed = f_Speed;
	colour = glm::vec4();
	offset = position = glm::vec3();
	parent = p_Planet;
}
Planet::~Planet(){};

void Planet::SetColour(glm::vec4 v4_colour){
	colour = v4_colour;
}

void Planet::Update(float f_Delta){
	if (parent != nullptr){
		offset = glm::rotate(offset,fSpeed * f_Delta,glm::vec3(0.0f,1.0f,0.0f));
		position = offset + parent->position;
	}else{
		offset = glm::rotate(offset,fSpeed * f_Delta,glm::vec3(0.0f,1.0f,0.0f));
		position = offset;
	}
}
void Planet::Draw(){
	Gizmos::addArcRing(glm::vec3(0),0,distance,distance + 1,7,100,colour);
	Gizmos::addSphere(position,24,24,size,colour);
}
