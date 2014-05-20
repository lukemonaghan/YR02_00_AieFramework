#ifndef __Assignment_01_H_
#define __Assignment_01_H_

#include "Application.h"
#include <glm/ext.hpp>

#include "Shader.h"
#include "FBXModel.h"
#include "Lights.h"
#include <Skybox.h>
#include <Plane.h>
#include <HUDItems.h>
#include "GPUParticle.h"
#include <CollisionMap.h>

#define MODEL_SCALE 512
#define PI 3.14159f

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

// Derived application class that wraps up all globals neatly
class Assignment_01 : public Application {
public:

	struct ModelPack{
		ModelPack(){};
		ModelPack(int ic,FBXModel *FBXm,glm::mat4 *Mats){
			iCount = ic;
			fbxModel = FBXm;
			m4Model = Mats;
		}
		int iCount;
		FBXModel *fbxModel;
		glm::mat4 *m4Model;
	};

	Assignment_01();
	virtual ~Assignment_01();
	void GenerateMap(bool LoadTex);
	void DrawMap();
	void CheckFirstRun();
protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix,m_projectionMatrixHud2D;
	glm::mat4	viewMatrix;

	Lights	*l_LightClass;
	Plane	*pFloor;
	Texture *tGrassTexture,*tDirtTexture,*tPathMap,*tCloudFog,*tFire,*tRain;

	ModelPack *mpBarn,*mpCottage,*mpCottage2;
	ModelPack *mpKeep;
	ModelPack *mpLongHouse;
	ModelPack *mpRatsNest;
	ModelPack *mpShed,*mpShed2,*mpShed3;
	ModelPack *mpStables;
	ModelPack *mpTavern;
	ModelPack *mpTowerBottom,*mpTowerMiddle,*mpTowerTop;
	ModelPack *mpWall,*mpWallCorner;
	ModelPack *mpButterFlys;

	Shader *LightShader,*CubeShader,*FontShader,*TessShader;

	Skybox *m_Skybox;
	CubeMap *m_CubeMap;
	BitFont *bfArial;

	GPUParticle *pFire,*pSmoke,*pRain;

	int IDs[2];
	float fFlickerTime;

	bool bCameraType,bDebugOpen;
	bool bFirstRun,bCheck; 
};

#endif // __Assignment_01_H_