#version 150

in vec4 Colour;
in vec2 TexCoord;

uniform sampler2D DiffuseTexture;

void main()
{
	vec4 texel = texture(DiffuseTexture,TexCoord) * Colour;
	if (texel.a <= 0.1f){discard;}
	gl_FragColor = texel;
}