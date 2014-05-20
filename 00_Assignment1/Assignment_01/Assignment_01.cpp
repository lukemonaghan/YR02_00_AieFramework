#include "Assignment_01.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

Assignment_01::Assignment_01(){}
Assignment_01::~Assignment_01(){}

bool Assignment_01::onCreate(int a_argc, char* a_argv[]) {
	// initialise the Gizmos helper class
	Gizmos::create();

	// create a world-space matrix for a camera
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(5,5,5),glm::vec3(0,2,0), glm::vec3(0,1,0)) );
	
	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);
	m_projectionMatrixHud2D = glm::ortho(0.0f,(float)DEFAULT_SCREENWIDTH,(float)DEFAULT_SCREENHEIGHT,0.0f,-10.0f,10.0f);

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);//we dont want this as the models are low poly and we want to see the backfaces inside buildings

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	bfArial = new BitFont("fonts/Arial.fnt");
	bfArial->SizeSet(16);

	FontShader = new Shader("Shaders/Assignment_FontShader.vert","Shaders/Assignment_FontShader.frag");
	FontShader->SetAttribs(3,0,"Position",1,"Color",2,"TexCoord1");
	FontShader->SetUniform("Model","m4fv",1,false,glm::value_ptr(glm::mat4(1)));
	FontShader->SetUniform("View","m4fv",1,false,glm::value_ptr(glm::mat4(1)));
	FontShader->SetUniform("Projection","m4fv",1,false,glm::value_ptr(m_projectionMatrixHud2D));

	fFlickerTime = 0.0f;
	bCameraType = false;
	bDebugOpen = false;
	bFirstRun = false;
	bCheck = true;
	return true;
	}

