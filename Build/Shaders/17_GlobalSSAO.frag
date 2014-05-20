#version 330

in vec2 VS_TexCoord1;

out vec4 fragColour;

uniform sampler2D positionMap;
uniform sampler2D normalMap;
uniform sampler2D randomMap;

uniform float uradius;
uniform float ubias;
uniform float uintensity;
uniform float uscale;

float calculateOcclusion(in vec2 offset, in vec3 viewPosition, in vec3 viewNormal) {
	// get the difference between the two fragment positions
	vec3 diff = texture( positionMap, VS_TexCoord1 + offset ).rgb - viewPosition;
	vec3 v = normalize( diff );

	// scale the distance
	float d = length( diff ) * uscale;

	// calculate occlusion if fragment is within a positive dot product
	return max( 0.0f, dot( viewNormal, v ) - ubias ) * (1.0f / (1.0f + d)) * uintensity;
}

void main() {
	// sample view-space textures
	vec3 viewPosition = texture( positionMap, VS_TexCoord1 ).rgb;
	vec3 viewNormal = normalize( texture( normalMap, VS_TexCoord1 ).rgb * 2 - 1 );
	vec2 randomSize = textureSize( randomMap, 0 );
	vec2 rand = normalize(texture(randomMap, VS_TexCoord1.xy / randomSize).xy * 2.0f - 1.0f);

	// base radius off distance to fragment
	float radius = uradius / viewPosition.z;
	
	// simple cross kernel
	vec2 crossKernel[4];
	crossKernel[0] = vec2(1,0);
	crossKernel[1] = vec2(-1,0);
	crossKernel[2] = vec2(0,1);
	crossKernel[3] = vec2(0,-1);

	// perform 16 occlusion tests, accumulating occlusion
	float ao = 0.0f;
	int iterations = 4; // can reduce this for lower quality / higher performance
	for (int j = 0; j < iterations; ++j){
		// scale offsets
		vec2 offset1 = reflect(crossKernel[j],rand) * radius;

		// rotate the offset
		vec2 offset2 = vec2(offset1.x * 0.707f - offset1.y * 0.707f,
			offset1.x * 0.707f + offset1.y * 0.707f);
  
		// use randomised cross kernel to sample occlusion
		ao += calculateOcclusion(offset1 * 0.25f, viewPosition, viewNormal);
		ao += calculateOcclusion(offset2 * 0.5f, viewPosition, viewNormal);
		ao += calculateOcclusion(offset1 * 0.75f, viewPosition, viewNormal);
		ao += calculateOcclusion(offset2, viewPosition, viewNormal);
	}

	// average occlusion
	ao /= iterations * 4.0f;
	ao = clamp(ao, 0.0, 1.0);
	// just display the occlusion
	fragColour.rgb = vec3(1 - ao);
	fragColour.a = 1;
}