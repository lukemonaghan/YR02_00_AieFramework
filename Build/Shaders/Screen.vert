#version 330   
 
in vec4 Position; 	//HudItems Position
in vec4 Colour;		//HudItems Colour
in vec2 TexCoord1;	//TexCoord for the pixel

out vec4 VS_Colour;
out vec2 VS_TexCoord1;

void main() {
	VS_Colour = Colour;
	VS_TexCoord1 = TexCoord1;
	gl_Position = Position; 
}