#version 330

in vec2 texCoord;
in vec3 normal;
in vec4 wPos;

out vec4 oDiffuse;
out vec4 oNormal;
out vec4 oWorld;

uniform sampler2D DiffuseTexture;

void main() {
	oDiffuse = texture2D(DiffuseTexture,texCoord);
	oNormal = vec4(normal, 1);
	oWorld = wPos;
}