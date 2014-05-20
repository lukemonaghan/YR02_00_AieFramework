#version 330

in vec4 position;
in vec2 texCoord;
in vec4 indices;
in vec4 weights;

out vec2 TexCoord;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;

// we need to give our bone array a limit
const int MAX_BONES = 128;
uniform mat4 bones[MAX_BONES];

void main(){	

	// cast the indices to integer's so they can index an array
	ivec4 index = ivec4(indices);
	
	// sample bones and blend up to 4
	vec4 P =  bones[ index.x ] * position * weights.x;
		 P += bones[ index.y ] * position * weights.y;
		 P += bones[ index.z ] * position * weights.z;
		 P += bones[ index.w ] * position * weights.w;
	
	TexCoord = texCoord;
	gl_Position = Projection * View * Model * P;
}