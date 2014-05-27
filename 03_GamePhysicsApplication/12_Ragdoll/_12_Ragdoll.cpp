#include "_12_Ragdoll.h"
#include <string.h>;

#include "PxShape.h"
#include <iostream>;
#include "PxScene.h"
#include "PxSceneQueryReport.h"
#include "extensions/PxJoint.h"
#include "PxRigidDynamic.h"
#include "extensions/PxDistanceJoint.h"
#include "extensions/PxSphericalJoint.h"
#include "PxArticulationLink.h"
#include "PxShape.h"

std::vector<PxJoint*>joints;

using namespace std;

using namespace physx;


//create some constants for axis of rotation to make definition of quaternions a bit neater
const PxVec3 X_AXIS = PxVec3(1,0,0);
const PxVec3 Y_AXIS = PxVec3(0,1,0);
const PxVec3 Z_AXIS = PxVec3(0,0,1);
//Parts which make up our ragdoll
enum RagDollParts
{
	NO_PARENT = -1,
	LOWER_SPINE,
	LEFT_PELVIS,
	RIGHT_PELVIS,
	LEFT_UPPER_LEG,
	RIGHT_UPPER_LEG,
	LEFT_LOWER_LEG,
	RIGHT_LOWER_LEG,
	LEFT_FOOT,
	RIGHT_FOOT,
	UPPER_SPINE,
	LEFT_CLAVICLE,
	RIGHT_CLAVICLE,
	NECK,
	HEAD,
	LEFT_UPPER_ARM,
	RIGHT_UPPER_ARM,
	LEFT_LOWER_ARM,
	RIGHT_LOWER_ARM,
	LEFT_HAND,
	RIGHT_HAND,
};

//simple ragdoll example
//format is: Local rotation, parentID, bone radius, bone length (not including radius), parent offset joint position, child offset joint position, name);

RagdollNode* ragdollSnakeData[] = {
	new RagdollNode(PxQuat::createIdentity(),NO_PARENT,4,1,1,1,"root"),
	new RagdollNode(PxQuat::createIdentity(),0,4,1,-1,1,"node1"),
	new RagdollNode(PxQuat::createIdentity(),1,4,1,-1,1,"node2"),
	NULL
};


//example of a more complicated jointed skeleton
RagdollNode* ragdollAlienThingData[] = 
{
	new RagdollNode(PxQuat::createIdentity()			,NO_PARENT	,16	,1		,1		,1	,"root"),
	new RagdollNode(PxQuat(PxPi/2.0f-0.4f	,Z_AXIS)	,0			,8	,1		,-1		,1	,"node1"),
	new RagdollNode(PxQuat(PxPi/2.0f-0.3f	,Z_AXIS)	,0			,8	,1		,-0.75	,1	,"node2"),
	new RagdollNode(PxQuat(PxPi/2.0f-0.2f	,Z_AXIS)	,0			,8	,1		,-0.5	,1	,"node2"),
	new RagdollNode(PxQuat(PxPi/2.0f-0.1f	,Z_AXIS)	,0			,8	,1		,-0.25	,1	,"node3"),
	new RagdollNode(PxQuat(PxPi/2.0f		,Z_AXIS)	,0			,8	,1		, 0		,1	,"node"),
	new RagdollNode(PxQuat(PxPi/2.0f+0.1f	,Z_AXIS)	,0			,8	,1		, 0.25	,1	,"node"),
	new RagdollNode(PxQuat(PxPi/2.0f+0.2f	,Z_AXIS)	,0			,8	,1		, 0.5	,1	,"node"),
	new RagdollNode(PxQuat(PxPi/2.0f+0.3f	,Z_AXIS)	,0			,8	,1		, 0.75	,1	,"node"),
	new RagdollNode(PxQuat(PxPi/2.0f+0.4f	,Z_AXIS)	,0			,8	,1		, 1		,1	,"node"),
	new RagdollNode(PxQuat(PxPi/2.0f-0.4f	,Z_AXIS)	,1			,8	,0.5	,-1		,1	,"node1"),
	new RagdollNode(PxQuat(PxPi/2.0f-0.3f	,Z_AXIS)	,2			,8	,0.5	,-1		,1	,"node2"),
	new RagdollNode(PxQuat(PxPi/2.0f-0.2f	,Z_AXIS)	,3			,8	,0.5	,-1		,1	,"node2"),
	new RagdollNode(PxQuat(PxPi/2.0f-0.1f	,Z_AXIS)	,4			,8	,0.5	,-1		,1	,"node3"),
	new RagdollNode(PxQuat(PxPi/2.0f		,Z_AXIS)	,5			,8	,0.5	,-1		,1	,"node"),
	new RagdollNode(PxQuat(PxPi/2.0f+0.1f	,Z_AXIS)	,6			,8	,0.5	,-1		,1	,"node"),
	new RagdollNode(PxQuat(PxPi/2.0f+0.2f	,Z_AXIS)	,7			,8	,0.5	,-1		,1	,"node"),
	new RagdollNode(PxQuat(PxPi/2.0f+0.3f	,Z_AXIS)	,8			,8	,0.5	,-1		,1	,"node"),
	new RagdollNode(PxQuat(PxPi/2.0f+0.4f	,Z_AXIS)	,9			,8	,0.5	,-1		,1	,"node"),
	NULL
};


