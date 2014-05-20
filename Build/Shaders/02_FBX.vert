#version 330   
 
in vec4 Position;
in vec2 TexCoord1;

out vec2 VS_TexCoord1;

uniform mat4 Model; 
uniform mat4 View;
uniform mat4 Projection;

void main() {
    VS_TexCoord1 = TexCoord1;
	gl_Position  = Projection * View * Model * Position; 
}