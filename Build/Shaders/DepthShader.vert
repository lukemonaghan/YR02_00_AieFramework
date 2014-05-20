#version 150

in vec4 Position;

uniform mat4 lightProjectionView;
uniform mat4 world;
	
void main() {
	gl_Position = lightProjectionView * world * Position;
}