//complex humanoid ragdoll example
RagdollNode* ragdollData[] = 
{
	new RagdollNode(PxQuat(PxPi/2.0f,Z_AXIS)		,NO_PARENT			,1.0f,3.00f, 1.0f, 1.0f,"lowerspine"),
	new RagdollNode(PxQuat(PxPi,Z_AXIS)				,LOWER_SPINE		,1.0f,1.00f,-1.0f, 1.0f,"leftpelvis"),
	new RagdollNode(PxQuat(0,Z_AXIS)				,LOWER_SPINE		,1.0f,1.00f,-1.0f, 1.0f,"rightpelvis"),
	new RagdollNode(PxQuat(PxPi/2.0f+0.2f,Z_AXIS)	,LEFT_PELVIS		,5.0f,2.00f,-1.0f, 1.0f,"leftupperleg"),
	new RagdollNode(PxQuat(PxPi/2.0f-0.2f,Z_AXIS)	,RIGHT_PELVIS		,5.0f,2.00f,-1.0f, 1.0f,"rightupperleg"),
	new RagdollNode(PxQuat(PxPi/2.0f+0.2f,Z_AXIS)	,LEFT_UPPER_LEG		,5.0f,1.75f,-1.0f, 1.0f,"leftlowerleg"),
	new RagdollNode(PxQuat(PxPi/2.0f-0.2f,Z_AXIS)	,RIGHT_UPPER_LEG	,5.0f,1.75f,-1.0f, 1.0f,"rightlowerleg"),
	new RagdollNode(PxQuat(PxPi/2.0f,Y_AXIS)		,LEFT_LOWER_LEG		,1.5f,1.50f,-1.0f, 1.0f,"leftfoot"),
	new RagdollNode(PxQuat(PxPi/2.0f,Y_AXIS)		,RIGHT_LOWER_LEG	,1.5f,1.50f,-1.0f, 1.0f,"rightfoot"),
	new RagdollNode(PxQuat(PxPi/2.0f,Z_AXIS)		,LOWER_SPINE		,1.0f,3.00f, 1.0f,-1.0f,"upperspine"),
	new RagdollNode(PxQuat(PxPi,Z_AXIS)				,UPPER_SPINE		,1.0f,1.50f, 1.0f, 1.0f,"leftclavicle"),
	new RagdollNode(PxQuat(0,Z_AXIS)				,UPPER_SPINE		,1.0f,1.50f, 1.0f, 1.0f,"rightclavicle"),
	new RagdollNode(PxQuat(PxPi/2.0f,Z_AXIS)		,UPPER_SPINE		,1.0f,1.00f, 1.0f,-1.0f,"neck"),
	new RagdollNode(PxQuat(PxPi/2.0f,Z_AXIS)		,NECK				,1.0f,3.00f, 1.0f,-1.0f,"HEAD"),
	new RagdollNode(PxQuat(PxPi-.3,Z_AXIS)			,LEFT_CLAVICLE		,3.0f,1.50f,-1.0f, 1.0f,"leftupperarm"),
	new RagdollNode(PxQuat(0.3,Z_AXIS)				,RIGHT_CLAVICLE		,3.0f,1.50f,-1.0f, 1.0f,"right upperarm"),
	new RagdollNode(PxQuat(PxPi-.3,Z_AXIS)			,LEFT_UPPER_ARM		,3.0f,1.00f,-1.0f, 1.0f,"leftlowerarm"),
	new RagdollNode(PxQuat(0.3,Z_AXIS)				,RIGHT_UPPER_ARM	,3.0f,1.00f,-1.0f, 1.0f,"rightlowerarm"),
	new RagdollNode(PxQuat(PxPi-.3,Z_AXIS)			,LEFT_LOWER_ARM		,1.0f,1.50f,-1.0f, 1.0f,"lefthand"),
	new RagdollNode(PxQuat(0.3,Z_AXIS)				,RIGHT_LOWER_ARM	,1.0f,1.50f,-1.0f, 1.0f,"righthand"),
	NULL
};

