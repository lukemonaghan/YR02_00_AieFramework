#version 330

in vec4 normals;
in vec2 texCoord;
in vec4 shadowCoord;

out vec4 FragColor;

uniform vec3 ambientLight;
uniform vec4 lightDir;

uniform sampler2D diffuseMap;
uniform sampler2D shadowMap;

const float SHADOW_BIAS = 0.007f;

void main() {
	float shadowFactor = 1;

	// calculate shadow by testing depth
	if ( texture( shadowMap, shadowCoord.xy ).z < shadowCoord.z - SHADOW_BIAS ){ shadowFactor = 0; }
	//shadowFactor += texture( shadowMap, shadowCoord.xy ).z + (shadowCoord.z - SHADOW_BIAS);
	// sample diffuse texture and perform lambert lighting
	float NdL = max( 0, dot( normalize(normals), normalize(-lightDir) ));
	vec3 texColour = texture( diffuseMap, texCoord ).rgb;

	// combine diffuse + ambient
	FragColor.rgb = texColour * NdL * shadowFactor + texColour * ambientLight;
	FragColor.a = 1;
}