#version 330

in vec4 VS_Colour;
in vec2 VS_TexCoord1;

out vec4 outColour;

uniform sampler2D DiffuseTexture;

void main() {
	outColour = texture( DiffuseTexture, VS_TexCoord1 ) * VS_Colour;
}