void Assignment_01::CheckFirstRun(){
	if (bFirstRun){
		LightShader = new Shader("Shaders/Assignment_01.vert","Shaders/Assignment_01.frag");
		LightShader->SetAttribs(5,0,"Position",1,"Colour",7,"UV",10,"UV2",2,"Normal");
		LightShader->SetUniform("Projection","m4fv",1,false,glm::value_ptr(m_projectionMatrix));

		l_LightClass = new Lights(5);
		//l_LightClass->SetAmbient(glm::vec3(1));
		l_LightClass->SetAmbient(glm::vec3(0.1f,0.1f,0.1f));
		l_LightClass->AddDirectional(glm::vec3(1,-1,1),glm::vec4(0.2f,0.2f,0.8f,1.0f));

		l_LightClass->AddPoint(glm::vec3(0, 4,0), 30,glm::vec4(1,0.0f,0,1));//floor
		IDs[0] = l_LightClass->AddPoint(glm::vec3(0, 8,0), 60,glm::vec4(1,0.5f,0,1));//floor
		IDs[1] = l_LightClass->AddPoint(glm::vec3(0,12,0), 90,glm::vec4(1,1.0f,0,1));//floor
		TessShader = new Shader("Shaders/Assignment/Tessellation.vert","Shaders/Assignment/Tessellation.frag","Shaders/Assignment/Tessellation.cont","Shaders/Assignment/Tessellation.eval");

		pFloor = new Plane(glm::vec3(-75,0,-75),glm::vec2(150,150),glm::vec2(2),true,60.0f);//tesselate this 16

		tGrassTexture = new Texture("textures/grass.png");
		tDirtTexture = new Texture("textures/dirt.png");
		tPathMap = new Texture("textures/path.png");
		tCloudFog = new Texture("textures/ParticleFog.png");
		tFire = new Texture("textures/FireParticle.png");
		tRain = new Texture("textures/RainParticle.png");

		CubeShader = new Shader("Shaders/07_CubeMap.vert","Shaders/07_CubeMap.frag");
		CubeShader->SetAttribs(1,0,"Position");

		m_Skybox = new Skybox(300,50);
		m_CubeMap = new CubeMap("./textures/SkyBox/night-sky/",".png");

		pFire = new GPUParticle(10000);
		pFire->setTextureID(tFire->ID());						//Texture
		pFire->setPosition(glm::vec3(0,12,0));					//Position
		pFire->setColourStart(glm::vec4(1,0.4,0,1));			//Colour
		pFire->setColourEnd(glm::vec4(0.2,0,0,0.0f));
		pFire->setVelocity(glm::vec3(0,5,0));					//Velocity
		pFire->setVelocityJitterMax(glm::vec3(5,5,5));			//Velocity Jitter
		pFire->setRange(glm::vec3(2,28,2));						//Spawner
		pFire->setRotationStepMax(45.0f);						//max rotation in degrees per second
		pFire->setAcceleration(glm::vec3(0.5f,-0.3f,0.5f));		//wind
		pFire->setSizeStart(glm::vec2(1));						//Size
		pFire->setSizeEnd(glm::vec2(0));
		pFire->setLifeSpanStart(2);								//Lifetime
		pFire->setLifeSpanEnd(4);
		//pFire->PreWarm(50.0f);

		pSmoke = new GPUParticle(5000);
		pSmoke->setTextureID(tCloudFog->ID());					//Texture
		pSmoke->setPosition(glm::vec3(0,12,0));					//Position
		pSmoke->setColourStart(glm::vec4(0.4,0.4,0.4,0.5f));	//Colour
		pSmoke->setColourEnd(glm::vec4(0.2,0.2,0.2,0.0f));
		pSmoke->setVelocity(glm::vec3(0,4,0));					//Velocity
		pSmoke->setVelocityJitterMax(glm::vec3(3,5,3));			//Velocity Jitter
		pSmoke->setRange(glm::vec3(2,28,2));					//Spawner
		pSmoke->setRotationStepMax(90.0f);						//max rotation in degrees per second
		pSmoke->setAcceleration(glm::vec3(0.25f,0.0f,0.25f));	//wind
		pSmoke->setSizeStart(glm::vec2(2));						//Size
		pSmoke->setSizeEnd(glm::vec2(3));
		pSmoke->setLifeSpanStart(2);							//Lifetime
		pSmoke->setLifeSpanEnd(5);

		pRain = new GPUParticle(50000);
		pRain->setTextureID(tRain->ID());						//Texture
		pRain->setPosition(glm::vec3(0,250,0));					//Position
		pRain->setColourStart(glm::vec4(0.0,0.0,1.0,1.0f));		//Colour
		pRain->setColourEnd  (glm::vec4(0.0,0.0,0.2,0.0f));
		pRain->setVelocity(glm::vec3(0,-20,0));					//Velocity
		pRain->setVelocityJitterMax(glm::vec3(0,20,0));			//VelocityJitter
		pRain->setRange(glm::vec3(150,0,150));					//Spawner
		pRain->setSizeStart(glm::vec2(0.2f));					//Size
		pRain->setSizeEnd(glm::vec2(0));
		pRain->setLifeSpanStart(50);							//Lifetime
		pRain->setLifeSpanEnd(1000);

		GenerateMap(true);
		l_LightClass->UpdateUniforms(LightShader);
		l_LightClass->UpdateUniforms(TessShader);

		bFirstRun = false;
	}
}

void Assignment_01::onUpdate(float a_deltaTime) {
	CheckFirstRun();
	if (bCheck) {bCheck = !bCheck;bFirstRun = true;}
	// update our camera matrix using the keyboard/mouse
	if (!bFirstRun){
		static bool bPressed;
		if (glfwGetKey(m_window,'`')){
			if (!bPressed){
				bPressed = true;
				bCameraType = !bCameraType;
			}
		}else{
			if (bPressed) bPressed = false;
		}
		(bCameraType) ? Utility::GravMovement( m_cameraMatrix, a_deltaTime, 5,0) : Utility::freeMovement( m_cameraMatrix, a_deltaTime, 20);
		LightShader->SetUniform("cameraPosition","3fv",1,&m_cameraMatrix[3].xyz);

		fFlickerTime += ((rand()%100)-20)*0.01f;
		fFlickerTime = glm::clamp(fFlickerTime,0.0f,1.0f);
		l_LightClass->PointDistance(IDs[0],15 + 15 * fFlickerTime);
		l_LightClass->PointDistance(IDs[1],30 + 30 * fFlickerTime);
		l_LightClass->UpdateUniforms(LightShader);
		l_LightClass->UpdateUniforms(TessShader);

		if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS){quit();}
		if (glfwGetKey(m_window,GLFW_KEY_F1) == GLFW_PRESS){ glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ); }else{ glPolygonMode( GL_FRONT_AND_BACK, GL_FILL ); }
		static bool bShaderReload;
		if (glfwGetKey(m_window,GLFW_KEY_F8) == GLFW_PRESS){ 
			if (!bShaderReload){
				bShaderReload = true;
				system("cls");
				LightShader->ReloadShader();
				CubeShader->ReloadShader();
				FontShader->ReloadShader();
				TessShader->ReloadShader();
			}
		}else if (bShaderReload){ 
			bShaderReload = false;
		}
	}
}

