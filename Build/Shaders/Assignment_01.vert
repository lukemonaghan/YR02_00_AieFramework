#version 330   
 
layout (location = 0) in vec4 Position;
layout (location = 1) in vec4 Colour;
layout (location = 2) in vec4 Normal;
layout (location = 5) in vec4 indices;
layout (location = 6) in vec4 weights;
layout (location = 7) in vec2 UV;
layout (location = 10)in vec2 UV2;

out vec3 position;
out vec3 normal;
out vec4 colour;
out vec2 uv;
out vec2 uv2;

uniform mat4 Model = mat4(1);
uniform mat4 View = mat4(1);
uniform mat4 Projection = mat4(1);

// we need to give our bone array a limit
const int MAX_BONES = 128;
uniform mat4 bones[MAX_BONES];
uniform int iAnimated = 0;

void main() {
	vec4 P = Position;
	if (iAnimated != 0){
		ivec4 index = ivec4(indices);
		P =  bones[ index.x ] * Position * weights.x;
		P += bones[ index.y ] * Position * weights.y;
		P += bones[ index.z ] * Position * weights.z;
		P += bones[ index.w ] * Position * weights.w;
	}
	uv2 = UV2;
	uv = UV;
	colour = Colour;
	position = (Model * P).xyz;
	normal = (Model * Normal).xyz;
	gl_Position = Projection * View * Model * P;
}