#version 150

in vec4 vColour;
in vec4 Position;
in vec2 texCoord;

out vec4 OutColour;

uniform sampler2D textureMap;

void main(void){
		
	gl_FragColor = texture(textureMap,texCoord) * vColour;
}