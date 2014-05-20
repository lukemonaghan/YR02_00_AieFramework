#version 330

in vec2 vUV;
in vec4 vColor;

out vec4 outColor;

uniform sampler2D DiffuseTexture;

void main() 
{ 
	vec4 Texel = texture2D( DiffuseTexture, vUV ) * vColor;
	//if (Texel.a < 0.5f){discard;}
	outColor = Texel;
}