#version 330

in vec2 vUV;
in vec4 vColor;
in vec4 vPosition;

out vec4 outColor;

uniform sampler2D DiffuseTexture;
uniform sampler2D SpecularTexture;

void main() 
{ 
	outColor = texture2D( DiffuseTexture, vUV.xy ) * vColor;
	outColor.r += vPosition.y/10.0f;
	outColor.g += vPosition.y/10.0f;
	outColor.b += vPosition.y/10.0f;
	outColor.a = 1.0f;
}