void Assignment_01::onDraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (!bFirstRun){
		if (bDebugOpen){
			Gizmos::clear();
		}
		// clear the backbuffer
		viewMatrix = glm::inverse( m_cameraMatrix );
		LightShader->SetUniform("View","m4fv",1,false,glm::value_ptr(viewMatrix));
		TessShader->SetUniform("View","m4fv",1,false,glm::value_ptr(viewMatrix));

		m_Skybox->Draw(m_CubeMap,CubeShader,(m_projectionMatrix * viewMatrix));
		//use our shader
		//set uniforms
		TessShader->SetUniform("projectionView","m4fv",1, false, glm::value_ptr(m_projectionMatrix * viewMatrix) );
		TessShader->SetUniform("displacementScale","1f",1, 1.0f );
		TessShader->SetUniform("TessLevel","1f",1, 64.0f );
		//set textures
		TessShader->SetTexture("textureMap",0,tGrassTexture->ID());
		TessShader->SetTexture("textureMapDirt",1,tDirtTexture->ID());
		TessShader->SetTexture("displacementMap",2,tPathMap->ID());
		pFloor->DrawTess();

		DrawMap();

		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		pFire->draw(m_cameraMatrix,m_projectionMatrix);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		pSmoke->draw(m_cameraMatrix,m_projectionMatrix);
		pRain->draw(m_cameraMatrix,m_projectionMatrix);

		//Debug information
		static bool bPressed2;
		if (glfwGetKey(m_window,GLFW_KEY_F12)){
			if (!bPressed2){
				bPressed2 = true;
				bDebugOpen = !bDebugOpen;
			}
		}else{
			if (bPressed2) bPressed2 = false;
		}
		if (bDebugOpen){
			Shader::Unbind();
			l_LightClass->Draw();
			Gizmos::draw(viewMatrix, m_projectionMatrix);
			bfArial->Clear();
			bfArial->AddFloat(1/Utility::getDeltaTime(),0,0,1,false);
			bfArial->AddString(((bCameraType) ? "Grav" : "Free"),0,16,1,false);
			bfArial->AddFloat(m_cameraMatrix[3].x,0,32,1,false);
			bfArial->AddFloat(m_cameraMatrix[3].y,64,32,1,false);
			bfArial->AddFloat(m_cameraMatrix[3].z,128,32,1,false);
			bfArial->Update();
			bfArial->Draw(FontShader);
		}
	}else{
		bfArial->Clear();
		bfArial->SizeSet(64);
		bfArial->AddString("Loading...",DEFAULT_SCREENWIDTH/2,DEFAULT_SCREENHEIGHT/2,1,true);
		bfArial->Update();
		bfArial->SizeSet(16);
		bfArial->Draw(FontShader);
	}
}

void Assignment_01::onDestroy() {
	// clean up anything we created
	Gizmos::destroy();
}

