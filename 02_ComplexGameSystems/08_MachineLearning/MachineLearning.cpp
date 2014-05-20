#include "MachineLearning.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

//!--TUTORIAL

#include <algorithm>    // std::sort

void MachineLearning::evolve(){
	int pop = m_population.size();
	int target = m_width * m_height;

	int totalFitness = 0;
	for (int i = 0; i < pop; i++){
		totalFitness += target - m_population[i].score;
	}

	std::vector<Agent> newPopulation;

	while(newPopulation.size() < pop){
		int roulette = rand()%totalFitness;

		Agent agent1;
		int accscore = 0;
		for (int i = 0; i < pop; i++){
			accscore += target - m_population[i].score;
			if (accscore > roulette){
				agent1 = m_population[i];
				break;
			}
		}

		Agent agent2;
		accscore = 0;
		for (int i = 0; i < pop; i++){
			accscore += target - m_population[i].score;
			if (accscore > roulette){
				agent2 = m_population[i];
				break;
			}
		}

		crossover(agent1,agent2);

		mutate(agent1);
		mutate(agent2);

		newPopulation.push_back(agent1);
		newPopulation.push_back(agent2);
	}

	m_population = newPopulation;
	m_generation++;
}

void MachineLearning::crossover(Agent& a_agent1, Agent& a_agent2){
	if (unitRand() < m_crossoverRate){
		int startbit = (rand() % (BIT_COUNT / 2)) * 2;

		for( ; startbit < BIT_COUNT; startbit++){
			int chromo = startbit / 32;
			int bit = startbit % 32;
			int temp1 = a_agent1.genome[chromo] & (1 << bit);
			int temp2 = a_agent2.genome[chromo] & (1 << bit);

			if (temp1 == 0){
				a_agent2.genome[chromo] &= ~(1<<bit);
			}else{
				a_agent2.genome[chromo] |=  (1<<bit);
			}

			if (temp2 == 0){
				a_agent1.genome[chromo] &= ~(1<<bit);
			}else{
				a_agent1.genome[chromo] |=  (1<<bit);
			}
		}
	}
}

void MachineLearning::mutate(Agent& a_agent){
	for(int startbit = 0; startbit < BIT_COUNT; startbit++){
		
		int chromo = startbit / 32;
		int bit = startbit % 32;

		if (unitRand() < m_mutationRate){
			a_agent.genome[chromo] ^= (1 << bit);
		}

	}
}

void MachineLearning::evaluateFitness() {
	int size = m_population.size();

	// loop population
	for (int i = 0 ; i < size ; ++i) {
		int currX = 0, currY = 0;

		// loop agents bits
		for (int bits = 0 ; bits < BIT_COUNT ; bits += 2 ) {
			// get exact bit
			int chromosome = bits / 32;
			int bit = bits % 32;

			if ((m_population[i].genome[chromosome] & (1<<bit)) == 0) {
				if ((m_population[i].genome[chromosome] & (1<<(bit+1))) == 0) {
					// [0,0] = up
					if (currY > 0 && m_maze[currX * m_height + currY - 1] == false) {
						--currY;
					}
				} else {
					// [0,1] = down
					if (currY < (m_height-1) && m_maze[currX * m_height + currY + 1] == false) {
						++currY;
					}
				}
			} else {
				if ((m_population[i].genome[chromosome] & (1<<(bit+1))) == 0) {
					// [1,0] = left
					if (currX > 0 && m_maze[(currX-1) * m_height + currY] == false) {
						--currX;
					}

				} else {
					// [1,1] = right
					if (currX < (m_width-1) && m_maze[(currX+1) * m_height + currY] == false) {
						++currX;
					}
				}
			}

			// exit if we reached the goal
			if (currX == (m_width-1) && currY == (m_height-1)) {
				break;
			}
		}
		// score
		m_population[i].score = abs((m_width-1) - currX) + abs((m_height-1) - currY);
	}
}

