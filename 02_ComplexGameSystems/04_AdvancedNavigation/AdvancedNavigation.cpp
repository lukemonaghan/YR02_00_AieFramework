#include "AdvancedNavigation.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

AdvancedNavigation::AdvancedNavigation(){}
AdvancedNavigation::~AdvancedNavigation(){}

bool AdvancedNavigation::onCreate(int a_argc, char* a_argv[]) {	
	Gizmos::create();
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//!--TUTORIAL

	m_sponza = new FBXFile();
	m_sponza->load("models/Sponza/SponzaSimple.fbx", FBXFile::UNITS_CENTIMETER);
	createOpenGLBuffers(m_sponza);

	m_navMesh = new FBXFile();
	m_navMesh->load("models/Sponza/SponzaSimpleNavMesh.fbx", FBXFile::UNITS_CENTIMETER);
	createOpenGLBuffers(m_navMesh);

	unsigned int vs = Utility::loadShader("shaders/04_AdvancedNavigation_Sponza.vert", GL_VERTEX_SHADER);
	unsigned int fs = Utility::loadShader("shaders/04_AdvancedNavigation_Sponza.frag", GL_FRAGMENT_SHADER);
	m_shader = Utility::createProgram(vs,0,0,0,fs);
	glDeleteShader(vs);
	glDeleteShader(fs);

	buildNavMesh(m_navMesh->getMeshByIndex(0),m_graph);

	//!--TUTORIAL

	return true;
}

void AdvancedNavigation::onUpdate(float a_deltaTime) {
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );
	Gizmos::clear();
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );
	for ( int i = 0 ; i < 21 ; ++i ){
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}

	//!--TUTORIAL

	for ( auto node : m_graph ){
		Gizmos::addAABBFilled(node->position,glm::vec3(0.1f),glm::vec4(1,0,0,1));

		if (node->edgeTarget[0] != nullptr){
			Gizmos::addLine( node->position + glm::vec3(0,0.1f,0),node->edgeTarget[0]->position + glm::vec3(0,0.1f,0) , glm::vec4(1,1,0,1) );
		}
		if (node->edgeTarget[1] != nullptr){
			Gizmos::addLine( node->position + glm::vec3(0,0.1f,0),node->edgeTarget[1]->position + glm::vec3(0,0.1f,0) , glm::vec4(1,1,0,1) );
		}
		if (node->edgeTarget[2] != nullptr){
			Gizmos::addLine( node->position + glm::vec3(0,0.1f,0),node->edgeTarget[2]->position + glm::vec3(0,0.1f,0) , glm::vec4(1,1,0,1) );
		}
	}

	//!--TUTORIAL

	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void AdvancedNavigation::onDraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	Gizmos::draw(viewMatrix, m_projectionMatrix);
	
	//!--TUTORIAL

	glUseProgram(m_shader);

	int location = glGetUniformLocation(m_shader, "projectionView");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr( m_projectionMatrix * viewMatrix ));

	unsigned int count = m_sponza->getMeshCount();
	for (unsigned int i = 0 ; i < count ; ++i )
	{
		FBXMeshNode* mesh = m_sponza->getMeshByIndex(i);

		GLData* data = (GLData*)mesh->m_userData;

		location = glGetUniformLocation(m_shader, "model");
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr( mesh->m_globalTransform ));

		location = glGetUniformLocation(m_shader, "invTransposeModel");
		glUniformMatrix4fv(location, 1, GL_TRUE, glm::value_ptr( glm::inverse( mesh->m_globalTransform ) ));

		glBindVertexArray(data->vao);
		glDrawElements(GL_TRIANGLES, mesh->m_indices.size(), GL_UNSIGNED_INT, nullptr);
	}

	//!--TUTORIAL
}

void AdvancedNavigation::onDestroy() {
	Gizmos::destroy();
}

int main(int argc, char* argv[]) {
	Application* app = new AdvancedNavigation();
	
	if (app->create("AIE - 04_AdvancedNavigation",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	delete app;
	return 0;
}

void AdvancedNavigation::buildNavMesh(FBXMeshNode* a_mesh,std::vector<NavNodeTri*>& a_graph){
	unsigned int TriCount = a_mesh->m_indices.size() / 3;

	for (unsigned int i = 0; i < TriCount; i++){
		NavNodeTri* node = new NavNodeTri();

		node->edgeTarget[0] = nullptr;
		node->edgeTarget[1] = nullptr;
		node->edgeTarget[2] = nullptr;

		node->vertices[0] = a_mesh->m_vertices[a_mesh->m_indices[i * 3 + 0] ].position.xyz;
		node->vertices[1] = a_mesh->m_vertices[a_mesh->m_indices[i * 3 + 1] ].position.xyz;
		node->vertices[2] = a_mesh->m_vertices[a_mesh->m_indices[i * 3 + 2] ].position.xyz;

		node->position = (node->vertices[0] + node->vertices[1] + node->vertices[2]) / 3.0f;

		a_graph.push_back(node);
	}

	for( auto start : a_graph){
		for( auto end : a_graph){
			if (start != end){
				if ((start->vertices[0] == end->vertices[0] && start->vertices[1] == end->vertices[1]) || 
					(start->vertices[0] == end->vertices[1] && start->vertices[1] == end->vertices[2]) || 
					(start->vertices[0] == end->vertices[2] && start->vertices[1] == end->vertices[0]) || 
					(start->vertices[0] == end->vertices[1] && start->vertices[1] == end->vertices[0]) || 
					(start->vertices[0] == end->vertices[2] && start->vertices[1] == end->vertices[1]) || 
					(start->vertices[0] == end->vertices[0] && start->vertices[1] == end->vertices[2])){
						start->edgeTarget[0] = end;
				}

				if ((start->vertices[1] == end->vertices[0] && start->vertices[2] == end->vertices[1]) || 
					(start->vertices[1] == end->vertices[1] && start->vertices[2] == end->vertices[2]) || 
					(start->vertices[1] == end->vertices[2] && start->vertices[2] == end->vertices[0]) || 
					(start->vertices[1] == end->vertices[1] && start->vertices[2] == end->vertices[0]) || 
					(start->vertices[1] == end->vertices[2] && start->vertices[2] == end->vertices[1]) || 
					(start->vertices[1] == end->vertices[0] && start->vertices[2] == end->vertices[2])){
						start->edgeTarget[1] = end;
				}

				if ((start->vertices[2] == end->vertices[0] && start->vertices[0] == end->vertices[1]) || 
					(start->vertices[2] == end->vertices[1] && start->vertices[0] == end->vertices[2]) || 
					(start->vertices[2] == end->vertices[2] && start->vertices[0] == end->vertices[0]) || 
					(start->vertices[2] == end->vertices[1] && start->vertices[0] == end->vertices[0]) || 
					(start->vertices[2] == end->vertices[2] && start->vertices[0] == end->vertices[1]) || 
					(start->vertices[2] == end->vertices[0] && start->vertices[0] == end->vertices[2])){
						start->edgeTarget[2] = end;
				}
			}
		}
	}

}

void AdvancedNavigation::createOpenGLBuffers(FBXFile* a_fbx)
{
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

void AdvancedNavigation::cleanupOpenGLBuffers(FBXFile* a_fbx)
{
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