PxFoundation* g_PhysicsFoundation = nullptr;
PxPhysics* g_Physics = nullptr;
PxScene* g_PhysicsScene = nullptr;

PxDefaultErrorCallback gDefaultErrorCallback;
PxDefaultAllocator gDefaultAllocatorCallback;
PxSimulationFilterShader gDefaultFilterShader = PxDefaultSimulationFilterShader;
PxMaterial* g_PhysicsMaterial = nullptr;

//List of our actors
std::vector<PxRigidActor*> g_PhysXActors;
 //list of articulations for rendering purposers
std::vector<PxArticulation*> g_PhysXActorsRagDolls;

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

bool GLFWMouseButton1Down = false;

RagdollTutorial::RagdollTutorial(){}
RagdollTutorial::~RagdollTutorial(){}

class myAllocator: public PxAllocatorCallback
{
public:
        virtual ~myAllocator() {}
        virtual void* allocate(size_t size, const char* typeName, const char* filename, int line)
		{
			void* pointer = _aligned_malloc(size,16);
			return pointer;
		}
        virtual void deallocate(void* ptr)
		{
			 _aligned_free(ptr);
		}
};

void RagdollTutorial::setUpVisualDebugger()
{
  // check if PvdConnection manager is available on this platform
  if (NULL == g_Physics->getPvdConnectionManager())
      return;
  // setup connection parameters
  const char*     pvd_host_ip = "127.0.0.1";  // IP of the PC which is running PVD
  int             port        = 5425;         // TCP port to connect to, where PVD is listening
  unsigned int    timeout     = 100;          // timeout in milliseconds to wait for PVD to respond,
                                              // consoles and remote PCs need a higher timeout.
  PxVisualDebuggerConnectionFlags connectionFlags = PxVisualDebuggerConnectionFlag::Debug
      | PxVisualDebuggerConnectionFlag::Profile | PxVisualDebuggerConnectionFlag::Memory;
  // and now try to connect
	    PxVisualDebuggerExt::createConnection(g_Physics->getPvdConnectionManager(),
			pvd_host_ip, port, timeout, connectionFlags);
  //    pvd_host_ip, port, timeout, connectionFlags));
}

bool RagdollTutorial::onCreate(int a_argc, char* a_argv[]) 
{
	m_file = NULL;

	// initialise the Gizmos helper class
	Gizmos::create(SHRT_MAX,SHRT_MAX);
	setUpPhysXTutorial();
	// create a world-space matrix for a camera
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(-28,4,10),glm::vec3(-28,0,0), glm::vec3(0,1,0)) );
	
	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.5f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 10000.0f);

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	return true;
}

void RagdollTutorial::setUpPhysXTutorial()
{
	PxAllocatorCallback *myCallback = new myAllocator();
	g_PhysicsFoundation = PxCreateFoundation(PX_PHYSICS_VERSION,*myCallback, gDefaultErrorCallback);
	g_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *g_PhysicsFoundation, PxTolerancesScale());
	PxInitExtensions(*g_Physics);
	//create physics material
	g_PhysicsMaterial = g_Physics->createMaterial(0.2f,0.2f,.25f);
	PxSceneDesc sceneDesc(g_Physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0,-10.0f,0);
	sceneDesc.filterShader =  &physx::PxDefaultSimulationFilterShader;
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
	g_PhysicsScene = g_Physics->createScene(sceneDesc);
	g_PhysicsScene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LIMITS, 1.0f);
	g_PhysicsScene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LIMITS, 1.0f);
	setUpVisualDebugger();
	tutorial_12();  //set up our tutorial
}

