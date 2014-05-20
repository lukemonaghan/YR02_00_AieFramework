#version 330

in vec4 viewPosition;
in vec4 viewNormal;

out vec4 positionMap;
out vec4 normalMap;

void main(){
	positionMap = viewPosition;
	normalMap = vec4(normalize(viewNormal.xyz) * 0.5f + 0.5f,1);
}