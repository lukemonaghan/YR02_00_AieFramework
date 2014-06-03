#include "_16_SoftBodies.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <SOIL.h>

#include <vector>
#include <PxTkStream.h>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

physx::PxFoundation* g_PhysicsFoundation =nullptr;
physx::PxPhysics* g_Physics = nullptr;
physx::PxScene* g_PhysicsScene = nullptr;
physx::PxDefaultErrorCallback gDefaultErrorCallback;
physx::PxDefaultAllocator gDefaultAllocatorCallback;
physx::PxSimulationFilterShader gDefaultFilterShader = physx::PxDefaultSimulationFilterShader;
physx::PxMaterial* g_PhysicsMaterial = nullptr;
physx::PxCooking* g_PhysicsCooker = nullptr;
std::vector<physx::PxRigidDynamic*> g_PhysXActors;

_16_SoftBodies::_16_SoftBodies(){}
_16_SoftBodies::~_16_SoftBodies(){}

unsigned int m_shader;
unsigned int m_texture;
unsigned int m_clothIndexCount;
unsigned int m_clothVertexCount;
glm::vec3*	m_clothPositions;
unsigned int m_clothVAO, m_clothVBO, m_clothTextureVBO, m_clothIBO;
physx::PxClothCollisionSphere *Spheres;

void addSphere(physx::PxShape* pShape,physx::PxRigidDynamic* actor) {
	//get the geometry for this PhysX collision volume
	physx::PxSphereGeometry geometry;
	float radius;
	bool status = pShape->getSphereGeometry(geometry);
	if(status){
		radius = geometry.radius;
	}
	//get the transform for this PhysX collision volume
	physx::PxMat44 m(physx::PxShapeExt::getGlobalPose(*pShape));
	glm::mat4 M(m.column0.x, m.column0.y, m.column0.z, m.column0.w,
				m.column1.x, m.column1.y, m.column1.z, m.column1.w,
				m.column2.x, m.column2.y, m.column2.z, m.column2.w,
				m.column3.x, m.column3.y, m.column3.z, m.column3.w);
	glm::vec3 position;
	//get the position out of the transform
	position.x = m.getPosition().x;
	position.y = m.getPosition().y;
	position.z = m.getPosition().z;
	glm::vec4 colour = glm::vec4(rand()%1000 / 1000.0f,rand()%1000 / 1000.0f,rand()%1000 / 1000.0f,1);
	//create our box gizmo
	Gizmos::addSphere(position,5,5,radius,colour,&M);
}

void addBox(physx::PxShape* pShape,physx::PxRigidDynamic* actor) {
	//get the geometry for this PhysX collision volume
	physx::PxBoxGeometry geometry;
	float width = 1, height = 1, length = 1;
	bool status = pShape->getBoxGeometry(geometry);
	if(status){
		width = geometry.halfExtents.x;
		height = geometry.halfExtents.y;
		length = geometry.halfExtents.z;
	}
	//get the transform for this PhysX collision volume
	physx::PxMat44 m(physx::PxShapeExt::getGlobalPose(*pShape));
	glm::mat4 M(m.column0.x, m.column0.y, m.column0.z, m.column0.w,
				m.column1.x, m.column1.y, m.column1.z, m.column1.w,
				m.column2.x, m.column2.y, m.column2.z, m.column2.w,
				m.column3.x, m.column3.y, m.column3.z, m.column3.w);
	glm::vec3 position;
	//get the position out of the transform
	position.x = m.getPosition().x;
	position.y = m.getPosition().y;
	position.z = m.getPosition().z;
	glm::vec3 extents = glm::vec3(width,height,length);
	glm::vec4 colour = glm::vec4(rand()%1000 / 1000.0f,rand()%1000 / 1000.0f,rand()%1000 / 1000.0f,1);
	//create our box gizmo
	Gizmos::addAABBFilled(position,extents,colour,&M);
}