//simple example which just creates sname
void RagdollTutorial::tutorial_12a()
{
	PxArticulation* ragDollArticulation;
	//create ragdoll
	ragDollArticulation = makeRagdoll(ragdollSnakeData,PxTransform(PxVec3(0,0,0)),.1f);
	//add it to the physx scene
	g_PhysicsScene->addArticulation(*ragDollArticulation);
	//add it to our list of articulations so we can render it
	g_PhysXActorsRagDolls.push_back(ragDollArticulation);
	//add a plane
	PxTransform pose = PxTransform( PxVec3(0.0f, -4, 0.0f),PxQuat(PxHalfPi*1.1, PxVec3(0.0f, 0.0f, 1.0f)));
	PxRigidStatic* plane = PxCreateStatic(*g_Physics, pose,PxPlaneGeometry(), *g_PhysicsMaterial);
	//add it to the physX scene
	g_PhysicsScene->addActor(*plane);
}

//more complex example which creates
void RagdollTutorial::tutorial_12()
{
	PxTransform localPose;
	PxArticulation* ragDollArticulation;
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(1000,0,0),glm::vec3(-2000,10,0), glm::vec3(0,1,0)) );

	//create first ragdoll at the top of the stairs
	ragDollArticulation = makeRagdoll(ragdollData,PxTransform(PxVec3(58,93.5,0)),.1f);
	g_PhysicsScene->addArticulation(*ragDollArticulation);
	g_PhysXActorsRagDolls.push_back(ragDollArticulation);
	
	//create another three as examples
	ragDollArticulation = makeRagdoll(ragdollAlienThingData,PxTransform(PxVec3(-20,0,0)),.1f);
	g_PhysicsScene->addArticulation(*ragDollArticulation);
	g_PhysXActorsRagDolls.push_back(ragDollArticulation);
	ragDollArticulation = makeRagdoll(ragdollData,PxTransform(PxVec3(-28,0,0)),.1f);
	g_PhysicsScene->addArticulation(*ragDollArticulation);
	g_PhysXActorsRagDolls.push_back(ragDollArticulation);
	ragDollArticulation = makeRagdoll(ragdollSnakeData,PxTransform(PxVec3(-36,0,0)),.1f);
	g_PhysicsScene->addArticulation(*ragDollArticulation);
	g_PhysXActorsRagDolls.push_back(ragDollArticulation);

	//add our steps
	addSteps(PxTransform(PxVec3(0,0,0)),30);

	//add a ground plane
	PxTransform pose = PxTransform( PxVec3(0.0f, -4, 0.0f),PxQuat(PxHalfPi*1, PxVec3(0.0f, 0.0f, 1.0f)));
	PxRigidStatic* plane = PxCreateStatic(*g_Physics, pose,PxPlaneGeometry(), *g_PhysicsMaterial);
	g_PhysicsScene->addActor(*plane);
}

//create a flight of steps for actor to fall down
void RagdollTutorial::addSteps(PxTransform transform, int numberSteps)
{
	for(int count = 0 ; count < numberSteps;count++)
	{
		//step extents
		float width = 1.0;
		float height = 3;
		float length = 20;
		PxVec3 extents(width,height,length);
		//calculate the transform for this step
		PxTransform pose = transform;
		pose.p.x +=  width* 2 * count;
		pose.p.y += height*.5f * count;
		extents.y += height*.5f * count;
		PxBoxGeometry box(extents);
		PxRigidStatic*staticObject = PxCreateStatic(*g_Physics,pose,box,*g_PhysicsMaterial);
		//add it to the physX scene
		g_PhysicsScene->addActor(*staticObject);
		g_PhysXActors.push_back(staticObject);
	}
}

