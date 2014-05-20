#version 330   
 
in vec4 Position;
in vec4 Color;
in vec4 Normal;
in vec2 TexCoord1;

out vec2 vUV;
out vec4 vColor;
out vec4 vNormal;

uniform mat4 Model; 
uniform mat4 View;
uniform mat4 Projection;

void main() {
	vNormal = Normal;
    vUV     = TexCoord1;
    vColor  = Color;
	gl_Position = Projection * View * Model * Position; 
}