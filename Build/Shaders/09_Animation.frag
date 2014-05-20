#version 150

in vec2 TexCoord;

uniform sampler2D DiffuseTexture;

void main()
{
	gl_FragColor = texture( DiffuseTexture, TexCoord );
}