//make a ragdoll out of actors and joints to demonstrate stability problems
void RagdollTutorial::makeRagdollUsingActors(RagdollNode** nodeArray,PxTransform worldPos,float scaleFactor)
{	
	//get a pointer to the first element in the array of nodes
	RagdollNode** currentNode = nodeArray;
	//while there are more nodes to process..
	while(*currentNode!= NULL)
	{
		//get a pointer to the data for this node
		RagdollNode* currentNodePtr = *currentNode;
		//reserver some space for the pointer to the parent npde
		RagdollNode* parentNode;
		//get local copies of the links dimensions
		float radius = currentNodePtr->radius*scaleFactor;
		float halfLength = currentNodePtr->halfLength*scaleFactor;
		float childHalfLength = radius + halfLength;
		float parentHalfLength;
		//get a local copy of the parents index
		int parentIdx = currentNodePtr->parentNodeIdx;
		currentNodePtr->scaledGobalPos = worldPos.p;  //set this to world pos of node for the case of the root. If it's not the root it will get over writen when we know it's actual coordinate
		if(parentIdx!= -1)
		{
			//if this node has a parent then change it's global pose to be relative to the parent
			parentNode = *(nodeArray + parentIdx);
			//get the parents bone dimensions
			parentHalfLength = (parentNode->radius + parentNode->halfLength) *scaleFactor;
			PxVec3 parentNodePos = parentNode->scaledGobalPos;
			//get the relative position of the child from the parent
			PxVec3 currentRelative = currentNodePtr->childLinkPos * currentNodePtr->globalRotation.rotate(PxVec3(childHalfLength,0,0));
			PxVec3 parentRelative = - currentNodePtr->parentLinkPos * parentNode->globalRotation.rotate(PxVec3(parentHalfLength,0,0));
			//store the global pose in the node for future reference
			currentNodePtr->scaledGobalPos = parentNodePos - (parentRelative + currentRelative);
		}
		//create the geometry for the shape which will be the collider for this link
		float jointSpace = .01f;
		float capsuleHalfLength = (halfLength>jointSpace?halfLength-jointSpace:0)+.01f; 
		PxCapsuleGeometry capsule(radius,capsuleHalfLength);
		float density = 1;
		PxRigidDynamic* parentActorPtr = NULL;
		PxQuat rotation = currentNodePtr->globalRotation;
		PxTransform scaledTransform = PxTransform(currentNodePtr->scaledGobalPos,rotation) ;
		scaledTransform.p = scaledTransform.p + worldPos.p;
		//get it's world transform
		//create the actor and add it to the physx scene and to our list of actors for rendering
		PxRigidDynamic* thisActorPtr = PxCreateDynamic(*g_Physics, scaledTransform, capsule,*g_PhysicsMaterial, density);
		g_PhysicsScene->addActor(*thisActorPtr);
		g_PhysXActors.push_back(thisActorPtr);

		currentNodePtr->actorPtr = thisActorPtr;
		//if the link has a parent then we need to link it to the parent with a joint
		if(parentIdx!= -1)
		{
			//get a pointer to the parent
			parentActorPtr = parentNode->actorPtr;
			//work out the constraint frames so we know how to reposition the joint to be between the bones
			PxQuat frameRotation = parentNode->globalRotation.getConjugate() * currentNodePtr->globalRotation;
			PxTransform parentConstraintFrame = PxTransform(PxVec3(currentNodePtr->parentLinkPos * parentHalfLength,0,0),frameRotation);
			PxTransform thisConstraintFrame = PxTransform(PxVec3(currentNodePtr->childLinkPos * childHalfLength,0,0));
			PxD6Joint* d6joint = NULL;
			//use a d6 joint because it's the best one for the jobe
			d6joint =PxD6JointCreate(*g_Physics,parentActorPtr,parentConstraintFrame,thisActorPtr,thisConstraintFrame); 
			if(d6joint)
			{
				//set up various parameters for the joint
				d6joint->setProjectionLinearTolerance(.1f);
				d6joint->setProjectionAngularTolerance(.1f);

				PxD6JointDrive drive(10.0f, -20.0f, PX_MAX_F32, true);
				d6joint->setDrive(PxD6Drive::eX, drive);
				d6joint->setDrive(PxD6Drive::eY, drive);
				d6joint->setDrive(PxD6Drive::eZ, drive);

//Drive the joint to the local(actor[0]) origin - since no angular dofs are free, the angular part of the transform is ignored

				d6joint->setDrivePosition(PxTransform(PxVec3(0,0,0)));
				d6joint->setDriveVelocity(PxVec3(0,0,0),PxVec3(0,0,0));
			}
		}
		thisActorPtr->putToSleep();
		currentNode++;
	}
}

