#version 150

in vec4 position;
in vec4 colour;
in vec2 textureCoordinate;

out vec2 texCoord;
out vec4 vColour;
out vec4 Position;

uniform mat4 projection;
uniform mat4 view;
uniform float time;
uniform float height;

void main() { 
	float speed = 0.098f;
	float magnitude = (sin((time * 3.14159265f / ((528.0f) * speed))) * 0.01f + 0.40f) * height;
	vec4 P = position;
	P.x -= cos(( time * 3.1415927f / (15.0f * speed)) + (position.z + position.y) * (3.1415927f*2.0f/5.0f)) * magnitude;
	P.y -= sin(( time * 3.1415927f / (15.0f * speed)) + (position.z + position.x) * (3.1415927f*2.0f/5.0f)) * magnitude;
	//P.z += sin(( time * 3.1415927f / (15.0f * speed)) + (position.y + position.x) * (3.1415927f*2.0f/5.0f)) * magnitude;
	//Passing to Fragment
	Position = P;
	vColour = colour * vec4(0.0f,0.0f,0.3f,0.8f);
	texCoord = textureCoordinate;
	//OpenGL stuff
	gl_Position = projection * view * Position;
}