#include "_02_PhysicsAPI.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include <PxPhysicsAPI.h>
#include <vector>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

physx::PxFoundation* g_PhysicsFoundation = nullptr;
physx::PxPhysics* g_Physics = nullptr;
physx::PxScene* g_PhysicsScene = nullptr;
physx::PxDefaultErrorCallback gDefaultErrorCallback;
physx::PxDefaultAllocator gDefaultAllocatorCallback;
physx::PxSimulationFilterShader gDefaultFilterShader = physx::PxDefaultSimulationFilterShader;
physx::PxMaterial* g_PhysicsMaterial = nullptr;
physx::PxCooking* g_PhysicsCooker = nullptr;
std::vector<physx::PxRigidDynamic*> g_PhysXActors;
bool FirstRun;

_02_PhysicsAPI::_02_PhysicsAPI(){}
_02_PhysicsAPI::~_02_PhysicsAPI(){}

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
	sceneDesc.gravity = physx::PxVec3(0,-10.0f,0);
	sceneDesc.filterShader = gDefaultFilterShader;
	sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
	g_PhysicsScene = g_Physics->createScene(sceneDesc);

	if(g_PhysicsScene) {
		printf("start physx scene2\n");
	}
}

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

void addGrid(const glm::vec3& a_center, const glm:: mat4* a_transform,int increments,float size) {
	int halfIncrements = increments/2;
	for ( int i = 0 ; i < increments+1 ; ++i ){
		glm::vec4 colour = i == halfIncrements ? glm::vec4(1,0,1,1) : glm::vec4(0,0,0,1);
		glm::vec3 l1Start = glm::vec3((-halfIncrements + i)*size, 0, halfIncrements*size);
		glm::vec3 l1End = glm::vec3((-halfIncrements + i)*size, 0, -halfIncrements*size);
		glm::vec3 l2Start = glm::vec3(halfIncrements*size, 0, (-halfIncrements + i)*size);
		glm::vec3 l2End = glm::vec3(-halfIncrements*size, 0, (-halfIncrements + i)*size);
		l1Start = (*a_transform * glm::vec4(l1Start,0)).xyz + a_center;
		l1End = (*a_transform * glm::vec4(l1End,0)).xyz + a_center;
		l2Start = (*a_transform * glm::vec4(l2Start,0)).xyz + a_center;
		l2End = (*a_transform * glm::vec4(l2End,0)).xyz + a_center;
		Gizmos::addLine( l1Start, l1End, colour);
		Gizmos::addLine(l2Start, l2End, colour);
	}
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

void upDatePhysx(){
	g_PhysicsScene->simulate( 1/120.0f );
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

void cleanUpPhsyx() {
	g_PhysicsCooker->release();
	g_PhysicsScene->release();
	g_Physics->release();
	g_PhysicsFoundation->release();
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

bool _02_PhysicsAPI::onCreate(int a_argc, char* a_argv[]) {
	Gizmos::create(100,UINT_MAX * 0.0001);
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,2,0),glm::vec3(9,2,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	//!- TUTORIAL
	setUpPhysXTutorial();
	setUpVisualDebugger();
	FirstRun = true;
	
	//!- TUTORIAL

	return true;
}

void _02_PhysicsAPI::onUpdate(float a_deltaTime) {

	glm::vec4 pos = m_cameraMatrix[3];
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 0 );

	if (glfwGetKey(m_window,'A') == GLFW_PRESS) {
		pos.z += (pos.z > 10) ? 0 : 4 * a_deltaTime;
	}else if (glfwGetKey(m_window,'D') == GLFW_PRESS) {
		pos.z -= (pos.z < -10) ? 0 : 4 * a_deltaTime;
	}

	m_cameraMatrix[3] = pos;

	Gizmos::clear();
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );
	for ( int i = 0 ; i < 21 ; ++i ) {
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );		
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}

	//!- TUTORIAL
	if (FirstRun == true){
		FirstRun = false;
		
		// Floor
		AddPlane(glm::vec3(0,0,1));

		// front wall
		AddBox(glm::vec3(-2,10,0),glm::vec3(1,10,10),1000000);
		// front wall
		AddBox(glm::vec3(12,10,0),glm::vec3(1,10,10),1000000);

		glm::vec3 size		(0.2f,0.1f,0.4f);
		glm::vec3 extent	(0.6f,0.6f,8.0f);
		float stride = 2;
		for (float x = size.x * 0.5f; x < extent.x; x += size.x){
			for (float z = size.z * 0.5f; z < extent.z; z += size.z){
				for (float y = size.y * 0.5f; y < extent.y; y += size.y){
					AddBox(glm::vec3(x, y, (extent.z * 0.5f) - z),glm::vec3(size * 0.5f),100);
				}
			}
		}
		for (float z = size.z * 0.5f; z < extent.z; z += size.z * stride){
			AddBox(glm::vec3(0.4f, extent.y * (size.y * 2) + (size.z * 1.75f), (extent.z * 0.5f) - z),glm::vec3(size.yzx * 0.5f),100);
		}
	}

	static bool bLeftPressed = false;
	static float counter = 0;
	if (glfwGetMouseButton(m_window,GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
		if (!bLeftPressed){
			bLeftPressed = true;
			AddBullet(m_cameraMatrix,25,0.1f,100000);
		}
	}else{
		bLeftPressed = false;
	}

	int oldrand = rand();
	srand(42);
	upDatePhysx();
	srand(oldrand);
	
	if (glfwGetKey(m_window,GLFW_KEY_F1) == GLFW_PRESS) {
		g_PhysicsScene->release();
		g_PhysXActors.clear();
		physx::PxSceneDesc sceneDesc(g_Physics->getTolerancesScale());
		sceneDesc.gravity = physx::PxVec3(0,-10.0f,0);
		sceneDesc.filterShader = gDefaultFilterShader;
		sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
		g_PhysicsScene = g_Physics->createScene(sceneDesc);

		if(g_PhysicsScene) {
			printf("start physx scene2\n");
		}

		FirstRun = true;
	}
	//!- TUTORIAL

	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS){
		quit();
	}
}

void _02_PhysicsAPI::onDraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	//!- TUTORIAL
	
	//!- TUTORIAL
	
	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void _02_PhysicsAPI::onDestroy(){
	//!- TUTORIAL
	cleanUpPhsyx();
	//!- TUTORIAL
	Gizmos::destroy();
}

int main(int argc, char* argv[]) {
	Application* app = new _02_PhysicsAPI();
	if (app->create("AIE - 02_PhysicsAPI",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true){
		app->run();
	}
	delete app;
	return 0;
}