//make a ragdoll using an articulations
PxArticulation* RagdollTutorial::makeRagdoll(RagdollNode** nodeArray,PxTransform worldPos,float scaleFactor)
{

	//create the articulation for our ragdoll
	PxArticulation *articulation = g_Physics->createArticulation();
	PxMaterial* ragdollMaterial = g_Physics->createMaterial(0.4f,0.4f,1.0f);
	RagdollNode** currentNode = nodeArray;
	//while there are more nodes to process...
	while(*currentNode!= NULL)
	{
		//get a pointer to the current node
		RagdollNode* currentNodePtr = *currentNode;
		//create a pointer ready to hold the parent node pointer if there is one
		RagdollNode* parentNode;
		//get scaled values for capsule
		float radius = currentNodePtr->radius*scaleFactor;
		float halfLength = currentNodePtr->halfLength*scaleFactor;
		float childHalfLength = radius + halfLength;
		float parentHalfLength;

		//copy the parentIDX
		int parentIdx = currentNodePtr->parentNodeIdx;
		//get a pointer to the parent
		PxArticulationLink* parentLinkPtr = NULL;
		currentNodePtr->scaledGobalPos = worldPos.p;  //set this to world pos of node. If it's not the root it will get over writen when we know it's actual coordinate
		if(parentIdx!= -1)
		{
			//if there is a parent then we need to work out our local position for the link
			//get a pointer to the parent node
			parentNode = *(nodeArray + parentIdx);
			//get a pointer to the link for the parent
			parentLinkPtr = parentNode->linkPtr;
			parentHalfLength = (parentNode->radius + parentNode->halfLength) *scaleFactor;

			//work out the local position of the node
			PxVec3 currentRelative = currentNodePtr->childLinkPos * currentNodePtr->globalRotation.rotate(PxVec3(childHalfLength,0,0));
			PxVec3 parentRelative = - currentNodePtr->parentLinkPos * parentNode->globalRotation.rotate(PxVec3(parentHalfLength,0,0));
			currentNodePtr->scaledGobalPos = parentNode->scaledGobalPos- (parentRelative + currentRelative);
		}

		//build the transform for the link
		PxTransform linkTransform = PxTransform(currentNodePtr->scaledGobalPos,currentNodePtr->globalRotation) ;
		//create the link in the articulation
		PxArticulationLink* link = articulation->createLink(parentLinkPtr, linkTransform);

		//add the pointer to this link into the ragdoll data so we have it for later when we want to link to it
		currentNodePtr->linkPtr = link;
		float jointSpace = .01f; //allows us to set a gap between joints
		float capsuleHalfLength = (halfLength>jointSpace?halfLength-jointSpace:0)+.01f; 
		PxCapsuleGeometry capsule(radius,capsuleHalfLength);
		link->createShape(capsule,*ragdollMaterial); //adds a capsule collider to the link
		PxRigidBodyExt::updateMassAndInertia(*link, 50.0f);  //adds some mass to the link

		if(parentIdx!= -1)
		{
			//get the pointer to the joint from the link
			PxArticulationJoint *joint = link->getInboundJoint();
			//get the relative rotation of this link
			PxQuat frameRotation = parentNode->globalRotation.getConjugate() * currentNodePtr->globalRotation;
			//set the parent contraint frame
			PxTransform parentConstraintFrame = PxTransform(PxVec3(currentNodePtr->parentLinkPos * parentHalfLength,0,0),frameRotation);
			//set the child constraint frame (this the constraint frame of the newly added link)
			PxTransform thisConstraintFrame = PxTransform(PxVec3(currentNodePtr->childLinkPos * childHalfLength,0,0));
			//set up the poses for the joint so it is in the correct place
			joint->setParentPose(parentConstraintFrame);
			joint->setChildPose(thisConstraintFrame);

			//set up some constraints to stop it flopping around
			joint->setSpring(20);
			joint->setDamping(100);
			joint->setSwingLimitEnabled(true);
			joint->setSwingLimit(0.4f,0.4f);
			joint->setTwistLimit(.4f,.4f);
			joint->setTwistLimitEnabled(true);
		}
		currentNode++;
	}

	//put it to sleep so we can see it in it's starting pose
	articulation->putToSleep();
	return articulation;
}

