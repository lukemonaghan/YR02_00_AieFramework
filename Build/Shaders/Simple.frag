#version 330

in vec2 VS_TexCoord1;
in vec4 VS_Color;

out vec4 outColor;

uniform sampler2D DiffuseTexture;

void main() { 
	outColor = texture2D( DiffuseTexture, VS_TexCoord1 ) * VS_Color;
	if (outColor.a < 0.1f){discard;}
}