#version 330   
 
in vec4 Position;
in vec4 Color;
in vec2 TexCoord1;

out vec2 VS_TexCoord1;
out vec4 VS_Color;

uniform mat4 Model; 
uniform mat4 View;
uniform mat4 Projection;

void main() {
    VS_TexCoord1 = TexCoord1;
	VS_Color  	 = Color;
	gl_Position  = Projection * View * Model * Position; 
}