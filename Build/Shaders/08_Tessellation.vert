#version 330

layout( location = 0 ) in vec4 position;
layout( location = 7 ) in vec2 texCoord;

out vec2 TexCoord;

void main()
{
	TexCoord = texCoord;
	gl_Position = position;
}