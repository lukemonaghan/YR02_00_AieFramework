#version 330
 
in vec3 TexCoord1;

out vec4 outColor;

uniform samplerCube CubeMap;

void main (void) {
	vec4 OC = texture(CubeMap, TexCoord1);
	if (OC.a < 0.5f){discard;}
    outColor = OC;
}