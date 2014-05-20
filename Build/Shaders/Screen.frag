#version 330

in vec4 VS_Colour;
in vec2 VS_TexCoord1;

out vec4 outColour;

uniform sampler2D DiffuseTexture;
uniform sampler2D NormalTexture;
uniform sampler2D WorldTexture;

void main() {
	vec4 Diffuse = texture2D(DiffuseTexture,VS_TexCoord1);	//Diffuse Vec4
	vec4 Normal = texture2D(NormalTexture,VS_TexCoord1);	//Normal Vec4
	vec4 World = texture2D(WorldTexture,VS_TexCoord1);		//World Vec4

	outColour = Diffuse * VS_Colour;
	if (outColour.a < 0.1f){discard;}
}