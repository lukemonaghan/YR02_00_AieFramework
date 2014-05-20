#version 330

in vec4 Position;
in vec3 Normal;
in vec3 Tangent;
in vec3 BiNormal;
in vec2 TexCoord1;

out vec3 VS_Normal; 
out vec3 VS_LightDir;
out vec2 VS_TexCoord1;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform mat3 NormalMatrix;

uniform vec3 LightPosition = vec3(0,2,0);

void main() {
	mat4 MV = View * Model;

	// calculate the surface normals in eye coordinates
	vec3 eye_normal = NormalMatrix * Normal;
	vec3 eye_tangent = NormalMatrix * Tangent;
	vec3 eye_binormal = NormalMatrix * BiNormal;
	
	// get the vertex position and Light position in eye coordinates
	vec3 eye_position = (MV * Position).xyz;
	vec3 eye_lightPos = (MV * vec4(LightPosition, 1)).xyz;
	
	// calculate the dir to the light from the vertex position
	vec3 lightDir = normalize(eye_lightPos - eye_position );
	//vec3 lightDir = normalize(eye_lightPos - Position.xyz );
	
	// use dot product with light direction and normals
	// this is used to adjust shading based on the direction 
	// of the surface
	VS_LightDir.x = dot(lightDir, eye_tangent);
	VS_LightDir.y = dot(lightDir, eye_binormal);
	VS_LightDir.z = dot(lightDir, eye_normal);

	// pass through the surface normal to the frag shader
	VS_Normal = eye_normal;
	
	// pass through the texcoord to the frag shader
	VS_TexCoord1 = TexCoord1;

	// finally set the position over the vertex
	gl_Position = Projection * View * Model * Position;

}