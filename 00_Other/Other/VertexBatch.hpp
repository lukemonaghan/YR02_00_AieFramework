#pragma once

#include <glm/ext.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <map>

namespace Osiris{
	//4 4 4 2
	struct PCNUV{

		glm::vec4 position;
		glm::vec4 colour;
		//glm::vec4 normal;
		//glm::vec2 uv;

		enum Offsets{
			PositionOffset	= 0,
			ColourOffset	= PositionOffset	+ sizeof(glm::vec4),
			//NormalOffset	= ColourOffset		+ sizeof(glm::vec4),
			//UVOffset		= NormalOffset		+ sizeof(glm::vec4)
		};
	};

	// Base GizmoObject, Inherit from this to create your own
	struct GizmoObject{
		~GizmoObject(){
			delete[] Indicies;
			delete[] Verticies;
		}
		unsigned int IndexCount,VertexCount;
		unsigned int *Indicies;
		PCNUV *Verticies;
	};

	// VertexBatch
	// Basic Single draw call batch of elements.
	class VertexBatch{
	public:
		// Instantiate the Gizmos.
		// Must setup all VertexAtribArrays
		VertexBatch(){

// create shaders
	const char* vsSource = "#version 150\n \
					 in vec4 Position; \
					 in vec4 Colour; \
					 out vec4 vColour; \
					 uniform mat4 ProjectionView; \
					 void main() { vColour = Colour; gl_Position = ProjectionView * Position; }";

	const char* fsSource = "#version 150\n \
					 in vec4 vColour; \
                     out vec4 FragColor; \
					 void main()	{ FragColor = vColour; }";
    
	int success = GL_FALSE;
    
	_vertexShader = glCreateShader(GL_VERTEX_SHADER);
	_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(_vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(_vertexShader);
    
	glGetShaderiv(_vertexShader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetShaderiv(_vertexShader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
        
		glGetShaderInfoLog(_vertexShader, infoLogLength, 0, infoLog);
		printf("Error: Failed to compile vertex shader!\n");
		printf("%s",infoLog);
		printf("\n");
		delete[] infoLog;
	}

	glShaderSource(_fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(_fragmentShader);
    
	glGetShaderiv(_fragmentShader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetShaderiv(_fragmentShader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
        
		glGetShaderInfoLog(_fragmentShader, infoLogLength, 0, infoLog);
		printf("Error: Failed to compile fragment shader!\n");
		printf("%s",infoLog);
		printf("\n");
		delete[] infoLog;
	}
    
	_sGizmoShader = glCreateProgram();
	glAttachShader(_sGizmoShader, _vertexShader);
	glAttachShader(_sGizmoShader, _fragmentShader);
	glBindAttribLocation(_sGizmoShader, 0, "Position");
	glBindAttribLocation(_sGizmoShader, 1, "Colour");
	glLinkProgram(_sGizmoShader);
    
    glGetProgramiv(_sGizmoShader, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetShaderiv(_sGizmoShader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
        
		glGetShaderInfoLog(_sGizmoShader, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s",infoLog);
		printf("\n");
		delete[] infoLog;
	}

			_NextObjectID = _VertexCount = _IndexCount = 0;
			_Indices = nullptr;

			// Create opengl buffers
			glGenVertexArrays(1, &_VAO);
			glBindVertexArray(_VAO);

			glGenBuffers(1, &_VBO);
			glGenBuffers(1, &_IBO);

			glBindBuffer(GL_ARRAY_BUFFER, _VBO);
			glBufferData(GL_ARRAY_BUFFER, 1048576 * sizeof(PCNUV), nullptr, GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _IBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, 1048576 * sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(PCNUV), (void*) + PCNUV::PositionOffset);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(PCNUV), (void*) + PCNUV::ColourOffset);
			//glEnableVertexAttribArray(2);
			//glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE , sizeof(PCNUV), (void*) + PCNUV::NormalOffset);
			//glEnableVertexAttribArray(3);
			//glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(PCNUV), (void*) + PCNUV::UVOffset);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}

		// Destroy the Gizmos.
		// !WARNING! does NOT clean up the given shader, be sure to delete it.
		// !WARNING! DOES clean up all given GizmoObjects.
		~VertexBatch(){
			glDeleteVertexArrays(1,&_VAO);
			glDeleteBuffers(1, &_VBO);
			glDeleteBuffers(1, &_IBO);
			glDeleteProgram(_sGizmoShader);
			glDeleteShader(_fragmentShader);
			glDeleteShader(_vertexShader);
			for (auto gizmo : _GizmoObjects){
				delete gizmo.second;
			}
			_GizmoObjects.clear();
		}

		// Create the given Gizmo.
		// go_ObjectToAdd a pointer to a GizmoObject.
		// return is the ID for this GizmoObject.
		unsigned int Add(GizmoObject *go_ObjectToAdd){
			_GizmoObjects[_NextObjectID] = go_ObjectToAdd;
			_VertexCount	+= _GizmoObjects[_NextObjectID]->VertexCount;
			_IndexCount		+= _GizmoObjects[_NextObjectID]->IndexCount;
			_NextObjectID++;
			return _NextObjectID - 1;
		}

		// Delete the given Gizmo.
		// ui_GizmoID = The ID of the gizmo to remove.
		// !WARNING! This calls a delete on the GizmoObject.
		// !WARNING! This batch will draw weird until Update() is called.
		void Remove(unsigned int ui_GizmoID){
			if (_GizmoObjects.find(ui_GizmoID) != _GizmoObjects.end()){
				_VertexCount	-= _GizmoObjects[ui_GizmoID]->VertexCount;
				_IndexCount		-= _GizmoObjects[ui_GizmoID]->IndexCount;
				delete _GizmoObjects[ui_GizmoID];
				_GizmoObjects.erase(ui_GizmoID);
			}
			if (_GizmoObjects.size() == 0){_NextObjectID = 0;}
		}

		// Get a Gizmo from the batch.
		// ui_GizmoID = The ID of the gizmo to return.
		// GizmoType  = The Type of the gizmo to return;
		// !WARNING! any changes will need to be updated.
		template <typename GizmoType>
		GizmoType* Get(unsigned int ui_GizmoID){
			if (_GizmoObjects.find(ui_GizmoID) != _GizmoObjects.end()){
				return (GizmoType*)_GizmoObjects[ui_GizmoID];
			}
		}

		// Update the Gizmos.
		// Call ONCE after all changes have been made.
		// If this hasn't been called nothing added or removed will be updated.
		void Update(){

			_Verticies = new PCNUV[_VertexCount];
			_Indices = new unsigned int[_IndexCount];

			_VertexCount = 0;
			_IndexCount = 0;

			for (std::map<unsigned int,GizmoObject*>::iterator it = _GizmoObjects.begin(); it != _GizmoObjects.end(); it++){
				for(unsigned int i = 0; i < it->second->VertexCount; i++){
					_Verticies[_VertexCount + i] = it->second->Verticies[i];
				}

				for(unsigned int i = 0; i < it->second->IndexCount; i++){
					_Indices[_IndexCount + i] = _VertexCount + it->second->Indicies[i];
				}

				_VertexCount += it->second->VertexCount;
				_IndexCount += it->second->IndexCount;
			}

			glBindBuffer(GL_ARRAY_BUFFER, _VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, _VertexCount * sizeof(PCNUV), _Verticies);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _IBO);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, _IndexCount * sizeof(unsigned int), _Indices);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

		}

		// Draw the Gizmos with the given Shader.
		void Draw(const glm::mat4& m4_View,const glm::mat4& m4_Projection){
			if (_sGizmoShader != 0 && _IndexCount >= 0 && _VAO != 0){
				glUseProgram(_sGizmoShader);
						
				unsigned int projectionViewUniform = glGetUniformLocation(_sGizmoShader,"ProjectionView");
				glUniformMatrix4fv(projectionViewUniform, 1, false, &(m4_Projection * m4_View)[0].x);

				glBindVertexArray( _VAO );
				glDrawElements(GL_TRIANGLES, _IndexCount, GL_UNSIGNED_INT, _Indices);
				glBindVertexArray(0);
			}
		}

	protected:
		unsigned int	_VAO,
						_VBO,
						_IBO;

		unsigned int	_vertexShader,
						_fragmentShader,
						_sGizmoShader;

		unsigned int	_VertexCount,
						_IndexCount;

		unsigned int	*_Indices;
		PCNUV			*_Verticies;

		unsigned int	 _NextObjectID;
		std::map<unsigned int,GizmoObject*> _GizmoObjects;   
	};

	namespace Gizmo{

		// Gizmo Plane
		// X Verticies, Y Indicies
		struct Plane : public GizmoObject{
			Plane(glm::vec3 Location,glm::vec2 Extents,glm::vec4 Colour){

				VertexCount = 4;
				Verticies = new PCNUV[VertexCount];

				Verticies[0].position = glm::vec4(Location,0.0f) + glm::vec4(  Extents.x,  0, -Extents.y,1.0f);
				Verticies[1].position = glm::vec4(Location,0.0f) + glm::vec4(  Extents.x,  0,  Extents.y,1.0f);
				Verticies[2].position = glm::vec4(Location,0.0f) + glm::vec4( -Extents.x,  0,  Extents.y,1.0f);
				Verticies[3].position = glm::vec4(Location,0.0f) + glm::vec4( -Extents.x,  0, -Extents.y,1.0f);
				
				//Verticies[0].normal = glm::vec4( 0.5f, 1.0f,-0.5f, 1.0f);
				//Verticies[1].normal = glm::vec4( 0.5f, 1.0f, 0.5f, 1.0f);
				//Verticies[2].normal = glm::vec4(-0.5f, 1.0f, 0.5f, 1.0f);
				//Verticies[3].normal = glm::vec4(-0.5f, 1.0f,-0.5f, 1.0f);
				//
				//Verticies[0].uv = glm::vec2(1,0);
				//Verticies[1].uv = glm::vec2(1,1);
				//Verticies[2].uv = glm::vec2(0,1);
				//Verticies[3].uv = glm::vec2(0,0);
				
				Verticies[0].colour = Colour;
				Verticies[1].colour = Colour;
				Verticies[2].colour = Colour;
				Verticies[3].colour = Colour;

				IndexCount = 6;
				Indicies = new unsigned int[IndexCount];
				Indicies[0] = Indicies[3] = 0; 
				if (Extents.y >= 0){
					Indicies[1] = 2; Indicies[2] = 1;
					Indicies[4] = 3; Indicies[5] = 2;
				}else{
					Indicies[1] = 1; Indicies[2] = 2;
					Indicies[4] = 2; Indicies[5] = 3;
				}

			}
		};

		// Gizmo Point
		// 1 Vertex, 1 Index
		struct Point : public GizmoObject{
			Point(glm::vec3 Location,glm::vec4 Colour){

				VertexCount = 1;
				Verticies = new PCNUV[VertexCount];
				Verticies[0].position = glm::vec4(Location,0.0f); 
				//Verticies[0].normal = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
				//Verticies[0].uv = glm::vec2(0,0);
				Verticies[0].colour = Colour;

				IndexCount = 1;
				Indicies = new unsigned int[IndexCount];
				Indicies[0] = 0;

			}
		};

		// Gizmo Sphere
		// X Verticies, Y Indicies
		struct Sphere : public GizmoObject{
			Sphere(glm::vec3 Location,float Radius, glm::vec2 v2_SubDivisions,glm::vec4 Colour){
			
				VertexCount = 1;
				Verticies = new PCNUV[VertexCount];
				Verticies[0].position = glm::vec4(Location,0.0f); 
				//Verticies[0].normal = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
				//Verticies[0].uv = glm::vec2(0,0);
				Verticies[0].colour = Colour;

				IndexCount = 1;
				Indicies = new unsigned int[IndexCount];
				Indicies[0] = 0;
			}
		};

		// Gizmo Box 
		// 8 Verticies, 36 Incicies
		struct BoxSingleFace : public GizmoObject{
			BoxSingleFace(glm::vec3 Location,glm::vec3 Extents,glm::vec4 Colour){
				IndexCount = 36;
				VertexCount = 8;

				Verticies = new PCNUV[VertexCount];
				Verticies[0].position = glm::vec4(Location,0.0f) + glm::vec4( -Extents.x, -Extents.y, -Extents.z,1.0f);
				Verticies[1].position = glm::vec4(Location,0.0f) + glm::vec4( -Extents.x, -Extents.y,  Extents.z,1.0f); 
				Verticies[2].position = glm::vec4(Location,0.0f) + glm::vec4(  Extents.x, -Extents.y,  Extents.z,1.0f); 
				Verticies[3].position = glm::vec4(Location,0.0f) + glm::vec4(  Extents.x, -Extents.y, -Extents.z,1.0f); 
				Verticies[4].position = glm::vec4(Location,0.0f) + glm::vec4(  Extents.x,  Extents.y, -Extents.z,1.0f); 
				Verticies[5].position = glm::vec4(Location,0.0f) + glm::vec4(  Extents.x,  Extents.y,  Extents.z,1.0f); 
				Verticies[6].position = glm::vec4(Location,0.0f) + glm::vec4( -Extents.x,  Extents.y,  Extents.z,1.0f); 
				Verticies[7].position = glm::vec4(Location,0.0f) + glm::vec4( -Extents.x,  Extents.y, -Extents.z,1.0f); 

				//Verticies[0].normal = glm::vec4(-0.5f,-0.5f,-0.5f, 1.0f); 
				//Verticies[1].normal = glm::vec4(-0.5f,-0.5f, 0.5f, 1.0f); 
				//Verticies[2].normal = glm::vec4( 0.5f,-0.5f, 0.5f, 1.0f); 
				//Verticies[3].normal = glm::vec4( 0.5f,-0.5f,-0.5f, 1.0f); 
				//Verticies[4].normal = glm::vec4( 0.5f, 0.5f,-0.5f, 1.0f); 
				//Verticies[5].normal = glm::vec4( 0.5f, 0.5f, 0.5f, 1.0f); 
				//Verticies[6].normal = glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f); 
				//Verticies[7].normal = glm::vec4(-0.5f, 0.5f,-0.5f, 1.0f); 
				//
				//Verticies[0].uv = glm::vec2(1,0);
				//Verticies[1].uv = glm::vec2(1,1);
				//Verticies[2].uv = glm::vec2(0,1);
				//Verticies[3].uv = glm::vec2(0,0);																														  
				//Verticies[4].uv = glm::vec2(1,0);
				//Verticies[5].uv = glm::vec2(1,1);
				//Verticies[6].uv = glm::vec2(0,1);
				//Verticies[7].uv = glm::vec2(0,0);

				for (unsigned int i = 0; i < VertexCount; i++){
					Verticies[i].colour = Colour;
				}

				unsigned int Ind[36] = {
					0,2,1 ,0,3,2,
					2,6,1 ,2,5,6,
					3,5,2 ,3,4,5,
					7,6,5 ,7,5,4,
					0,4,3 ,0,7,4,
					1,7,0 ,1,6,7,
				};

				Indicies = new unsigned int[IndexCount];
				for (unsigned int i = 0; i < IndexCount; i++){
					Indicies[i] = Ind[i];
				}
			}
		};
	}
}

