#version 330

in vec2 eTexCoord;
in vec4 vColour;

out vec4 fragColour;

uniform sampler2D diffuseMap;

void main() {
	fragColour = vec4( texture(diffuseMap,eTexCoord).rrr,1);
}