void Assignment_01::GenerateMap(bool LoadTex){
	printf("Setting up walls\n");
	int SideCount = 13;
	int Count = SideCount * 4;
	glm::mat4 *MatrixArray = new glm::mat4[Count];
	
	for (int i = 0; i < SideCount; i++){
		MatrixArray[i]  = glm::translate(glm::vec3((MODEL_SCALE * (i-(SideCount/2))*2),(MODEL_SCALE * (SideCount)),0));
		MatrixArray[SideCount + i] = glm::rotate(PI/2,glm::vec3(0,0,1)) * MatrixArray[i];
		MatrixArray[(SideCount * 2) + i] = glm::rotate(PI,glm::vec3(0,0,1)) * MatrixArray[i];
		MatrixArray[(SideCount * 3) + i] = glm::rotate(PI*1.5f,glm::vec3(0,0,1)) * MatrixArray[i];
	}
	
	mpWall = new ModelPack(Count,new FBXModel("Models/Assignment/poorlords_wall.FBX",FBXFile::UNITS_METER,LoadTex),MatrixArray);
	mpWall->fbxModel->Update();//we only need to do this once, to help setup the matricies for the model;
	
	//Count = 4;
	//
	//MatrixArray = new glm::mat4[Count];
	//MatrixArray[0] = glm::translate(glm::vec3(-MODEL_SCALE/4,MODEL_SCALE,0));
	//MatrixArray[1] = glm::rotate(PI/2,glm::vec3(0,0,1)) * MatrixArray[0];
	//MatrixArray[2] = glm::rotate(PI,glm::vec3(0,0,1)) * MatrixArray[0];
	//MatrixArray[3] = glm::rotate(PI*1.5f,glm::vec3(0,0,1)) * MatrixArray[0];
	//mpWallCorner = new ModelPack(Count,new FBXModel("Models/Assignment/poorlords_wall_corner.FBX",FBXFile::UNITS_METER),MatrixArray);
	//mpWallCorner->fbxModel->Update();//we only need to do this once, to help setup the matricies for the model;
	
	Count = 1;
	printf("Setting up towers\n");
	MatrixArray = new glm::mat4[Count];
	MatrixArray[0] = glm::translate(glm::vec3(0,0,MODEL_SCALE/2.2));
	mpTowerBottom = new ModelPack(Count,new FBXModel("Models/Assignment/poorlords_tower_bottom.FBX",FBXFile::UNITS_METER,LoadTex),MatrixArray);
	mpTowerBottom->fbxModel->Update();//we only need to do this once, to help setup the matricies for the model;
	
	Count = 3;
	MatrixArray = new glm::mat4[Count];
	MatrixArray[0] = glm::translate(glm::vec3(0,0,MODEL_SCALE/2.2 + MODEL_SCALE));
	MatrixArray[1] = glm::translate(glm::vec3(0,0,MODEL_SCALE/2.2 + MODEL_SCALE*2));
	MatrixArray[2] = glm::translate(glm::vec3(0,0,MODEL_SCALE/2.2 + MODEL_SCALE*3));
	mpTowerMiddle = new ModelPack(Count,new FBXModel("Models/Assignment/poorlords_tower_middle.FBX",FBXFile::UNITS_METER,LoadTex),MatrixArray);
	mpTowerMiddle->fbxModel->Update();//we only need to do this once, to help setup the matricies for the model;
	
	Count = 1;
	MatrixArray = new glm::mat4[Count];
	MatrixArray[0] = glm::translate(glm::vec3(0,0,MODEL_SCALE/2.1 + MODEL_SCALE*4));
	mpTowerTop = new ModelPack(Count,new FBXModel("Models/Assignment/poorlords_tower_top.FBX",FBXFile::UNITS_METER,LoadTex),MatrixArray);
	mpTowerTop->fbxModel->Update();//we only need to do this once, to help setup the matricies for the model;
	
	printf("Setting up taverns\n");
	Count = 2;
	MatrixArray = new glm::mat4[Count];
	MatrixArray[0] = glm::translate(glm::vec3(MODEL_SCALE * -1.1,MODEL_SCALE * -8,MODEL_SCALE/2.2)) * glm::rotate(PI/2,glm::vec3(0,0,1));
	MatrixArray[1] = glm::translate(glm::vec3(MODEL_SCALE * -1.1,MODEL_SCALE * -6,MODEL_SCALE/2.2)) * glm::rotate(PI/2,glm::vec3(0,0,1));
	
	mpTavern = new ModelPack(Count,new FBXModel("Models/Assignment/poorfolks_tavern.FBX",FBXFile::UNITS_METER,LoadTex),MatrixArray);
	mpTavern->fbxModel->Update();//we only need to do this once, to help setup the matricies for the model;
	
	printf("Setting up stables\n");
	Count = 4;
	MatrixArray = new glm::mat4[Count];
	MatrixArray[0] = glm::translate(glm::vec3(MODEL_SCALE * 6,MODEL_SCALE * 5,MODEL_SCALE/2.65)) * glm::rotate(PI  ,glm::vec3(0,0,1));
	MatrixArray[1] = glm::translate(glm::vec3(MODEL_SCALE * 9,MODEL_SCALE * 5,MODEL_SCALE/2.65)) * glm::rotate(PI  ,glm::vec3(0,0,1));
	MatrixArray[2] = glm::translate(glm::vec3(MODEL_SCALE * 6,MODEL_SCALE * 7,MODEL_SCALE/2.65)) ;
	MatrixArray[3] = glm::translate(glm::vec3(MODEL_SCALE * 9,MODEL_SCALE * 7,MODEL_SCALE/2.65));
	
	mpStables = new ModelPack(Count,new FBXModel("Models/Assignment/poorfolks_stables.FBX",FBXFile::UNITS_METER,LoadTex),MatrixArray);
	mpStables->fbxModel->Update();//we only need to do this once, to help setup the matricies for the model;
	
	printf("Setting up wall sheds\n");
	Count = 4;
	MatrixArray = new glm::mat4[Count];
	MatrixArray[0] = glm::rotate(PI     ,glm::vec3(0,0,1)) * glm::translate(glm::vec3(MODEL_SCALE * SideCount - (MODEL_SCALE*0.25f),MODEL_SCALE * 6,0));
	MatrixArray[1] = glm::rotate(PI*0.5f,glm::vec3(0,0,1)) * glm::translate(glm::vec3(MODEL_SCALE * SideCount - (MODEL_SCALE*0.25f),MODEL_SCALE * 2,0));
	MatrixArray[2] = glm::rotate(PI*1.5f,glm::vec3(0,0,1)) * glm::translate(glm::vec3(MODEL_SCALE * SideCount - (MODEL_SCALE*0.25f),MODEL_SCALE * 5,0));
	MatrixArray[3] =										 glm::translate(glm::vec3(MODEL_SCALE * SideCount - (MODEL_SCALE*0.25f),MODEL_SCALE * 4,0));
	
	mpShed = new ModelPack(Count,new FBXModel("Models/Assignment/poorlords_shed.FBX",FBXFile::UNITS_METER,LoadTex),MatrixArray);
	mpShed->fbxModel->Update();//we only need to do this once, to help setup the matricies for the model;
	
	Count = 3;
	MatrixArray = new glm::mat4[Count];
	MatrixArray[0] = glm::rotate(PI     ,glm::vec3(0,0,1)) * glm::translate(glm::vec3(MODEL_SCALE * SideCount - (MODEL_SCALE*1.4f),MODEL_SCALE * 2,MODEL_SCALE/4));
	MatrixArray[1] = glm::rotate(PI*0.5f,glm::vec3(0,0,1)) * glm::translate(glm::vec3(MODEL_SCALE * SideCount - (MODEL_SCALE*1.4f),MODEL_SCALE * 5,MODEL_SCALE/4));
	MatrixArray[2] = glm::rotate(PI*1.5f,glm::vec3(0,0,1)) * glm::translate(glm::vec3(MODEL_SCALE * SideCount - (MODEL_SCALE*1.4f),MODEL_SCALE * 1,MODEL_SCALE/4));
	
	mpShed2 = new ModelPack(Count,new FBXModel("Models/Assignment/poorlords_shed2.FBX",FBXFile::UNITS_METER,LoadTex),MatrixArray);
	mpShed2->fbxModel->Update();//we only need to do this once, to help setup the matricies for the model;
	
	
	Count = 8;
	MatrixArray = new glm::mat4[Count];
	MatrixArray[0] = glm::rotate(PI     ,glm::vec3(0,0,1)) * glm::translate(glm::vec3(MODEL_SCALE * -4,MODEL_SCALE * -SideCount + (MODEL_SCALE*0.25f),0));
	MatrixArray[1] = glm::rotate(PI*0.5f,glm::vec3(0,0,1)) * glm::translate(glm::vec3(MODEL_SCALE * -2,MODEL_SCALE * -SideCount + (MODEL_SCALE*0.25f),0));
	MatrixArray[2] = glm::rotate(PI*1.5f,glm::vec3(0,0,1)) * glm::translate(glm::vec3(MODEL_SCALE *  8,MODEL_SCALE * -SideCount + (MODEL_SCALE*0.25f),0));
	MatrixArray[3] =										 glm::translate(glm::vec3(MODEL_SCALE * -6,MODEL_SCALE * -SideCount + (MODEL_SCALE*0.25f),0));
	MatrixArray[4] = glm::rotate(PI     ,glm::vec3(0,0,1)) * glm::translate(glm::vec3(MODEL_SCALE * -3,MODEL_SCALE * -SideCount + (MODEL_SCALE*0.25f),0));
	MatrixArray[5] = glm::rotate(PI*0.5f,glm::vec3(0,0,1)) * glm::translate(glm::vec3(MODEL_SCALE *  1,MODEL_SCALE * -SideCount + (MODEL_SCALE*0.25f),0));
	MatrixArray[6] = glm::rotate(PI*1.5f,glm::vec3(0,0,1)) * glm::translate(glm::vec3(MODEL_SCALE * -4,MODEL_SCALE * -SideCount + (MODEL_SCALE*0.25f),0));
	MatrixArray[7] =										 glm::translate(glm::vec3(MODEL_SCALE *  2,MODEL_SCALE * -SideCount + (MODEL_SCALE*0.25f),0));
	mpShed3 = new ModelPack(Count,new FBXModel("Models/Assignment/poorlords_shed3.FBX",FBXFile::UNITS_METER,LoadTex),MatrixArray);
	mpShed3->fbxModel->Update();//we only need to do this once, to help setup the matricies for the model;
	
	printf("Setting up Rats Nests\n");
	Count = 4;
	MatrixArray = new glm::mat4[Count];
	MatrixArray[0] = glm::translate(glm::vec3(MODEL_SCALE *  5,MODEL_SCALE *  -5,MODEL_SCALE*1.4f));
	MatrixArray[1] = glm::translate(glm::vec3(MODEL_SCALE * 10,MODEL_SCALE *  -5,MODEL_SCALE*1.4f)) * glm::rotate(PI*0.5f,glm::vec3(0,0,1));
	MatrixArray[2] = glm::translate(glm::vec3(MODEL_SCALE * 10,MODEL_SCALE * -10,MODEL_SCALE*1.4f)) * glm::rotate(PI*1.0f,glm::vec3(0,0,1));
	MatrixArray[3] = glm::translate(glm::vec3(MODEL_SCALE *  5,MODEL_SCALE * -10,MODEL_SCALE*1.4f)) * glm::rotate(PI*1.5f,glm::vec3(0,0,1));
	
	mpRatsNest = new ModelPack(Count,new FBXModel("Models/Assignment/rats_nest.FBX",FBXFile::UNITS_METER,LoadTex),MatrixArray);
	mpRatsNest->fbxModel->Update();//we only need to do this once, to help setup the matricies for the model;
	
	printf("Setting up Long House\n");
	Count = 1;
	MatrixArray = new glm::mat4[Count];
	MatrixArray[0] = glm::translate(glm::vec3(MODEL_SCALE * -7.75f,MODEL_SCALE * -7,0));
	mpLongHouse = new ModelPack(Count,new FBXModel("Models/Assignment/bloodraven_longhouse.FBX",FBXFile::UNITS_METER,LoadTex),MatrixArray);
	mpLongHouse->fbxModel->Update();//we only need to do this once, to help setup the matricies for the model;
	
	printf("Setting up Keep\n");
	Count = 1;
	MatrixArray = new glm::mat4[Count];
	MatrixArray[0] = glm::translate(glm::vec3(MODEL_SCALE * 7.5f,MODEL_SCALE * -7.5f,0));
	mpKeep = new ModelPack(Count,new FBXModel("Models/Assignment/poorlords_keep.FBX",FBXFile::UNITS_METER,LoadTex),MatrixArray);
	mpKeep->fbxModel->Update();//we only need to do this once, to help setup the matricies for the model;
	
	printf("Setting up Barns\n");
	Count = 2;
	MatrixArray = new glm::mat4[Count];
	MatrixArray[0] = glm::translate(glm::vec3(MODEL_SCALE * -4.0f,MODEL_SCALE * 4,0)) * glm::rotate(PI/2,glm::vec3(0,0,1));
	MatrixArray[1] = glm::translate(glm::vec3(MODEL_SCALE * -8.0f,MODEL_SCALE * 4,0)) * glm::rotate(PI/2,glm::vec3(0,0,1));
	mpBarn = new ModelPack(Count,new FBXModel("Models/Assignment/poorfolks_barn.FBX",FBXFile::UNITS_METER,LoadTex),MatrixArray);
	mpBarn->fbxModel->Update();//we only need to do this once, to help setup the matricies for the model;
	
	printf("Setting up Cottages\n");
	Count = 4;
	MatrixArray = new glm::mat4[Count];
	MatrixArray[0] = glm::translate(glm::vec3(MODEL_SCALE * 9,MODEL_SCALE * 0.45f,MODEL_SCALE * 9)) * glm::rotate(-PI/2,glm::vec3(0,1,0));
	MatrixArray[2] = glm::translate(glm::vec3(MODEL_SCALE * 9,MODEL_SCALE * 0.45f,MODEL_SCALE * 7)) * glm::rotate(-PI/2,glm::vec3(0,1,0));
	MatrixArray[1] = glm::translate(glm::vec3(MODEL_SCALE * 7,MODEL_SCALE * 0.45f,MODEL_SCALE * 9)) * glm::rotate( PI/2,glm::vec3(0,1,0));
	MatrixArray[3] = glm::translate(glm::vec3(MODEL_SCALE * 7,MODEL_SCALE * 0.45f,MODEL_SCALE * 7)) * glm::rotate( PI/2,glm::vec3(0,1,0));
	mpCottage = new ModelPack(Count,new FBXModel("Models/Assignment/poorfolks_cottage.FBX",FBXFile::UNITS_METER,LoadTex),MatrixArray);
	mpCottage->fbxModel->Update();//we only need to do this once, to help setup the matricies for the model;
	
	printf("Setting up Cottage2s\n");
	Count = 4;
	MatrixArray = new glm::mat4[Count];
	MatrixArray[0] = glm::translate(glm::vec3(MODEL_SCALE * -5,MODEL_SCALE * -9,0)) * glm::rotate( PI/2,glm::vec3(0,0,1));
	MatrixArray[2] = glm::translate(glm::vec3(MODEL_SCALE * -5,MODEL_SCALE * -7,0)) * glm::rotate( PI/2,glm::vec3(0,0,1));
	MatrixArray[1] = glm::translate(glm::vec3(MODEL_SCALE * -3,MODEL_SCALE * -9,0)) * glm::rotate(-PI/2,glm::vec3(0,0,1));
	MatrixArray[3] = glm::translate(glm::vec3(MODEL_SCALE * -3,MODEL_SCALE * -7,0)) * glm::rotate(-PI/2,glm::vec3(0,0,1));
	mpCottage2 = new ModelPack(Count,new FBXModel("Models/Assignment/poorfolks_cottage2.FBX",FBXFile::UNITS_METER,LoadTex),MatrixArray);
	mpCottage2->fbxModel->Update();//we only need to do this once, to help setup the matricies for the model;
		
	Count = 50 + (rand()%25);
	MatrixArray = new glm::mat4[Count];
	for (int i = 0; i < Count; i++){
		MatrixArray[i] = glm::translate(glm::vec3(((rand()%150)-75)*100,80,((rand()%150)-75)*100));
	}
	mpButterFlys = new ModelPack(Count,new FBXModel("Models/Assignment/Animated/Butterfly.FBX",FBXFile::UNITS_METER,LoadTex),MatrixArray);
	mpButterFlys->fbxModel->Update();//we only need to do this once, to help setup the matricies for the model;
	mpButterFlys->fbxModel->HasAnimation = true;
}

