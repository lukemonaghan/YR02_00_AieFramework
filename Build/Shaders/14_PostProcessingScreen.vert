#version 330

in vec4 Position;
	
out vec2 textureCoordinate;
	
void main()
{
	textureCoordinate = Position.xy * 0.5 + 0.5;
	gl_Position = Position;
}