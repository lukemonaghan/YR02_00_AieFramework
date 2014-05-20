#include "Threading.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

//--TUTORIAL

void Threading::createOpenGLBuffers(FBXFile *a_fbx){
	// create the GL VAO/VBO/IBO data for meshes
	for ( unsigned int i = 0 ; i < a_fbx->getMeshCount() ; ++i )
	{
		FBXMeshNode* mesh = a_fbx->getMeshByIndex(i);

		// storage for the opengl data in 3 unsigned int
		GLData* glData = new GLData();

		glGenVertexArrays(1, &glData->vao);
		glBindVertexArray(glData->vao);

		glGenBuffers(1, &glData->vbo);
		glGenBuffers(1, &glData->ibo);

		glBindBuffer(GL_ARRAY_BUFFER, glData->vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData->ibo);

		glBufferData(GL_ARRAY_BUFFER, mesh->m_vertices.size() * sizeof(FBXVertex), mesh->m_vertices.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->m_indices.size() * sizeof(unsigned int), mesh->m_indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); // position
		glEnableVertexAttribArray(1); // normal
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char*)FBXVertex::PositionOffset );
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char*)FBXVertex::NormalOffset );

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		mesh->m_userData = glData;
	}
}

void Threading::cleanupOpenGLBuffers(FBXFile *a_fbx){
	// bind our vertex array object and draw the mesh
	for ( unsigned int i = 0 ; i < a_fbx->getMeshCount() ; ++i )
	{
		FBXMeshNode* mesh = a_fbx->getMeshByIndex(i);

		GLData* glData = (GLData*)mesh->m_userData;

		glDeleteVertexArrays(1, &glData->vao);
		glDeleteBuffers(1, &glData->vbo);
		glDeleteBuffers(1, &glData->ibo);

		delete[] glData;
	}
}

//--TUTORIAL

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

Threading::Threading(){}

Threading::~Threading(){}

bool Threading::onCreate(int a_argc, char* a_argv[]) {
	Gizmos::create();
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//--TUTORIAL

	unsigned int vs = Utility::loadShader("shaders/04_AdvancedNavigation_Sponza.vert", GL_VERTEX_SHADER);
	unsigned int fs = Utility::loadShader("shaders/04_AdvancedNavigation_Sponza.frag", GL_FRAGMENT_SHADER);
	m_shader = Utility::createProgram(vs,0,0,0,fs);
	glDeleteShader(vs);
	glDeleteShader(fs);

	//std::thread MyThread(printHello);

	m_fbx = nullptr;
	m_loadingComplete = false;
	m_ready = false;

	m_loadingThread = new std::thread(
		[this]{
			m_fbx = new FBXFile();
			m_fbx->load("models/Sponza/SponzaSimple.fbx", FBXFile::UNITS_CENTIMETER, false, false);
			m_loadingComplete = true;
		}
	);

	//--TUTORIAL

	return true;
}

void Threading::onUpdate(float a_deltaTime) {
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );
	Gizmos::clear();
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );
	for ( int i = 0 ; i < 21 ; ++i ){
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}

	//--TUTORIAL
	
	if (m_ready == false){
		if (m_loadingComplete == true){
			m_loadingThread->join();
			delete m_loadingThread;
			m_loadingThread = nullptr;

			createOpenGLBuffers(m_fbx);
			m_ready = true;
		}else{
			int tick = fmod(Utility::getTotalTime() * 10 , 4.99f);
			for (int i = 0; i < 5; i++){
				glm::vec4 colour = tick == i ? glm::vec4(1,0,0,1) : glm::vec4(0,0,1,1);
				Gizmos::addAABBFilled(glm::vec3(-2 + i,0,0),glm::vec3(0.5f),colour);
			}
		}
	}

	//--TUTORIAL

	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void Threading::onDraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	//--TUTORIAL

	if (m_ready == true){
		glUseProgram(m_shader);

		int location = glGetUniformLocation(m_shader, "projectionView");
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr( m_projectionMatrix * viewMatrix ));

		unsigned int count = m_fbx->getMeshCount();
		for (unsigned int i = 0 ; i < count ; ++i )
		{
			FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);

			GLData* data = (GLData*)mesh->m_userData;

			location = glGetUniformLocation(m_shader, "model");
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr( mesh->m_globalTransform ));

			location = glGetUniformLocation(m_shader, "invTransposeModel");
			glUniformMatrix4fv(location, 1, GL_TRUE, glm::value_ptr( glm::inverse( mesh->m_globalTransform ) ));

			glBindVertexArray(data->vao);
			glDrawElements(GL_TRIANGLES, mesh->m_indices.size(), GL_UNSIGNED_INT, nullptr);
		}
	}

	//--TUTORIAL

	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void Threading::onDestroy() {
	cleanupOpenGLBuffers(m_fbx);
	delete m_fbx;
	Gizmos::destroy();
}

int main(int argc, char* argv[]) {
	Application* app = new Threading();
	if (app->create("AIE - 16_Threading",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();

	delete app;
	return 0;
}