void addWidget(physx::PxShape* shape,physx::PxRigidDynamic* actor) {
	physx::PxGeometryType::Enum type = shape->getGeometryType();
	switch(type){
		case physx::PxGeometryType::eBOX:
			addBox(shape,actor);
			break;
		case physx::PxGeometryType::eSPHERE:
			addSphere(shape,actor);
			break;
	}
}

class myAllocator: public physx::PxAllocatorCallback{
public:
	virtual ~myAllocator() {}
	virtual void* allocate(size_t size, const char* typeName, const char* filename, int line){
		void* pointer = _aligned_malloc(size,16);
		return pointer;
	}
	virtual void deallocate(void* ptr){
		_aligned_free(ptr);
	}
};

void setUpPhysXTutorial(){
	physx::PxAllocatorCallback *myCallback = new myAllocator();
	g_PhysicsFoundation = PxCreateFoundation(PX_PHYSICS_VERSION,*myCallback, gDefaultErrorCallback);
	g_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *g_PhysicsFoundation, physx::PxTolerancesScale());
	g_PhysicsCooker = PxCreateCooking(PX_PHYSICS_VERSION, *g_PhysicsFoundation, physx::PxCookingParams(physx::PxTolerancesScale()));
	PxInitExtensions(*g_Physics);
	//create physics material
	g_PhysicsMaterial = g_Physics->createMaterial(0.5f,0.5f,0.6f);
	physx::PxSceneDesc sceneDesc(g_Physics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0,-30.0f,0);
	sceneDesc.filterShader = gDefaultFilterShader;
	sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
	g_PhysicsScene = g_Physics->createScene(sceneDesc);

	if(g_PhysicsScene) {
		printf("start physx scene2\n");
	}
}

void cleanUpPhsyx() {
	g_PhysicsCooker->release();
	g_PhysicsScene->release();
	g_Physics->release();
	g_PhysicsFoundation->release();
}

void upDatePhysx(){
	g_PhysicsScene->simulate( 1/120.f );
	while (g_PhysicsScene->fetchResults() == false) {
	// don’t need to do anything here yet but we still need to do the fetch
	}
	for (auto actor:g_PhysXActors) {
		physx::PxU32 nShapes = actor->getNbShapes();
		physx::PxShape** shapes = new physx::PxShape*[nShapes];
		actor->getShapes(shapes, nShapes);
		// Render all the shapes in the physx actor (for early tutorials there is just one)
		while (nShapes--) {
			addWidget(shapes[nShapes],actor);
		}
		delete [] shapes;
	}
}

void AddPlane(glm::vec3 v3_Facing){
	//add a plane
	physx::PxTransform pose = physx::PxTransform( physx::PxVec3(0,0,0),physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0.0f, 0.0f, 1.0f)));
	physx::PxRigidStatic* plane = PxCreateStatic(*g_Physics, pose, physx::PxPlaneGeometry(), *g_PhysicsMaterial);
	//add it to the physX scene
	g_PhysicsScene->addActor(*plane);
}

void AddBox(const glm::vec3& v3_Transform, const glm::vec3& v3_Dimentions,float f_density) {
	//add a box
	physx::PxBoxGeometry box(v3_Dimentions.x,v3_Dimentions.y,v3_Dimentions.z);
	physx::PxTransform transform(physx::PxVec3(v3_Transform.x,v3_Transform.y,v3_Transform.z));
	physx::PxRigidDynamic* dynamicActor = PxCreateDynamic(*g_Physics, transform, box,*g_PhysicsMaterial, f_density); 
	//add it to the physX scene
	g_PhysicsScene->addActor(*dynamicActor);
	//add it to our copy of the scene
	g_PhysXActors.push_back(dynamicActor);
}

void AddSphere(const glm::vec3& v3_Transform,float f_radius,float f_density) {
	//add a box
	physx::PxSphereGeometry sphere(f_radius);
	physx::PxTransform transform(physx::PxVec3(v3_Transform.x,v3_Transform.y,v3_Transform.z));
	physx::PxRigidDynamic* dynamicActor = PxCreateDynamic(*g_Physics, transform, sphere,*g_PhysicsMaterial, f_density); 
	//add it to the physX scene
	g_PhysicsScene->addActor(*dynamicActor);
	//add it to our copy of the scene
	g_PhysXActors.push_back(dynamicActor);
}

