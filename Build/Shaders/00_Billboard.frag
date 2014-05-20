#version 150 

in vec2 VS_UV;
in vec4 VS_Color;

out vec4 outColor;

uniform sampler2D DiffuseTexture;

void main() { 
	vec4 Color = texture2D( DiffuseTexture, VS_UV ) * VS_Color;
	if (Color.a < 0.5f){discard;}
	outColor = Color;
}