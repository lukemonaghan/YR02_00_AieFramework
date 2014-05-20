#version 330   
 
in vec4 Position;
in vec4 Normal;
in vec4 Colour;
in vec2 TexCoord1;

out vec4 VS_Position;
out vec4 VS_Normal;
out vec4 VS_Colour;
out vec2 VS_TexCoord1;
out vec2 VS_TexCoord2;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main() {
	VS_Position = Position;
	VS_Normal = Normal;
	VS_Colour = Colour;
	VS_TexCoord1 = TexCoord1;
	
	gl_Position = Projection * View * Model * Position;
}