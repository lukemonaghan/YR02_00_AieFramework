#version 330   
 
in vec4 Position; 	//HudItems Position
in vec2 TexCoord1;	//TexCoord for the pixel

out vec2 VS_TexCoord1;

void main() {
	VS_TexCoord1 = TexCoord1;
	gl_Position = Position; 
}