void AddBullet(const glm::mat4& m4_Camera,float f_Power,float f_radius,float f_density) {
	//add a box
	physx::PxSphereGeometry sphere(f_radius);
	physx::PxTransform transform(physx::PxVec3(m4_Camera[3].x,m4_Camera[3].y,m4_Camera[3].z));
	physx::PxRigidDynamic* dynamicActor = PxCreateDynamic(*g_Physics, transform, sphere,*g_PhysicsMaterial, f_density); 
	physx::PxVec3 velocity(m4_Camera[2].x * -f_Power,m4_Camera[2].y * -f_Power,m4_Camera[2].z * -f_Power);
	dynamicActor->setLinearVelocity(velocity,true);
	//add it to the physX scene
	g_PhysicsScene->addActor(*dynamicActor);
	//add it to our copy of the scene
	g_PhysXActors.push_back(dynamicActor);
}

void setUpVisualDebugger() {
	// check if PvdConnection manager is available on this platform
	if (NULL == g_Physics->getPvdConnectionManager())
		return;
	// setup connection parameters
	const char* pvd_host_ip = "127.0.0.1"; // IP of the PC which is running PVD
	int port = 5425; // TCP port to connect to, where PVD is listening
	unsigned int timeout = 100; // timeout in milliseconds to wait for PVD to respond,
	// consoles and remote PCs need a higher timeout.
	physx::PxVisualDebuggerConnectionFlags connectionFlags = physx::PxVisualDebuggerConnectionFlag::Debug | physx::PxVisualDebuggerConnectionFlag::Profile | physx::PxVisualDebuggerConnectionFlag::Memory;
	// and now try to connect
	physx::PxVisualDebuggerExt::createConnection(g_Physics->getPvdConnectionManager(),pvd_host_ip, port, timeout, connectionFlags);
	// pvd_host_ip, port, timeout, connectionFlags));
}

