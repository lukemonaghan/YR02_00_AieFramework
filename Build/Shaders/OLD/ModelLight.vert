#version 330   
 
in vec4 Position;
in vec4 Color;
in vec2 TexCoord1;

out vec2 vUV;
out vec4 vColor;
out vec4 vPosition;

uniform mat4 Model; 
uniform mat4 View;
uniform mat4 Projection;

void main() {
	//Passing to Fragment
    vUV     = TexCoord1;
    vColor  = Color;
	vPosition = Position;
	//Passing to Buffer
	gl_Position = Projection * View * Model * Position; 
}