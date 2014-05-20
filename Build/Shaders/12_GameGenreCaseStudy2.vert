#version 330

layout( location = 0 ) in vec2 vert;

uniform vec2 extents;
uniform vec2 center;

uniform vec2 screenDimensions;

void main()
{
	vec2 p = vert * extents + center;
	gl_Position = vec4(p / screenDimensions * 2 - 1,0,1);
}