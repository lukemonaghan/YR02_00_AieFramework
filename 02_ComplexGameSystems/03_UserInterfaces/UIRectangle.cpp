#include "UIRectangle.h"
#include <GL\glew.h>

UIRectangle::UIRectangle(unsigned int a_Shader,const glm::vec2& a_dimentions,const glm::vec4& a_colour){
	m_shader = a_Shader;
	m_dimentions = a_dimentions;
	m_colour = a_colour;
	m_texture = 0;

	m_textureCoordinates[0] = glm::vec2(0,1);
	m_textureCoordinates[1] = glm::vec2(0,0);
	m_textureCoordinates[2] = glm::vec2(1,1);
	m_textureCoordinates[3] = glm::vec2(1,0);

	glm::vec3 verts[4] = {
		glm::vec3(-0.5f, 0.5f,1.0f),
		glm::vec3(-0.5f,-0.5f,1.0f),
		glm::vec3( 0.5f, 0.5f,1.0f),
		glm::vec3( 0.5f,-0.5f,1.0f)
	};

	glGenVertexArrays(1,&m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1,&m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER,m_vbo);
	glBufferData(GL_ARRAY_BUFFER,4 * sizeof(glm::vec3),verts,GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glBindVertexArray(0);
}

UIRectangle::~UIRectangle(){
	glDeleteVertexArrays(1,&m_vao);
	glDeleteBuffers(1,&m_vbo);
}

void UIRectangle::draw(const glm::mat4& a_Projection){
	int program = -1;
	glGetIntegerv(GL_CURRENT_PROGRAM,&program);

	if (program != m_shader){
		glUseProgram(m_shader);
	}

	int location = glGetUniformLocation(m_shader,"projection");
	glUniformMatrix4fv(location,1,GL_FALSE,glm::value_ptr(a_Projection));

	glm::mat3 transform = getTransform();
	location = glGetUniformLocation(m_shader,"transform");
	glUniformMatrix3fv(location,1,GL_FALSE,glm::value_ptr(transform));

	location = glGetUniformLocation(m_shader,"dimensions");
	glUniform2f(location,m_dimentions.x,m_dimentions.y);

	location = glGetUniformLocation(m_shader,"colour");
	glUniform4fv(location,1,glm::value_ptr(m_colour));

	location = glGetUniformLocation(m_shader,"textureCoordinates");
	glUniform2fv(location,4,(float*)m_textureCoordinates);

	location = glGetUniformLocation(m_shader,"diffuseMap");
	glUniform1i(location,0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,m_texture);

	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLE_STRIP,0,4);

	UIElement::draw(a_Projection);

	if (program != m_shader){
		glUseProgram(program);
	}

}

void UIRectangle::update(float a_Deltatime){
	UIElement::update(a_Deltatime);
}