void MachineLearning::drawPath(const Agent& a_agent) {
	float startX = -(m_width * 0.5f);
	float startY = -(m_height * 0.5f);
	glm::vec4 yellow(1,1,0,1);

	glm::vec3 v0(startX, 0.1f, startY);
	glm::vec3 v1(startX + 1, 0.1f, startY + 1);
	glm::vec3 v2(startX + 1, 0.1f, startY);
	glm::vec3 v3(startX, 0.1f, startY + 1);

	// draw starting square
	Gizmos::addTri(v0,v1,v2, yellow);
	Gizmos::addTri(v0,v3,v1, yellow);

	int currX = 0, currY = 0;

	// loop agent's bits
	for (int bits = 0 ; bits < BIT_COUNT ; bits += 2 ) {
		// get exact bit
		int chromosome = bits / 32;
		int bit = bits % 32;

		if ((a_agent.genome[chromosome] & (1<<bit)) == 0) {
			if ((a_agent.genome[chromosome] & (1<<(bit+1))) == 0) {
				// [0,0] = up
				if (currY > 0 && m_maze[currX * m_height + currY - 1] == false) {
					--currY;
				}
			} else {
				// [0,1] = down
				if (currY < (m_height-1) && m_maze[currX * m_height + currY + 1] == false) {
					++currY;
				}
			}
		} else {
			if ((a_agent.genome[chromosome] & (1<<(bit+1))) == 0) {
				// [1,0] = left
				if (currX > 0 && m_maze[(currX-1) * m_height + currY] == false) {
					--currX;
				}
			} else {
				// [1,1] = right
				if (currX < (m_width-1) && m_maze[(currX+1) * m_height + currY] == false){
					++currX;
				}
			}
		}

		v0 = glm::vec3(startX + currX, 0.1f, startY + currY) ;
		v1 = glm::vec3(startX + currX + 1, 0.1f, startY + currY + 1);
		v2 = glm::vec3(startX + currX + 1, 0.1f, startY + currY);
		v3 = glm::vec3(startX + currX, 0.1f, startY + currY + 1);

		Gizmos::addTri(v0,v1,v2, yellow);
		Gizmos::addTri(v0,v3,v1, yellow);

		// exit if we reached the goal
		if (currX == (m_width-1) && currY == (m_height-1)){
			return;
		}
	}
}

void MachineLearning::setUpMaze(int a_width, int a_height, float a_percent) {
	m_width = a_width;
	m_height = a_height;
	m_wallRate = a_percent;
	m_population.clear();

	if (m_maze != nullptr){
		delete[] m_maze;
	}
	m_maze = new bool[ m_width * m_height ];
	memset(m_maze, 0, sizeof(bool) * m_width * m_height);

	for ( int x = 0 ; x < m_width ; ++x ) {
		for ( int y = 0 ; y < m_height ; ++y ) {
			if (unitRand() < m_wallRate){
				m_maze[ x * m_height + y ] = true;
			}
		}
	}
}

void MachineLearning::startGeneration(){
	m_generation = 0;
	m_pathfound = false;
	m_crossoverRate = 0.7f;
	m_mutationRate = 0.1f;

	for (int i = 0; i < 1000; i++){
		Agent agent;

		agent.score = m_width + m_height;
		
		for (int j = 0; j < CHROMOSOME_COUNT; j++){
			int UpperBits = rand();
			agent.genome[j] = (UpperBits << 16) + rand();
		}

		m_population.push_back(agent);
	}
}

//!--TUTORIAL

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

MachineLearning::MachineLearning(){}
MachineLearning::~MachineLearning(){}

bool MachineLearning::onCreate(int a_argc, char* a_argv[]) {
	Gizmos::create();
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(0,25,25),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//!--TUTORIAL
	m_maze = nullptr;
	srand(time(NULL));
	setUpMaze(25,25,0.25f);
	startGeneration();

	//!--TUTORIAL

	return true;
}

void MachineLearning::onUpdate(float a_deltaTime) {
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );
	Gizmos::clear();

	//!--TUTORIAL

	static bool Pressed = false;
	if (glfwGetKey(m_window,GLFW_KEY_F1) == GLFW_PRESS) {
		if (Pressed == false){
			Pressed = true;
			setUpMaze(25,25,0.25f);
			startGeneration();
		}
	} else {
		if (Pressed == true){
			Pressed = false;
		}
	}

	// draw maze
	float startX = -(m_width * 0.5f);
	float startY = -(m_height * 0.5f);

	glm::vec4 black(0,0,0,1);
	glm::vec4 white(1);

	for ( int x = 0 ; x < m_width ; ++x )
	{
		for ( int y = 0 ; y < m_height ; ++y )
		{
			glm::vec3 v0(startX + x, 0, startY + y);
			glm::vec3 v1(startX + x + 1, 0, startY + y + 1);
			glm::vec3 v2(startX + x + 1, 0, startY + y);
			glm::vec3 v3(startX + x, 0, startY + y + 1);

			Gizmos::addTri(v0,v1,v2, m_maze[x * m_height + y] == true ? black : white);
			Gizmos::addTri(v0,v3,v1, m_maze[x * m_height + y] == true ? black : white);
		}
	}

	if (m_pathfound == false){
		evaluateFitness();
	}

	std::sort(m_population.begin(),m_population.end());

	m_pathfound = m_pathfound || m_population.front().score == 0;

	drawPath(m_population.front());

	if (m_pathfound == false){
		evolve();
		printf("Generation %i \n",m_generation);
	}

	//!--TUTORIAL

	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		quit();
	}
}

void MachineLearning::onDraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	//!--TUTORIAL

	//!--TUTORIAL

	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void MachineLearning::onDestroy() {
	Gizmos::destroy();

	//!--TUTORIAL

	delete m_maze;

	//!--TUTORIAL
}

int main(int argc, char* argv[]) {
	Application* app = new MachineLearning();
	if (app->create("AIE - 08_MachineLearning",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	delete app;
	return 0;
}

