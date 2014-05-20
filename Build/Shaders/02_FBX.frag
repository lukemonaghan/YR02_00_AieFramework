#version 330

in vec2 VS_TexCoord1;
in vec4 VS_Color;

out vec4 outColor;

uniform sampler2D DiffuseTexture;
uniform sampler2D SpecularTexture;
uniform sampler2D NormalTexture;

void main() 
{ 
	outColor = texture2D( DiffuseTexture, VS_TexCoord1 ) * VS_Color;
	outColor.a = 1.0f;
}