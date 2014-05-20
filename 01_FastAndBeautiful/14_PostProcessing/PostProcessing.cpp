#include "PostProcessing.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

bool PostProcessing::onCreate(int a_argc, char* a_argv[]) {
	ShaderType = 0;
	MaxTypes = 20;
	count = 5;

	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, Utility::Width/(float)Utility::Height, 0.1f, 50.0f);

	glClearColor(0.25f,0.25f,0.25f,0.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	m_spear = new FBXModel("models/tanks/artillery_tank.fbx",FBXFile::UNITS_METER);
	m_spear->Update();

	Matricies = new glm::mat4[count * count];
	int modelscale = 100;
	for (int i = 0; i < count; i++){
		Matricies[(i * count) + 0] = glm::translate(glm::vec3(i * 5 * modelscale,0, 10 * modelscale));
		Matricies[(i * count) + 1] = glm::translate(glm::vec3(i * 5 * modelscale,0,  5 * modelscale));
		Matricies[(i * count) + 2] = glm::translate(glm::vec3(i * 5 * modelscale,0,  0 * modelscale));
		Matricies[(i * count) + 3] = glm::translate(glm::vec3(i * 5 * modelscale,0, -5 * modelscale));
		Matricies[(i * count) + 4] = glm::translate(glm::vec3(i * 5 * modelscale,0,-10 * modelscale));
	}

	m_Shader = new Shader("shaders/14_PostProcessing.vert","shaders/14_PostProcessing.frag");
	m_Shader->SetAttribs(4,0,"Position",1,"Color",2,"Normal",7,"TexCoord1");
	m_Shader->SetUniform("Projection","m4fv",1,false,glm::value_ptr(m_projectionMatrix));
	m_ShaderScreen = new Shader("shaders/14_PostProcessingScreen.vert","shaders/14_PostProcessingScreen.frag");
	m_ShaderScreen->SetAttribs(1,0,"Position");
	m_ShaderScreen->SetUniform("filterMode","1i",1,ShaderType);
	m_ShaderScreen->SetUniform("Width","1i",1,Utility::Width);
	m_ShaderScreen->SetUniform("Height","1i",1,Utility::Height);

	fbPostProcessing = new FrameBuffer(1,Utility::Width,Utility::Height);
	htScreen = new HUDTexture(glm::vec2(Utility::Width/2,Utility::Height/2),glm::vec2(Utility::Width,Utility::Height));

	return true;
}

void PostProcessing::onUpdate(float a_deltaTime) {
	// update our camera matrix using the keyboard/mouse
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );

	static bool bPressed;
	if (glfwGetKey(m_window,GLFW_KEY_F1)){
		if (!bPressed){
			bPressed = true;
			system("cls");//WINDOWS ONLYYYYYY!!!
			m_Shader->ReloadShader();
			m_Shader->SetAttribs(4,0,"Position",1,"Color",2,"Normal",7,"TexCoord1");
			m_Shader->SetUniform("Projection","m4fv",1,false,glm::value_ptr(m_projectionMatrix));
			m_ShaderScreen->ReloadShader();
			m_ShaderScreen->SetAttribs(1,0,"Position");
			m_ShaderScreen->SetUniform("filterMode","1i",1,ShaderType);
			m_ShaderScreen->SetUniform("Width","1i",1,Utility::Width);
			m_ShaderScreen->SetUniform("Height","1i",1,Utility::Height);
		}
	}else if (bPressed){
		bPressed = false;
	}

	static bool bPressed2;
	if (glfwGetKey(m_window,GLFW_KEY_F2)){
		if (!bPressed2){
			bPressed2 = true;
			ShaderType++;
			if (ShaderType > MaxTypes){ShaderType = 0;}
			m_ShaderScreen->SetUniform("filterMode","1i",1,ShaderType);
			std::cout << "filterMode : " << ShaderType << std::endl;
		}
	}else if (bPressed2){
		bPressed2 = false;
	}
	static bool bPressed3;
	static int iDrawMode = 0;
	if (glfwGetKey(m_window,GLFW_KEY_F3)){
		if (!bPressed3){
			bPressed3 = true;
			iDrawMode = (iDrawMode + 1) % 2;
			m_ShaderScreen->SetUniform("DrawMode","1i",1,iDrawMode);
		}
	}else if (bPressed3){
		bPressed3 = false;
	}

	m_ShaderScreen->SetUniform("time","1f",1,Utility::getTotalTime());

	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS) { quit(); }
}

void PostProcessing::onDraw() {	
	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	m_Shader->SetUniform("View","m4fv",1,false,glm::value_ptr(viewMatrix));

    // switch to the framebuffer
	fbPostProcessing->Bind();
	Clear();
	for (int i = 0; i < count * count; i++){
		m_spear->Model(Matricies[i]);
		m_spear->Draw( m_Shader ,viewMatrix);
	}
    // switch back to the back buffer
	FrameBuffer::Unbind();
	Clear();
    // bind the post shader
	m_ShaderScreen->Bind();
	m_ShaderScreen->SetTexture("diffuseMap",0,fbPostProcessing->GetTexture(0));
	m_ShaderScreen->SetTexture("depthMap",1,fbPostProcessing->GetDepthTexture());
	htScreen->Draw();
	if (glfwGetKey(m_window,GLFW_KEY_F4) == GLFW_PRESS) { 
		m_ShaderScreen->SetUniform("DrawMode","1i",1,1);
		m_ShaderScreen->SetUniform("filterMode","1i",1,0);
		htScreen->Draw();
		m_ShaderScreen->SetUniform("filterMode","1i",1,ShaderType);
	}
}

void PostProcessing::onDestroy() { }

// main that controls the creation/destruction of an application
int main(int argc, char* argv[]) {
	// explicitly control the creation of our application
	Application* app = new PostProcessing();
	
	if (app->create("AIE - 14_PostProcessing",Utility::Width,Utility::Height,argc,argv) == true)
		app->run();
		
	// explicitly control the destruction of our application
	delete app;

	return 0;
}