bool _16_SoftBodies::onCreate(int a_argc, char* a_argv[]) {
	Gizmos::create();
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	//!- TUTORIAL
	setUpPhysXTutorial();
	setUpVisualDebugger();

	// set cloth properties
	float springSize = 0.2f;
	unsigned int springRows = 40;
	unsigned int springCols = 40;
	// this position will represent the top middle vertex
	glm::vec3 clothPosition = glm::vec3(0,12,0);
	// shifting grid position for looks
	float halfWidth = springRows * springSize * 0.5f;
	// generate vertices for a grid with texture coordinates
	m_clothVertexCount = springRows * springCols;
	m_clothPositions = new glm::vec3[m_clothVertexCount];
	glm::vec2* clothTextureCoords = new glm::vec2[m_clothVertexCount];
	for (unsigned int r = 0; r < springRows; ++r)
	{
	for (unsigned int c = 0; c < springCols; ++c)
	{
	m_clothPositions[r * springCols + c].x = clothPosition.x + springSize * c;
	m_clothPositions[r * springCols + c].y = clothPosition.y;
	m_clothPositions[r * springCols + c].z = clothPosition.z + springSize * r - halfWidth;
	clothTextureCoords[r * springCols + c].x = 1.0f - r / (springRows - 1.0f);
	clothTextureCoords[r * springCols + c].y = 1.0f - c / (springCols - 1.0f);
	}
	}
	// set up indices for a grid
	m_clothIndexCount = (springRows-1) * (springCols-1) * 2 * 3;
	unsigned int* indices = new unsigned int[ m_clothIndexCount ];
	unsigned int* index = indices;
	for (unsigned int r = 0; r < (springRows-1); ++r)
	{
	for (unsigned int c = 0; c < (springCols-1); ++c)
	{
	// indices for the 4 quad corner vertices
	unsigned int i0 = r * springCols + c;
	unsigned int i1 = i0 + 1;
	unsigned int i2 = i0 + springCols;
	unsigned int i3 = i2 + 1;
	// every second quad changes the triangle order
	if ((c + r) % 2)
	{
	*index++ = i0; *index++ = i2; *index++ = i1;
	*index++ = i1; *index++ = i2; *index++ = i3;
	}
	else
	{
	*index++ = i0; *index++ = i2; *index++ = i3;
	*index++ = i0; *index++ = i3; *index++ = i1;
	}
	}
	}

	// set up opengl data for the grid, with the positions as dynamic
	glGenVertexArrays(1, &m_clothVAO);
	glBindVertexArray(m_clothVAO);
	glGenBuffers(1, &m_clothIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_clothIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_clothIndexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);
	glGenBuffers(1, &m_clothVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_clothVBO);
	glBufferData(GL_ARRAY_BUFFER, m_clothVertexCount * (sizeof(glm::vec3)), 0, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0); // position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (char*)0);
	glGenBuffers(1, &m_clothTextureVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_clothTextureVBO);
	glBufferData(GL_ARRAY_BUFFER, m_clothVertexCount * (sizeof(glm::vec2)), clothTextureCoords, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1); // texture
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (char*)0);
	glBindVertexArray(0);

	unsigned int vs = Utility::loadShader("shaders/03_Physics/16_SoftBodies.vert", GL_VERTEX_SHADER);
	unsigned int fs = Utility::loadShader("shaders/03_Physics/16_SoftBodies.frag", GL_FRAGMENT_SHADER);
	m_shader = Utility::createProgram(vs,0,0,0,fs);
	glDeleteShader(vs);
	glDeleteShader(fs);
	m_texture = SOIL_load_OGL_texture("textures/random.png",SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);

	m_cloth = createCloth(clothPosition, m_clothVertexCount, m_clothIndexCount, m_clothPositions, indices);
	if (m_cloth != nullptr){
		g_PhysicsScene->addActor(*m_cloth);
	}
	// texture coords and indices no longer needed
	delete[] clothTextureCoords;
	delete[] indices;
	//!- TUTORIAL

	return true;
}

void _16_SoftBodies::onUpdate(float a_deltaTime) {
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );
	Gizmos::clear();
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );
	for ( int i = 0 ; i < 21 ; ++i ){
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}
	
	//!- TUTORIAL
	static bool bLeftPressed = false;
	static float counter = 0;
	if (glfwGetMouseButton(m_window,GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
		if (!bLeftPressed){
			bLeftPressed = true;
			AddBullet(m_cameraMatrix,100,1.0f,100000);
		}
	}else{
		bLeftPressed = false;
	}

	upDatePhysx();

	if (m_cloth != nullptr){
		physx::PxClothReadData* data = m_cloth->lockClothReadData();

		glm::vec3 min;
		glm::vec3 max;

		for ( unsigned int i = 0 ; i < m_clothVertexCount ; ++i){
			m_clothPositions[i] = glm::vec3(data->particles[i].pos.x,data->particles[i].pos.y,data->particles[i].pos.z);
			if (i == 0){min = m_clothPositions[i];}
			if (i == m_clothVertexCount){max = m_clothPositions[i];}
		}
		data->unlock();
	}

	//!- TUTORIAL
	
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		quit();
	}
}

void _16_SoftBodies::onDraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	//!- TUTORIAL
	// bind shader and transforms, along with texture
	glUseProgram(m_shader);
	int location = glGetUniformLocation(m_shader, "projectionView");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m_projectionMatrix * viewMatrix));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	// update vertex positions from the cloth
	glBindBuffer(GL_ARRAY_BUFFER, m_clothVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_clothVertexCount * sizeof(glm::vec3), m_clothPositions);
	// disable face culling so that we can draw it double-sided
	glDisable(GL_CULL_FACE);
	// bind and draw the cloth
	glBindVertexArray(m_clothVAO);
	glDrawElements(GL_TRIANGLES, m_clothIndexCount, GL_UNSIGNED_INT, 0);
	glEnable(GL_CULL_FACE);
	//!- TUTORIAL
	
	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void _16_SoftBodies::onDestroy(){
	//!- TUTORIAL
	
	//!- TUTORIAL
	Gizmos::destroy();
}

