#version 330

in vec4 VS_Colour;
in vec4 VS_Normal;
in vec4 VS_Position;

out vec4 oDiffuse;
out vec4 oNormal;
out vec4 oWorld;

void main() {
	oDiffuse = VS_Colour;
	oNormal = VS_Normal;
	oWorld = VS_Position;
}