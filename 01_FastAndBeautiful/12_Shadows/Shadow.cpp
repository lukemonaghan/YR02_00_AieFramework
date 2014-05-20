#include "Shadow.h"
#include <SOIL.h>
#include <Utilities.h>

Shadow::Shadow(char* string,FBXFile::UNIT_SCALE a_Scale){
	m_file = new FBXFile();
	m_file->load( string,a_Scale,true,true );

	// how manu meshes and materials are stored within the fbx file
	unsigned int meshCount = m_file->getMeshCount();
	unsigned int matCount = m_file->getMaterialCount();

	// loop through each mesh
	for(int i=0; i<meshCount; ++i) {
		// get the current mesh
		FBXMeshNode *pMesh = m_file->getMeshByIndex(i);

		// genorate our OGL_FBXRenderData for storing the meshes VBO, IBO and VAO
		// and assign it to the meshes m_userData pointer so that we can retrive 
		// it again within the render function
		OGL_FBXRenderData *ro = new OGL_FBXRenderData();
		pMesh->m_userData = ro;

		// OPENGL: genorate the VBO, IBO and VAO
		glGenBuffers(1, &ro->VBO);
		glGenBuffers(1, &ro->IBO);
		glGenVertexArrays(1, &ro->VAO);

		// OPENGL: Bind  VAO, and then bind the VBO and IBO to the VAO
		glBindVertexArray(ro->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, ro->VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ro->IBO);

		// Send the vertex data to the VBO
		glBufferData(GL_ARRAY_BUFFER, pMesh->m_vertices.size() * sizeof(FBXVertex), pMesh->m_vertices.data(), GL_STATIC_DRAW);
		
		// send the index data to the IBO
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, pMesh->m_indices.size() * sizeof(unsigned int), pMesh->m_indices.data(), GL_STATIC_DRAW);

		//Here we push everything! just need to setup in the shaders

		// enable the attribute locations that will be used on our shaders
		glEnableVertexAttribArray(0); //Pos
		glEnableVertexAttribArray(2); //norm
		glEnableVertexAttribArray(7); //tex
		
		// tell our shaders where the information within our buffers lie
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char *)0) + FBXVertex::PositionOffset);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char *)0) + FBXVertex::NormalOffset);
		glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char *)0) + FBXVertex::TexCoord1Offset);

		// finally, where done describing our mesh to the shader
		// we can describe the next mesh
		glBindVertexArray(0);

		pMesh->updateGlobalTransform();
	}

	// Loop through each material and load the texture
	for(unsigned int i = 0; i<matCount; ++i) {
		// get the current material being loaded
		FBXMaterial *pMaterial = m_file->getMaterialByIndex(i);

		// each material can optionally have many different textures that can be used to describe how the object should be rendered.
		// Look at the FBXMaterial structure.
		// the following textures can be assigned to a material within the 3D modeling programs
		// we can optionally support loading these in...
		// - DiffuseTexture,
		// - AmbientTexture,
		// - GlowTexture,
		// - SpecularTexture,
		// - GlossTexture,
		// - NormalTexture,
		// - AlphaTexture,
		// - DisplacementTexture

		for(unsigned int j = 0; j<FBXMaterial::TextureTypes_Count; ++j)	{
			// find the path to the texture to be loaded
			std::string path = m_file->getPath();
			// append the filename of the texture
			path += pMaterial->textureFilenames[j];
			// load the texture using SOIL
			pMaterial->textureIDs[j] = SOIL_load_OGL_texture(path.c_str(), 4, 0, SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_INVERT_Y);
			// lets just print what is loaded to the console...
			if (pMaterial->textureIDs[j] != 0){
				printf("Loading texture : %s - ID: %i\n", path.c_str(), pMaterial->textureIDs[j]);
			}
		}
	}

	//resolution of the texture to put our shadow map in
	m_shadowSize = 1024;

	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	glGenFramebuffers(1, &m_shadowFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFramebuffer);

	glGenTextures(1,&m_shadowTexture);
	glBindTexture(GL_TEXTURE_2D,m_shadowTexture);	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT,m_shadowSize, m_shadowSize,0,GL_DEPTH_COMPONENT,GL_FLOAT,NULL);
	glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_shadowTexture, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("depth buffer not created");
	} else {
		printf("Success! created depth buffer\n"); 
	}

	// return to back-buffer rendering
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Shadow::~Shadow(void){ }

