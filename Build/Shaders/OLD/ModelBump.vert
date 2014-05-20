#version 330   
 
in vec4 Position;
in vec4 Color;
in vec2 TexCoord1;

out vec2 vUV;
out vec4 vColor;

uniform mat4 Model; 
uniform mat4 View;
uniform mat4 Projection;

void main() 
{
    vUV     = TexCoord1;
    vColor  = Color;
	//Passing to Fragment
	gl_Position = Projection * View * Model * Position; 
}