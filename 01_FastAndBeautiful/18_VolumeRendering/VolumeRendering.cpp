#include "VolumeRendering.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

float fTimer = 0.0f;

bool VolumeRendering::onCreate(int a_argc, char* a_argv[]) {
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	Gizmos::create();
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );
	for ( int i = 0 ; i < 21 ; ++i ){
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );	
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}

	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, Utility::Width/(float)Utility::Height, 0.1f, 1000.0f);

	// create our grid volume
	m_gridSize = glm::ivec3( 20, 20, 20 );
	m_cubeSize = glm::vec3(0.5f,0.5f,0.5f);
	
	// fill the 3-D array of floats
	int rows = m_gridSize.x + 1;
	int cols = m_gridSize.y + 1;
	int depth = m_gridSize.z + 1;

	m_volume = new float * *[ rows ];
	for (int i = 0 ; i < rows ; ++i ){
		m_volume[i] = new float *[ cols ];
		for (int j = 0 ; j < cols ; ++j ){
			m_volume[i][j] = new float [ depth ];
		}
	}

	// create the vertices for the volume
	m_maxVertices = m_gridSize.x * m_gridSize.y * m_gridSize.z * 5 * 3;
	m_vertices = new glm::vec3[ m_maxVertices ];
	
	// create geometry for volume
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_maxVertices, nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (char*)0);

	glBindVertexArray(0);

	// setup the blobs for the volume
	m_blobCount = 15;
	m_blobPoints = new glm::vec3[m_blobCount];
	m_blobVelocities = new glm::vec3[m_blobCount];

	for ( unsigned int i = 0; i < m_blobCount; ++i){
		m_blobVelocities[i].x = (rand()%10 - 5) * 0.5f;
		m_blobVelocities[i].y = (rand()%10 - 5) * 0.5f;
		m_blobVelocities[i].z = (rand()%10 - 5) * 0.5f;
		
		glm::vec3 radius = glm::ballRand(1.0f);
		m_blobPoints[i].x = radius.x;
		m_blobPoints[i].y = radius.y;
		m_blobPoints[i].z = radius.z;
		
	}

	// set some fluid properties
	m_fluidColour = glm::vec4(0,1,0,1);
	m_fluidTransform = glm::translate(-2.75f,0.0f,-2.75f);

	// threshold for defining solid / not-solid
	m_volumeThreshold = 5.0f;

	sWorld = new Shader("shaders/18_marchingCubes.vert","shaders/18_marchingCubes.frag",nullptr,nullptr,"shaders/18_marchingCubes.geom");
	sWorld->SetAttribs(1,0,"position");
	sWorld->SetUniform("Projection","m4fv",1,false,glm::value_ptr(m_projectionMatrix));
	return true;
}

void VolumeRendering::onUpdate(float a_deltaTime) {
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );

	float Spread = 0.25f;

	// update the blobs
	for ( unsigned int i = 0; i < m_blobCount; ++i){
		m_blobPoints[i] += m_blobVelocities[i] * a_deltaTime;


		if (m_blobPoints[i].x < -(m_gridSize.x * m_cubeSize.x * Spread) ||
			m_blobPoints[i].x >  (m_gridSize.x * m_cubeSize.x * Spread)){

			m_blobVelocities[i].x *= -1;
		}
		if (m_blobPoints[i].y < -(m_gridSize.y * m_cubeSize.y * Spread) ||
			m_blobPoints[i].y >  (m_gridSize.y * m_cubeSize.y * Spread) ){

			m_blobVelocities[i].y *= -1;
		}
		if (m_blobPoints[i].z < -(m_gridSize.z * m_cubeSize.z * Spread) ||
			m_blobPoints[i].z >  (m_gridSize.z * m_cubeSize.z * Spread)){

			m_blobVelocities[i].z *= -1;
		}
	}

	// populate a volume
	glm::vec3 dist;
	glm::vec3 position;
	float result = 0;
	glm::vec3 start = glm::vec3(m_gridSize) * m_cubeSize * -0.5f;

	int rows = m_gridSize.x + 1;
	int cols = m_gridSize.y + 1;
	int depth = m_gridSize.z + 1;
	int x, y, z;

	for ( x = 0 ; x < rows ; ++x){
		for ( y = 0 ; y < cols ; ++y ){
			for ( z = 0 ; z < depth ; ++z ){
				result = 0.0f;
				for ( unsigned int i = 0 ; i < m_blobCount ; ++i ){
					position = start + glm::vec3(x,y,z) * m_cubeSize;

					dist = position - m_blobPoints[i];
					result += 1.0f / glm::length2( dist );
				}
				m_volume[ x ][ y ][ z ] = result;
			}
		}
	}

	fTimer += a_deltaTime;
	if (fTimer > 1.0f){
		fTimer = 0.0f;
		printf("Delta %f\n",1/a_deltaTime);
	}

	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS){quit();}
}

void VolumeRendering::onDraw() {
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	sWorld->SetUniform("View","m4fv",1,false,glm::value_ptr(viewMatrix));
	sWorld->SetUniform("Model","m4fv",1,false,glm::value_ptr(m_fluidTransform));
	sWorld->SetUniform("cameraPosition","3fv",1,glm::value_ptr((glm::vec3)m_cameraMatrix[3].xyz));

	Clear();

	// march through our volume and create triangles

	unsigned int vertexCount = m_marcher.march(m_gridSize, m_cubeSize, m_volume, m_volumeThreshold, m_vertices, m_maxVertices);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * vertexCount, m_vertices);

	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);

	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void VolumeRendering::onDestroy() {
	Gizmos::destroy();
	delete[] m_blobPoints;
	delete[] m_blobVelocities;
	delete[] m_vertices;

	int rows = m_gridSize.x + 1;
	int cols = m_gridSize.y + 1;
	int depth = m_gridSize.z + 1;

	if (m_volume != nullptr){
		for (int i = 0 ; i < rows ; ++i ) {
			for (int j = 0 ; j < cols ; ++j ){
				delete[] m_volume[i][j];
			}
			delete[] m_volume[i];
		}
		delete[] m_volume;
	}
}

int main(int argc, char* argv[]) {
	Application* app = new VolumeRendering();
	if (app->create("AIE - 18_VolumeRendering",Utility::Width,Utility::Height,argc,argv) == true){
		app->run();
	}
	delete app;
	return 0;
}