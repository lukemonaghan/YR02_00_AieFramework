#version 330

in vec4 Position;
in vec4 Normal;
in vec2 TexCoord1;

out vec2 VS_TexCoord1;
out vec3 VS_Weight1;
out vec3 VS_Weight2;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model = mat4(1);
uniform float Time = 0;

uniform vec3 lightDirection = vec3(0,0,0);

void main() {
	// standard projection * view * world multiplication
	gl_Position = Projection * View * Model * Position;

	// transform normal to world space with inverse-transpose world / global matrix
	mat4 invWorld = inverse(Model);
	vec3 N = normalize(invWorld * Normal).xyz;

	// pass through texture coords
	// Note: we're scaling the coordinate to tile the texture across the sphere
	// if we didn't scale then the hatching is too thick
	// this isn't always needed for meshes, but is for the sphere
	// You will need to load the texture with wrapping for its address mode!
	VS_TexCoord1 = TexCoord1 * 6.0f;

	// Lambert lighting calculation
	vec3 LD = lightDirection;
	if (LD == vec3(0)){
		LD.x = cos(Time) * 2;
		LD.z = sin(Time) * 2;
	}
	float diffuse = max( 0.0f, dot(normalize(LD), N) );

	// taking the 0-1 diffuse and mapping it to 0-6 for easy interpolation
	float tone = diffuse * 6.0;

	// smoothstep smoothly interpolates between 0-1 as tone goes between the two given values
	// so in this first line, a tone of 4 will return 0 and a tone of 6 will return 1
	VS_Weight1.x = 1 - smoothstep(4, 6, tone);
	VS_Weight1.y = 1 - smoothstep(3, 5, tone);
	VS_Weight1.z = 1 - smoothstep(2, 4, tone);
	VS_Weight2.x = 1 - smoothstep(1, 3, tone);
	VS_Weight2.y = 1 - smoothstep(0, 2, tone);
	VS_Weight2.z = 1 - smoothstep(0, 1, tone);
}
