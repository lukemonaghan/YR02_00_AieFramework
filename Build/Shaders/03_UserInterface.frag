#version 330

uniform vec4 colour;

in vec2 texCoord;
out vec4 fragColour;

uniform sampler2D diffuseMap;

void main(){
	fragColour = texture(diffuseMap,texCoord) * colour;
}

