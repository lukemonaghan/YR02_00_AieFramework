#version 330

in vec2 VS_TexCoord1;
in vec3 VS_Weight1;
in vec3 VS_Weight2;

out vec4 fragColour;

uniform sampler2D hatchSampler0;
uniform sampler2D hatchSampler1;

void main() {
	float grey = 0.0f;

	grey += dot(VS_Weight1, texture(hatchSampler0, VS_TexCoord1).rgb);
	grey += dot(VS_Weight2, texture(hatchSampler1, VS_TexCoord1).rgb);

	vec3 temp = VS_Weight1 + VS_Weight2;
	float white = 1.0f - (temp.x + temp.y + temp.z);
	grey += white;

	fragColour = vec4( grey, grey, grey, 1 );
}