void Assignment_01::DrawMap(){
	LightShader->SetUniform("ItemID","1i",1,0);
	for (int i = 0; i < mpWall->iCount; i++){
		mpWall->fbxModel->Model(mpWall->m4Model[i]);
		mpWall->fbxModel->Draw(LightShader,viewMatrix);
	}
	//for (int i = 0; i < mpWallCorner->iCount; i++){
	//	mpWallCorner->fbxModel->Model(mpWallCorner->m4Model[i]);
	//	mpWallCorner->fbxModel->Draw(LightShader,viewMatrix);
	//}
	for (int i = 0; i < mpTowerBottom->iCount; i++){
		mpTowerBottom->fbxModel->Model(mpTowerBottom->m4Model[i]);
		mpTowerBottom->fbxModel->Draw(LightShader,viewMatrix);
	}
	for (int i = 0; i < mpTowerMiddle->iCount; i++){
		mpTowerMiddle->fbxModel->Model(mpTowerMiddle->m4Model[i]);
		mpTowerMiddle->fbxModel->Draw(LightShader,viewMatrix);
	}
	for (int i = 0; i < mpTowerTop->iCount; i++){
		mpTowerTop->fbxModel->Model(mpTowerTop->m4Model[i]);
		mpTowerTop->fbxModel->Draw(LightShader,viewMatrix);
	}
	for (int i = 0; i < mpTavern->iCount; i++){
		mpTavern->fbxModel->Model(mpTavern->m4Model[i]);
		mpTavern->fbxModel->Draw(LightShader,viewMatrix);
	}
	for (int i = 0; i < mpStables->iCount; i++){
		mpStables->fbxModel->Model(mpStables->m4Model[i]);
		mpStables->fbxModel->Draw(LightShader,viewMatrix);
	}
	for (int i = 0; i < mpShed->iCount; i++){
		mpShed->fbxModel->Model(mpShed->m4Model[i]);
		mpShed->fbxModel->Draw(LightShader,viewMatrix);
	}
	for (int i = 0; i < mpShed2->iCount; i++){
		mpShed2->fbxModel->Model(mpShed2->m4Model[i]);
		mpShed2->fbxModel->Draw(LightShader,viewMatrix);
	}
	for (int i = 0; i < mpShed3->iCount; i++){
		mpShed3->fbxModel->Model(mpShed3->m4Model[i]);
		mpShed3->fbxModel->Draw(LightShader,viewMatrix);
	}
	for (int i = 0; i < mpRatsNest->iCount; i++){
		mpRatsNest->fbxModel->Model(mpRatsNest->m4Model[i]);
		mpRatsNest->fbxModel->Draw(LightShader,viewMatrix);
	}
	for (int i = 0; i < mpLongHouse->iCount; i++){
		mpLongHouse->fbxModel->Model(mpLongHouse->m4Model[i]);
		mpLongHouse->fbxModel->Draw(LightShader,viewMatrix);
	}
	for (int i = 0; i < mpKeep->iCount; i++){
		mpKeep->fbxModel->Model(mpKeep->m4Model[i]);
		mpKeep->fbxModel->Draw(LightShader,viewMatrix);
	}
	for (int i = 0; i < mpBarn->iCount; i++){
		mpBarn->fbxModel->Model(mpBarn->m4Model[i]);
		mpBarn->fbxModel->Draw(LightShader,viewMatrix);
	}
	for (int i = 0; i < mpCottage->iCount; i++){
		mpCottage->fbxModel->Model(mpCottage->m4Model[i]);
		mpCottage->fbxModel->Draw(LightShader,viewMatrix);
	}
	for (int i = 0; i < mpCottage2->iCount; i++){
		mpCottage2->fbxModel->Model(mpCottage2->m4Model[i]);
		mpCottage2->fbxModel->Draw(LightShader,viewMatrix);
	}

	LightShader->SetUniform("iAnimated","1i",1,1);
	for (int i = 0; i < mpButterFlys->iCount; i++){
		mpButterFlys->m4Model[i][3].xz -= (mpButterFlys->m4Model[i][2]).xz * 50.0f * Utility::getDeltaTime();//move forward
		mpButterFlys->m4Model[i][3].xz += (mpButterFlys->m4Model[i][0]).xz * ((rand()%16-8)*1.0f) * Utility::getDeltaTime();//strafe
		mpButterFlys->m4Model[i][3].y += glm::sin(Utility::getTotalTime())*0.1f;//heightbobble
		mpButterFlys->m4Model[i] = glm::rotate(mpButterFlys->m4Model[i],(((rand()%19)-9) * 0.0174532925f),glm::vec3(0,1,0)); 
		mpButterFlys->fbxModel->Model(mpButterFlys->m4Model[i]);
		mpButterFlys->fbxModel->Draw(LightShader,viewMatrix);
	}
	LightShader->SetUniform("iAnimated","1i",1,0);

}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[]) {
	// explicitly control the creation of our application
	Application* app = new Assignment_01();
	
	if (app->create("AIE - Assignment_01",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	// explicitly control the destruction of our application
	delete app;

	return 0;
}

