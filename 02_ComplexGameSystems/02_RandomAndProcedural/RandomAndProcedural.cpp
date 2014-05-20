#include "RandomAndProcedural.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

RandomAndProcedural::RandomAndProcedural(){}
RandomAndProcedural::~RandomAndProcedural(){}

bool RandomAndProcedural::onCreate(int a_argc, char* a_argv[]) {
	Gizmos::create();
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	Utility::build3DPlane(50,m_vao,m_vbo,m_ibo);

	unsigned int vs = Utility::loadShader("shaders/02_RandomProcedural.vert",GL_VERTEX_SHADER);
	unsigned int fs = Utility::loadShader("shaders/02_RandomProcedural.frag",GL_FRAGMENT_SHADER);
	
	unsigned int es = Utility::loadShader("shaders/02_RandomProcedural.eval",GL_TESS_EVALUATION_SHADER);
	unsigned int cs = Utility::loadShader("shaders/02_RandomProcedural.cont",GL_TESS_CONTROL_SHADER);

	m_shader = Utility::createProgram(vs,cs,es,0,fs);

	glDeleteShader(vs);
	glDeleteShader(fs);

	m_dimentions = glm::ivec2(256,256);

	m_height = new float [m_dimentions.x * m_dimentions.y];

	int layers = 6;
	float amplitude = 0.75f;
	glm::vec2 size = 1.0f / glm::vec2(m_dimentions - 1);

	for (int x = 0; x < m_dimentions.x; ++x){
		for (int y = 0; y < m_dimentions.y; ++y){
			float value = 0;
			for(int i = 0; i < layers; ++i){
				float freq = (float) glm::pow(2,i);
				float amp = glm::pow(amplitude,(float)i);
				value += glm::perlin(glm::vec2(x,y) * size * freq) * amp;
			}
			m_height[x * m_dimentions.y + y] = value * 0.5f + 0.5f;
		}
	}

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D,m_texture);
	glTexImage2D(GL_TEXTURE_2D,0, GL_RED,m_dimentions.x,m_dimentions.y,0,GL_RED,GL_FLOAT,m_height);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D,0);

	return true;
}

void RandomAndProcedural::onUpdate(float a_deltaTime) 
{
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );
	Gizmos::clear();
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );

	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void RandomAndProcedural::onDraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );

	glUseProgram(m_shader);

	int location = glGetUniformLocation(m_shader,"projectionViewModel");
	glUniformMatrix4fv(location,1,GL_FALSE,glm::value_ptr(m_projectionMatrix * viewMatrix));

	location = glGetUniformLocation(m_shader,"diffuseMap");
	glUniform1i(location,0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,m_texture);
	
	glBindVertexArray(m_vao);
	glPatchParameteri(GL_PATCH_VERTICES,3);
	glDrawElements(GL_PATCHES,6,GL_UNSIGNED_INT,nullptr);

	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void RandomAndProcedural::onDestroy() {
	glDeleteProgram(m_shader);

	glDeleteVertexArrays(1,&m_vao);
	glDeleteBuffers(1,&m_vbo);
	glDeleteBuffers(1,&m_ibo);

	Gizmos::destroy();
}

int main(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Application* app = new RandomAndProcedural();
	
	if (app->create("AIE - 02_RandomAndProcedural",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	// explicitly control the destruction of our application
	delete app;

	return 0;
}