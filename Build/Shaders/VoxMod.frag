#version 130
 
in vec3 GSTexCoord1;

out vec4 outColor;

uniform samplerCube DiffuseTexture;

void main() {
	vec4 OC = texture(DiffuseTexture, GSTexCoord1);
	if (OC.a < 0.2f){discard;}
    outColor = OC;
}