int main(int argc, char* argv[]) {
	Application* app = new _16_SoftBodies();
	if (app->create("AIE - 16_SoftBodies",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true){
		app->run();
	}
	delete app;
	return 0;
}

physx::PxCloth* _16_SoftBodies::createCloth(const glm::vec3& a_position,unsigned int& a_vertexCount, unsigned int& a_indexCount,const glm::vec3* a_vertices,unsigned int* a_indices){
	// set up the cloth description
	physx::PxClothMeshDesc clothDesc;
	clothDesc.setToDefault();
	clothDesc.points.count = a_vertexCount;
	clothDesc.points.stride = sizeof(glm::vec3);
	clothDesc.points.data = a_vertices;
	clothDesc.triangles.count = a_indexCount / 3;
	clothDesc.triangles.stride = sizeof(unsigned int) * 3;
	clothDesc.triangles.data = a_indices;
	// cook the geometry into fabric
	PxToolkit::MemoryOutputStream buf;
	if (g_PhysicsCooker->cookClothFabric(clothDesc, physx::PxVec3(0,-9.8f,0), buf) == false) {
		return nullptr;
	}
	//$(SolutionDir)../3rdParty/PhysX/Samples/PxToolkit/lib/win32
	PxToolkit::MemoryInputData data(buf.getData(), buf.getSize());
	physx::PxClothFabric* fabric = g_Physics->createClothFabric(data);

	//Coll data
	physx::PxClothCollisionData cd;
	cd.setToDefault();
	cd.numSpheres = 32;
	cd.pairIndexBuffer = 0;
	Spheres = new physx::PxClothCollisionSphere[32];
	for (int x = 0; x < 32; x++){
		Spheres[x].pos = physx::PxVec3(rand()%10,5 + rand()%5,-5 + rand()%10);
		Spheres[x].radius = 0.5f + rand()%100 / 100.0f;
	}
	cd.spheres = Spheres;

	// set up the particles for each vertex
	physx::PxClothParticle* particles = new physx::PxClothParticle[ a_vertexCount ];
	for ( unsigned int i = 0 ; i < a_vertexCount ; ++i ){
	particles[i].pos = physx::PxVec3( a_vertices[i].x, a_vertices[i].y, a_vertices[i].z );

	// set weights (0 means static)
	//if ( a_vertices[i].x == a_position.x )
	//	particles[i].invWeight = 0;
	//else
		particles[i].invWeight = 2.f;
	}

	// create the cloth then setup the spring properties
	physx::PxCloth* cloth = g_Physics->createCloth(physx::PxTransform( physx::PxVec3(a_position.x,a_position.y,a_position.z) ),*fabric, particles, cd, physx::PxClothFlag::eSWEPT_CONTACT);
	// we need to set some solver configurations
	if (cloth != nullptr){
		physx::PxClothPhaseSolverConfig bendCfg;
		bendCfg.solverType = physx::PxClothPhaseSolverConfig::eFAST;
		bendCfg.stiffness = 1.0f;
		bendCfg.stretchStiffness = 1.0f;
		cloth->setPhaseSolverConfig(physx::PxClothFabricPhaseType::eBENDING, bendCfg);
		cloth->setPhaseSolverConfig(physx::PxClothFabricPhaseType::eSTRETCHING, bendCfg);
		cloth->setPhaseSolverConfig(physx::PxClothFabricPhaseType::eSHEARING, bendCfg);
		cloth->setPhaseSolverConfig(physx::PxClothFabricPhaseType::eSTRETCHING_HORIZONTAL, bendCfg);
		cloth->setDampingCoefficient(1.0f);
	}


	delete[] particles;
	return cloth;
}