//add our ball gun so we can shoot at the ragdolls
void RagdollTutorial::tutorial_3_ballGun()
{
	float muzzleSpeed = -50;
	GLFWwindow* window = glfwGetCurrentContext();
	if (glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_1) == GLFW_PRESS && !GLFWMouseButton1Down)
	{
		GLFWMouseButton1Down = true;
		PxSphereGeometry sphere(.5);
		//get the camera position from the camera matrix
		glm::vec3 position(m_cameraMatrix[3]);
		//get the camera rotationfrom the camera matrix
		glm::vec3 direction(m_cameraMatrix[2]);
		physx::PxVec3 velocity = physx::PxVec3(direction.x,direction.y,direction.z)* muzzleSpeed;
		float density = 5;
		PxTransform transform(PxVec3(position.x,position.y,position.z),PxQuat::createIdentity());
		PxRigidDynamic *dynamicActor = PxCreateDynamic(*g_Physics, transform, sphere,*g_PhysicsMaterial, density);
		dynamicActor->setLinearVelocity(velocity,true);
		g_PhysicsScene->addActor(*dynamicActor);
		g_PhysXActors.push_back(dynamicActor);
	}
	if (!glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_1) )
	{
		GLFWMouseButton1Down = false;
	}
	int index = 0;
}

void RagdollTutorial::onUpdate(float a_deltaTime) 
{
	// update our camera matrix using the keyboard/mouse
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );
	// clear all gizmos from last frame
	Gizmos::clear();
	if(a_deltaTime == 0){a_deltaTime = 1/60.0f;}
	static float count = 1;
	count+= .01f;
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );
	// quit our application when escape is pressed
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
	tutorial_3_ballGun();  //add this if we want to have a ball gun in the scene
	upDatePhysx(a_deltaTime);
}

void RagdollTutorial::upDatePhysx(float a_deltaTime)
{
	a_deltaTime = 1/60.0f;
	g_PhysicsScene->simulate( a_deltaTime );
	while (g_PhysicsScene->fetchResults() == false)
	{
	// don�t need to do anything here yet but we have to fetch results
	}
	// Add widgets to represent all the phsyX actors which are in the scene
	for (auto actor:g_PhysXActors)
	{
		{
			PxU32 nShapes = actor->getNbShapes();
			PxShape** shapes = new PxShape*[nShapes];
			actor->getShapes(shapes, nShapes);
			// Render all the shapes in the physx actor (for early tutorials there is just one)
			while (nShapes--)
			{
				 addWidget(shapes[nShapes],actor); 
			}
			delete [] shapes;
		}
	}

	for(auto articulation:g_PhysXActorsRagDolls)
	{
		{
			PxU32 nLinks = articulation->getNbLinks();
			PxArticulationLink** links = new PxArticulationLink*[nLinks];
			articulation->getLinks(links, nLinks);
			// Render all the shapes in the physx actor (for early tutorials there is just one)
			while (nLinks--)
			{
				PxArticulationLink* link = links[nLinks];
				PxU32 nShapes = link->getNbShapes();
				PxShape** shapes = new PxShape*[nShapes];
				link->getShapes(shapes, nShapes);
				while (nShapes--)
				{
					addWidget(shapes[nShapes],link); 
				}
			}
			delete [] links;
		}
	}
}

void RagdollTutorial::addWidget(PxShape* shape,PxRigidActor* actor) 
{ 
    PxGeometryType::Enum type = shape->getGeometryType();
    switch(type) 
    {  
		case PxGeometryType::eBOX:
			addBox(shape,actor);
		break;
		case PxGeometryType::eSPHERE:
			addSphere(shape,actor);
		break;
		case PxGeometryType::eCAPSULE:
			addCapsule(shape,actor);
		break;
    } 
} 

void RagdollTutorial::addSphere(PxShape* pShape,PxRigidActor* actor) 
{
	PxSphereGeometry geometry;
	float radius = 1;
	//get the geometry for this PhysX collision volume
	bool status = pShape->getSphereGeometry(geometry);
	if(status)
	{
		radius = geometry.radius;
	}
	//get the transform for this PhysX collision volume
	PxMat44 m(PxShapeExt::getGlobalPose(*pShape));
	glm::mat4 M(m.column0.x, m.column0.y, m.column0.z, m.column0.w,
	m.column1.x, m.column1.y, m.column1.z, m.column1.w,
	m.column2.x, m.column2.y, m.column2.z, m.column2.w,
	m.column3.x, m.column3.y, m.column3.z, m.column3.w);
	glm::vec3 position;
		 
	position.x = m.getPosition().x;
	position.y = m.getPosition().y;
	position.z = m.getPosition().z;
	//create a widget to represent it
	Gizmos::addSphere(position,10,10,radius,glm::vec4(1,0,1,1));
}