void Shadow::setUpLightAndShadowMatrix(Shader* s_DepthShader) {
	float fCone = 2.5f;
	// setup light direction and shadow matrix
	glm::vec3 lightPosition = glm::vec3(5*cos(Utility::getTotalTime()*0.5f),3,5*sin(Utility::getTotalTime()*0.5f));
	m_lightDirection = glm::normalize(glm::vec4( -lightPosition, 1));

	glm::mat4 depthViewMatrix = glm::lookAt(lightPosition, glm::vec3(0,0,0), glm::vec3(0,1,0));
	glm::mat4 depthProjectionMatrix = glm::ortho<float>(-fCone,fCone,-fCone,fCone,-1,fCone*10.0f);
	//glm::mat4 depthProjectionMatrix = glm::perspective(glm::pi<float>() * 0.5f,1.0f,2.0f,50.0f);
	m_shadowProjectionViewMatrix = depthProjectionMatrix * depthViewMatrix;

	// set to render to the back-buffer
	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFramebuffer); 
	glViewport(0, 0, m_shadowSize, m_shadowSize);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for ( unsigned int i = 0 ; i < m_file->getMeshCount() ; ++i ) {
		FBXMeshNode* mesh = m_file->getMeshByIndex(i);
		s_DepthShader->SetUniform("lightProjectionView","m4fv",1,false,glm::value_ptr(m_shadowProjectionViewMatrix));
		s_DepthShader->SetUniform("world","m4fv",1,false,glm::value_ptr(mesh->m_globalTransform));
		OGL_FBXRenderData *ro = (OGL_FBXRenderData *)mesh->m_userData;
		glBindVertexArray( ro->VAO );
		glDrawElements(GL_TRIANGLES, mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}
}

void Shadow::create2DQuad(int W,int H) {
	glGenVertexArrays(1, &m_2dQuad.vao);
	glBindVertexArray(m_2dQuad.vao);

	// create a 200x200 2D GUI quad (resize it to screen-space!)
	glm::vec2 size(200,200);
	size.x /= W;
	size.y /= H;
	size *= 2;
	
	// setup the quad in the top corner
	float quadVertices[] = {
		-1.0f		  , 1.0f - size.y, 0.0f, 1.0f, 0, 0,
		-1.0f + size.x, 1.0f - size.y, 0.0f, 1.0f, 1, 0,
		-1.0f		  , 1.0f		 , 0.0f, 1.0f, 0, 1,

		-1.0f		  , 1.0f		 , 0.0f, 1.0f, 0, 1,
		-1.0f + size.x, 1.0f - size.y, 0.0f, 1.0f, 1, 0,
		-1.0f + size.x, 1.0f		 , 0.0f, 1.0f, 1, 1,
	};

	glGenBuffers(1, &m_2dQuad.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_2dQuad.vbo);	
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 6, quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, ((char*)0)+16);
	glBindVertexArray(0);
}

void Shadow::displayShadowMap(Shader* s_Shader) {
	s_Shader->SetTexture("DiffTex",0,m_shadowTexture);
	glBindVertexArray(m_2dQuad.vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Shadow::drawScene(Shader* s_Shader,int W,int H,glm::mat4& m_cameraMatrix,glm::mat4& m_projectionMatrix) {
	glViewport(0, 0, W, H);
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	s_Shader->SetTexture("shadowMap",1,m_shadowTexture);
	s_Shader->SetUniform("lightDir","4fv",1,glm::value_ptr(m_lightDirection));
	s_Shader->SetUniform("projectionView","m4fv",1,false,glm::value_ptr(m_projectionMatrix * viewMatrix));
	for ( unsigned int i = 0 ; i < m_file->getMeshCount() ; ++i ) {
		FBXMeshNode* mesh = m_file->getMeshByIndex(i);
		s_Shader->SetUniform("lightProjectionView","m4fv",1,false,glm::value_ptr(m_shadowProjectionViewMatrix));
		s_Shader->SetUniform("world","m4fv",1,false,glm::value_ptr(mesh->m_globalTransform));
		s_Shader->SetTexture("diffuseMap",0,mesh->m_material->textureIDs[ FBXMaterial::DiffuseTexture ]);
		OGL_FBXRenderData *ro = (OGL_FBXRenderData *)mesh->m_userData;
		glBindVertexArray( ro->VAO );
		glDrawElements(GL_TRIANGLES, mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}
}