//helper function to convert PhysX matrix to Opengl 
glm::mat4 Px2Glm(PxMat44 m)
{
	glm::mat4 M(m.column0.x, m.column0.y, m.column0.z, m.column0.w,
	m.column1.x, m.column1.y, m.column1.z, m.column1.w,
	m.column2.x, m.column2.y, m.column2.z, m.column2.w,
	m.column3.x, m.column3.y, m.column3.z, m.column3.w);
	return M;
}
//helper function to convert PhysX vector to Opengl 
glm::vec3   Px2GlV3(PxVec3 v1)
{
	glm::vec3 v2;
	v2.x = v1.x;
	v2.y = v1.y;
	v2.z = v1.z;
	return v2;
}

void RagdollTutorial::addCapsule(PxShape* pShape,PxRigidActor* actor) 
{
	//creates a gizmo representation of a capsule using 2 spheres and a cylinder
	glm::vec4 colour(0,0,1,1);  //make our capsule blue
	PxCapsuleGeometry capsuleGeometry;
	float radius = 1; //temporary values whilst we try and get the real value from PhysX
	float halfHeight = 1;
	//get the geometry for this PhysX collision volume
	bool status = pShape->getCapsuleGeometry(capsuleGeometry);
	if(status)
	{
		//this should always happen but just to be safe we check the status flag
		radius = capsuleGeometry.radius; //copy out capsule radius
		halfHeight = capsuleGeometry.halfHeight; //copy out capsule half length
	}
	//get the world transform for the centre of this PhysX collision volume
	PxTransform transform = PxShapeExt::getGlobalPose(*pShape);
	//use it to create a matrix
	PxMat44 m(transform);
	//convert it to an open gl matrix for adding our gizmos
	glm::mat4 M = Px2Glm(m);
	//get the world position from the PhysX tranform
	glm::vec3 position = Px2GlV3(transform.p); 
	glm::vec4 axis(halfHeight,0,0,0);	//axis for the capsule
	axis = M * axis; //rotate axis to correct orientation
	//add our 2 end cap spheres...
	Gizmos::addSphere(position+axis.xyz,10,10,radius,colour);
	Gizmos::addSphere(position-axis.xyz,10,10,radius,colour);
	//the cylinder gizmo is oriented 90 degrees to what we want so we need to change the rotation matrix...
	glm::mat4 m2 = glm::rotate(M,11/7.0f,0.0f,0.0f,1.0f); //adds an additional rotation onto the matrix
	//now we can use this matrix and the other data to create the cylinder...
	Gizmos::addCylinderFilled(position,radius,halfHeight,10,colour,&m2);
}

void RagdollTutorial::addBox(PxShape* pShape,PxRigidActor* actor) 
{
	//get the geometry for this PhysX collision volume
	PxBoxGeometry geometry;
	float width = 1, height = 1, length = 1;
	bool status = pShape->getBoxGeometry(geometry);
	if(status)
	{
		width = geometry.halfExtents.x;
		height = geometry.halfExtents.y;
		length = geometry.halfExtents.z;
	}
	//get the transform for this PhysX collision volume
	PxMat44 m(PxShapeExt::getGlobalPose(*pShape));
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
	glm::vec4 colour = glm::vec4(1,0,0,1);
	if(actor->getName() != NULL && strcmp(actor->getName(),"Pickup1")) //seriously horrid hack so I can show pickups a different colour
	{
		colour = glm::vec4(0,1,0,1);
	}
	//create our box gizmo
	Gizmos::addAABBFilled(position,extents,colour,&M);
}

void RagdollTutorial::onDraw() 
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );

	// draw the gizmos from this frame
	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void RagdollTutorial::onDestroy()
{
	// clean up anything we created
	Gizmos::destroy();
	cleanUpPhsyx();
}

void RagdollTutorial::cleanUpPhsyx()
{
	g_PhysicsScene->release();
	g_Physics->release();
	g_PhysicsFoundation->release();
}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Application* app = new RagdollTutorial();
	if (app->create("AIE - Physx Tutorial",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
	// explicitly control the destruction of our application
	delete app;
	return 0;
}